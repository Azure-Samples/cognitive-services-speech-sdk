// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import Darwin
import Foundation
import MicrosoftCognitiveServicesSpeech

func interpreterError(_ message: String) -> NSError {
    NSError(domain: "LiveInterpreter", code: 1, userInfo: [
        NSLocalizedDescriptionKey: message
    ])
}

func requiredEnvironmentVariable(_ name: String) throws -> String {
    guard let value = ProcessInfo.processInfo.environment[name], !value.isEmpty else {
        throw interpreterError("Missing environment variable: \(name)")
    }
    return value
}

final class InterpreterState {
    // Access only on stateQueue, including after stopContinuousRecognition returns.
    var audioIndex = 0
    var translationCount = 0
    var failure: Error?
    var stopping = false
    var closed = false
}

func runLiveInterpreter() throws {
    let speechKey = try requiredEnvironmentVariable("SPEECH_KEY")
    let endpoint = try requiredEnvironmentVariable("SPEECH_ENDPOINT")
    let audioFile = try requiredEnvironmentVariable("LIVE_INTERPRETER_AUDIO_FILE")

    let translationConfig = try SPXSpeechTranslationConfiguration(
        endpoint: endpoint, subscription: speechKey)
    translationConfig.addTargetLanguage("fr")
    translationConfig.voiceName = "personal-voice"
    translationConfig.setSpeechSynthesisOutputFormat(.riff16Khz16BitMonoPcm)

    let autoDetectConfig = try SPXAutoDetectSourceLanguageConfiguration(openRange: ())
    guard let audioConfig = SPXAudioConfiguration(wavFileInput: audioFile) else {
        throw interpreterError("Couldn't open WAV input: \(audioFile)")
    }
    let recognizer = try SPXTranslationRecognizer(
        speechTranslationConfiguration: translationConfig,
        autoDetectSourceLanguageConfiguration: autoDetectConfig,
        audioConfiguration: audioConfig)

    let stopped = DispatchSemaphore(value: 0)
    let stateQueue = DispatchQueue(label: "live-interpreter")
    let state = InterpreterState()

    recognizer.addRecognizingEventHandler { _, event in
        stateQueue.sync {
            guard !state.closed, state.failure == nil else { return }
            print("RECOGNIZING: \(event.result.text ?? "")")
            print("TRANSLATING: \(event.result.translations)")
        }
    }
    recognizer.addRecognizedEventHandler { _, event in
        stateQueue.sync {
            guard !state.closed, state.failure == nil else { return }
            if event.result.reason == .noMatch {
                print("NO MATCH: no speech recognized in this segment.")
                return
            }
            guard event.result.reason == .translatedSpeech else {
                state.failure = interpreterError("Recognition failed: result reason=\(event.result.reason)")
                stopped.signal()
                return
            }
            let language = event.result.properties?.getPropertyBy(
                .speechServiceConnectionAutoDetectSourceLanguageResult)
            guard let text = event.result.text, !text.isEmpty,
                  let language = language, !language.isEmpty,
                  let translation = event.result.translations["fr"] as? String, !translation.isEmpty else {
                state.failure = interpreterError(
                    "Recognition failed: missing recognized text, detected language, or French translation.")
                stopped.signal()
                return
            }
            state.translationCount += 1
            print("RECOGNIZED in '\(language)': \(text)")
            print("TRANSLATED (fr): \(translation)")
        }
    }
    recognizer.addSynthesizingEventHandler { _, event in
        stateQueue.sync {
            guard !state.closed, state.failure == nil else { return }
            guard event.result.reason != .canceled else {
                state.failure = interpreterError("Translation synthesis failed: result reason=Canceled.")
                stopped.signal()
                return
            }
            // Empty audio ends an utterance, not the continuous recognition session.
            guard let audio = event.result.audio, !audio.isEmpty else { return }
            let outputFile = "translation-\(state.audioIndex + 1).wav"
            do {
                try audio.write(to: URL(fileURLWithPath: outputFile), options: .withoutOverwriting)
                state.audioIndex += 1
                print("Wrote \(outputFile) (\(audio.count) bytes)")
            } catch {
                state.failure = interpreterError("Couldn't create \(outputFile): \(error.localizedDescription)")
                stopped.signal()
            }
        }
    }
    recognizer.addCanceledEventHandler { _, event in
        stateQueue.sync {
            guard !state.closed else { return }
            print("CANCELED: Reason=\(event.reason)")
            if event.reason == .error || (event.reason != .endOfStream && !state.stopping) {
                let details = event.errorDetails ?? "No error details."
                let failure = interpreterError(
                    "Translation canceled: ErrorCode=\(event.errorCode): \(details)")
                print(failure.localizedDescription)
                if state.failure == nil {
                    state.failure = failure
                }
            }
            stopped.signal()
        }
    }
    recognizer.addSessionStoppedEventHandler { _, _ in
        stateQueue.sync {
            guard !state.closed else { return }
            print("SESSION STOPPED")
            stopped.signal()
        }
    }

    let deadline = DispatchTime.now() + 300
    // Bound synchronous SDK start/stop calls as well as the completion wait.
    DispatchQueue.global(qos: .userInitiated).asyncAfter(deadline: deadline) {
        fputs("Live Interpreter timed out after five minutes.\n", stderr)
        _exit(1)
    }
    try recognizer.startContinuousRecognition()
    let waitResult = stopped.wait(timeout: deadline)
    stateQueue.sync {
        state.stopping = true
        if waitResult == .timedOut {
            state.failure = interpreterError("Translation timed out after five minutes.")
        }
    }
    // Never stop the recognizer from an SDK callback or while holding stateQueue.
    try recognizer.stopContinuousRecognition()
    let failure: Error? = stateQueue.sync {
        state.closed = true
        if state.failure == nil && (state.translationCount == 0 || state.audioIndex == 0) {
            state.failure = interpreterError(
                "Translation failed: no translated speech or synthesized audio was produced.")
        }
        return state.failure
    }
    if let failure = failure {
        throw failure
    }
}

do {
    try runLiveInterpreter()
} catch {
    fputs("Live Interpreter failed: \(error.localizedDescription)\n", stderr)
    exit(1)
}
