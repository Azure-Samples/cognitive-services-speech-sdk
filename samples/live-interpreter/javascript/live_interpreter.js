// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

const fs = require("node:fs");
const sdk = require("microsoft-cognitiveservices-speech-sdk");

function requiredEnvironment(name) {
    const value = process.env[name];
    if (!value) {
        throw new Error(`Set ${name} before running this sample.`);
    }
    return value;
}

function createTranslationConfig(speechKey, endpoint) {
    const config = sdk.SpeechTranslationConfig.fromEndpoint(
        new URL(endpoint), speechKey
    );
    config.addTargetLanguage("fr");
    config.voiceName = "personal-voice";
    config.speechSynthesisOutputFormat =
        sdk.SpeechSynthesisOutputFormat.Riff16Khz16BitMonoPcm;
    return config;
}

function writeAudio(file, audio) {
    fs.writeFileSync(file, audio, { flag: "wx" });
}

async function main() {
    const speechKey = requiredEnvironment("SPEECH_KEY");
    const endpoint = requiredEnvironment("SPEECH_ENDPOINT");
    const audioFile = requiredEnvironment("LIVE_INTERPRETER_AUDIO_FILE");
    const translationConfig = createTranslationConfig(speechKey, endpoint);
    const autoDetectConfig = sdk.AutoDetectSourceLanguageConfig.fromOpenRange();
    const audioConfig = sdk.AudioConfig.fromWavFileInput(
        fs.readFileSync(audioFile), audioFile
    );
    const recognizer = sdk.TranslationRecognizer.FromConfig(
        translationConfig, autoDetectConfig, audioConfig
    );
    let audioIndex = 0;
    let failure;

    try {
        await new Promise((resolve, reject) => {
            let finished = false;
            const finish = error => {
                if (finished) {
                    return;
                }
                finished = true;
                clearTimeout(timeout);
                if (error) {
                    reject(error);
                } else {
                    resolve();
                }
            };
            const timeout = setTimeout(
                () => finish(new Error("Translation timed out after five minutes.")),
                300000
            );
            const logResult = (label, result) => {
                console.log(`${label} in '${result.language}': ${result.text}`);
                for (const language of result.translations.languages) {
                    console.log(
                        `    TRANSLATED into '${language}': ` +
                        result.translations.get(language)
                    );
                }
            };
            recognizer.recognizing = (sender, event) => {
                logResult("RECOGNIZING", event.result);
            };
            recognizer.recognized = (sender, event) => {
                if (event.result.reason === sdk.ResultReason.TranslatedSpeech) {
                    logResult("RECOGNIZED", event.result);
                } else if (event.result.reason === sdk.ResultReason.NoMatch) {
                    console.log("NOMATCH: Speech couldn't be recognized.");
                }
            };
            recognizer.synthesizing = (sender, event) => {
                const audio = event.result.audio;
                if (audio.byteLength === 0) {
                    return;
                }
                const file = `translation-${++audioIndex}.wav`;
                try {
                    writeAudio(file, Buffer.from(audio));
                    console.log(`Wrote ${file}: ${audio.byteLength} bytes`);
                } catch (error) {
                    failure = error;
                    finish(error);
                }
            };
            recognizer.canceled = (sender, event) => {
                console.log(`CANCELED: Reason=${event.reason}`);
                if (event.reason === sdk.CancellationReason.Error) {
                    failure = new Error(
                        `ErrorCode=${event.errorCode}, ` +
                        `ErrorDetails=${event.errorDetails}`
                    );
                }
                finish(failure);
            };
            recognizer.sessionStopped = () => finish(failure);
            recognizer.startContinuousRecognitionAsync(
                () => console.log("Translation started."),
                error => finish(new Error(String(error)))
            );
        });
    } finally {
        try {
            await new Promise((resolve, reject) => {
                const timeout = setTimeout(
                    () => reject(new Error("Stopping translation timed out.")),
                    30000
                );
                recognizer.stopContinuousRecognitionAsync(
                    () => {
                        clearTimeout(timeout);
                        resolve();
                    },
                    error => {
                        clearTimeout(timeout);
                        reject(new Error(String(error)));
                    }
                );
            });
        } finally {
            recognizer.close();
            audioConfig.close();
            translationConfig.close();
        }
    }
    if (failure) {
        throw failure;
    }
}

module.exports = { createTranslationConfig, requiredEnvironment, writeAudio };

if (require.main === module) {
    main().catch(error => {
        console.error(`Live Interpreter failed: ${error.message}`);
        process.exitCode = 1;
    });
}
