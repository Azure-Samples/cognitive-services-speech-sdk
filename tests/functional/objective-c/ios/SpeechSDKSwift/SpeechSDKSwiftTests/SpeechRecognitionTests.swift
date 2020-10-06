//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest
import AVFoundation

class SpeechRecognitionTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
    }

    func testEmptyRegion() {
        XCTAssertThrowsError(try SPXSpeechConfiguration(subscription: "InvalidSubscription", region: "")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testEmptyKey() {
        XCTAssertThrowsError(try SPXSpeechConfiguration(subscription: "", region: "westus")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testEmptyEndpoint() {
        XCTAssertThrowsError(try SPXSpeechConfiguration(endpoint: "")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
        XCTAssertThrowsError(try SPXSpeechConfiguration(endpoint: "", subscription: speechKey)) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testInvalidSubscriptionKey() {
        let audioFilename = "whatstheweatherlike"
        let filePath = Bundle.main.url(forResource: audioFilename, withExtension: "wav")!

        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:"InvalidSubscriptionKey", region:"westus")
        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!)

        let result = try! reco.recognizeOnce()

        print("received result: \(result.text ?? "*no text*") with reason \(result.reason)")

        let cancellationDetails = try! SPXCancellationDetails(fromCanceledRecognitionResult: result)

        XCTAssertEqual(result.text, "");
        XCTAssertEqual(cancellationDetails.reason, SPXCancellationReason.error);
        XCTAssertTrue(cancellationDetails.errorDetails!.range(of: "websocket upgrade failed", options: .caseInsensitive) != nil);
        XCTAssertTrue(cancellationDetails.errorDetails!.range(of: "authentication error", options: .caseInsensitive)  != nil);
        XCTAssertTrue(cancellationDetails.errorDetails!.range(of: "subscription", options: .caseInsensitive) != nil);
        XCTAssertEqual(result.reason, SPXResultReason.canceled);
        XCTAssertEqual(result.duration, 0);
        XCTAssertEqual(result.offset, 0);
        XCTAssertGreaterThan(result.resultId.count, 0);
    }

    func testInvalidRegion() {
        let audioFilename = "whatstheweatherlike"
        let filePath = Bundle.main.url(forResource: audioFilename, withExtension: "wav")!

        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:"InvalidSubscriptionKey", region:"InvalidRegion")
        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!)

        let result = try! reco.recognizeOnce()
        print("received result: \(result.text ?? "*no text*") with reason \(result.reason)")

        let cancellationDetails = try! SPXCancellationDetails(fromCanceledRecognitionResult: result)

        XCTAssertEqual(result.text, "");
        XCTAssertEqual(cancellationDetails.reason, SPXCancellationReason.error);
        XCTAssertTrue(cancellationDetails.errorDetails!.range(of: "connection failed", options: .caseInsensitive) != nil);
        XCTAssertEqual(result.reason, SPXResultReason.canceled);
        XCTAssertEqual(result.duration, 0);
        XCTAssertEqual(result.offset, 0);
        XCTAssertGreaterThan(result.resultId.count, 0);
    }

    func testInvalidWavefileName() {
        let audioConfig = SPXAudioConfiguration(wavFileInput: "invalidFilename")
        let speechConfig = try! SPXSpeechConfiguration(subscription:"InvalidSubscriptionKey", region:"InvalidRegion")
        XCTAssertThrowsError(try SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!))  { error in
            XCTAssertTrue(error.localizedDescription.contains("Error: Exception with an error code: 0x8 (SPXERR_FILE_OPEN_FAILED)"))
        }
    }

    func testNoMatchResultAndDetails() {
        let audioFilename = "silenceshort"
        let filePath = Bundle.main.url(forResource: audioFilename, withExtension: "wav")!

        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!)

        let result = try! reco.recognizeOnce()
        print("received result: \(result.text ?? "*no text*") with reason \(result.reason)")

        let nomatchDetails = try! SPXNoMatchDetails(fromNoMatch: result)

        XCTAssertEqual(result.text, "");
        XCTAssertEqual(nomatchDetails.reason, SPXNoMatchReason.initialSilenceTimeout);
        XCTAssertEqual(result.reason, SPXResultReason.noMatch);
        XCTAssertTrue(result.duration > 0 || result.offset > 0, "offset: \(result.offset), duration: \(result.duration)");
        XCTAssertGreaterThan(result.resultId.count, 0);
    }

    func testSetProxyConfig() {
        let speechConfig = try! SPXSpeechConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        try! speechConfig.setProxyUsingHost("proxy.xyz", port: 8080, userName: "", password: "")
        try! speechConfig.setProxyUsingHost("proxy.xyz", port: 8080, userName: "users", password: "pass")

        XCTAssertThrowsError(try speechConfig.setProxyUsingHost("proxy.xyz", port: 8080, userName: nil, password: nil))  { error in
            XCTAssertTrue(error.localizedDescription.contains("Error: Proxy user name or password is null."))
        }
    }
}

class TranslationRecognitionTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
    }

    func testEmptyRegion() {
        XCTAssertThrowsError(try SPXSpeechTranslationConfiguration(subscription: "InvalidSubscription", region: "")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testEmptyKey() {
        XCTAssertThrowsError(try SPXSpeechTranslationConfiguration(subscription: "", region: "westus")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testEmptyEndpoint() {
        XCTAssertThrowsError(try SPXSpeechTranslationConfiguration(endpoint: "")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
        XCTAssertThrowsError(try SPXSpeechTranslationConfiguration(endpoint: "", subscription: speechKey)) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testInvalidSubscriptionKey() {
        let audioFilename = "whatstheweatherlike"
        let filePath = Bundle.main.url(forResource: audioFilename, withExtension: "wav")!

        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let translationConfig = try! SPXSpeechTranslationConfiguration(subscription:"InvalidSubscriptionKey", region:"westus")
        translationConfig.addTargetLanguage("fr-FR")
        let reco = try! SPXTranslationRecognizer(speechTranslationConfiguration: translationConfig, audioConfiguration: audioConfig!)

        let result = try! reco.recognizeOnce()

        print("received result: \(result.text ?? "*no text*") with reason \(result.reason)")

        let cancellationDetails = try! SPXCancellationDetails(fromCanceledRecognitionResult: result)

        XCTAssertEqual(result.text, "");
        XCTAssertEqual(cancellationDetails.reason, SPXCancellationReason.error);
        XCTAssertTrue(cancellationDetails.errorDetails!.range(of: "websocket upgrade failed", options: .caseInsensitive) != nil);
        XCTAssertTrue(cancellationDetails.errorDetails!.range(of: "authentication error", options: .caseInsensitive)  != nil);
        XCTAssertEqual(result.reason, SPXResultReason.canceled);
        XCTAssertEqual(result.duration, 0);
        XCTAssertEqual(result.offset, 0);
        XCTAssertGreaterThan(result.resultId.count, 0);
    }

    func testInvalidRegion() {
        let audioFilename = "whatstheweatherlike"
        let filePath = Bundle.main.url(forResource: audioFilename, withExtension: "wav")!

        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let translationConfig = try! SPXSpeechTranslationConfiguration(subscription:"InvalidSubscriptionKey", region:"InvalidRegion")
        translationConfig.addTargetLanguage("fr-FR")
        let reco = try! SPXTranslationRecognizer(speechTranslationConfiguration: translationConfig, audioConfiguration: audioConfig!)

        let result = try! reco.recognizeOnce()
        print("received result: \(result.text ?? "*no text*") with reason \(result.reason)")

        let cancellationDetails = try! SPXCancellationDetails(fromCanceledRecognitionResult: result)

        XCTAssertEqual(result.text, "");
        XCTAssertEqual(cancellationDetails.reason, SPXCancellationReason.error);
        XCTAssertTrue(cancellationDetails.errorDetails!.contains(
            "Connection failed (no connection to the remote host)."),
                      cancellationDetails.errorDetails!);
        XCTAssertEqual(result.reason, SPXResultReason.canceled);
        XCTAssertEqual(result.duration, 0);
        XCTAssertEqual(result.offset, 0);
        XCTAssertGreaterThan(result.resultId.count, 0);
    }

    func testInvalidWavefileName() {
        let audioConfig = SPXAudioConfiguration(wavFileInput: "invalidFilename")
        let translationConfig = try! SPXSpeechTranslationConfiguration(subscription:"InvalidSubscriptionKey", region:"InvalidRegion")
        translationConfig.addTargetLanguage("fr-FR")
        XCTAssertThrowsError(try SPXTranslationRecognizer(speechTranslationConfiguration:translationConfig, audioConfiguration: audioConfig!))  { error in
            XCTAssertTrue(error.localizedDescription.contains("Error: Exception with an error code: 0x8 (SPXERR_FILE_OPEN_FAILED)"))
        }
    }

    func testNoMatchResultAndDetails() {
        let audioFilename = "silenceshort"
        let filePath = Bundle.main.url(forResource: audioFilename, withExtension: "wav")!

        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let translationConfig = try! SPXSpeechTranslationConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        translationConfig.addTargetLanguage("fr")
        translationConfig.speechRecognitionLanguage = "en-US"
        let reco = try! SPXTranslationRecognizer(speechTranslationConfiguration: translationConfig, audioConfiguration: audioConfig!)

        let result = try! reco.recognizeOnce()
        print("received result: \(result.text ?? "*no text*") with reason \(result.reason)")

        let nomatchDetails = try! SPXNoMatchDetails(fromNoMatch: result)

        XCTAssertEqual(result.text, "");
        XCTAssertEqual(nomatchDetails.reason, SPXNoMatchReason.initialSilenceTimeout);
        XCTAssertEqual(result.reason, SPXResultReason.noMatch);
        XCTAssertTrue(result.duration > 0 || result.offset > 0, "offset: \(result.offset), duration: \(result.duration)");
        XCTAssertGreaterThan(result.resultId.count, 0);
    }
}

class SpeechRecognitionEndToEndTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""
    let timeoutInSeconds = 20.0
    let weatherFileName = "whatstheweatherlike"
    let weatherTextEnglish = "What's the weather like?"

    var result: [String: String] = ["finalText": ""]
    var counts: [String: Int] = ["connectedCount": 0,
                                 "disconnectedCount": 0,
                                 "finalResultCount": 0,
                                 "sessionStartedCount": 0,
                                 "sessionStoppedCount": 0]

    var connection: SPXConnection? = nil
    var reco: SPXSpeechRecognizer? = nil

    var sessionStoppedCountPred: NSPredicate? = nil
    var disconnectedPred: NSPredicate? = nil
    var connectedPred: NSPredicate? = nil

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
        let filePath = Bundle.main.url(forResource: weatherFileName, withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        self.reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!)

        self.connection = try! SPXConnection(from: reco!)

        self.connection!.addConnectedEventHandler() {[unowned self] _, evt in
            print("connected to session \(evt.sessionId)!")
            self.counts["connectedCount"]! += 1
        }

        self.connection!.addDisconnectedEventHandler() {[unowned self] _, evt in
            print("disconnected from session \(evt.sessionId)!")
            self.counts["disconnectedCount"]! += 1
        }

        self.reco!.addRecognizedEventHandler() {[unowned self] _, evt in
            print("received result: \(evt.result.text ?? "*no text*") with reason \(evt.result.reason)")
            self.result["finalText"]! = evt.result.text!
            self.counts["finalResultCount"]! += 1
            print("counts in recog handler: \(self.counts)")
        }

        self.reco!.addSessionStartedEventHandler() {[unowned self] _, evt in
            self.counts["sessionStartedCount"]! += 1
            print("counts in sessionStarted handler: \(self.counts)")
        }

        self.reco!.addSessionStoppedEventHandler() {[unowned self] _, evt in
            self.counts["sessionStoppedCount"]! += 1
            print("counts in sessionStopped handler: \(self.counts)")
        }

        let sessionStoppedCountPredBlock: (Any?, [String : Any]?) -> Bool = {evaluatedObject, _ in
            return (1 == (evaluatedObject as! SpeechRecognitionEndToEndTests).counts["sessionStoppedCount"])
        }

        sessionStoppedCountPred = NSPredicate(block: sessionStoppedCountPredBlock)
        disconnectedPred = NSPredicate(block: {evaluatedObject, _ in
            return (1 == (evaluatedObject as! SpeechRecognitionEndToEndTests).counts["disconnectedCount"])
        })
        connectedPred = NSPredicate(block: {evaluatedObject, _ in
            return (1 == (evaluatedObject as! SpeechRecognitionEndToEndTests).counts["connectedCount"])
        })

    }

    func testRecognizeOnce() {
        let transcription = try? self.reco!.recognizeOnce()

        XCTAssertEqual(transcription!.text, weatherTextEnglish)
        XCTAssertEqual(transcription!.reason, SPXResultReason.recognizedSpeech)
        XCTAssertGreaterThan(transcription!.duration, 0)
        XCTAssertGreaterThan(transcription!.offset, 0)
        XCTAssertGreaterThan(transcription!.resultId.count, 0)
    }

    func testContinuous () {
        try! self.reco?.startContinuousRecognition()

        self.expectation(for: sessionStoppedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        XCTAssertEqual(self.result["finalText"], self.weatherTextEnglish);
        XCTAssertEqual(self.counts["finalResultCount"], 1);
        XCTAssertGreaterThan(self.counts["connectedCount"]!, 0);

        try! self.reco?.stopContinuousRecognition()

        XCTAssertGreaterThanOrEqual(self.counts["connectedCount"]!, self.counts["disconnectedCount"]!);
    }

    func testContinuousWithPreConnection () {
        self.connection!.open(true)
        self.expectation(for: connectedPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        try! self.reco?.startContinuousRecognition()

        self.expectation(for: sessionStoppedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        XCTAssertEqual(self.result["finalText"], self.weatherTextEnglish);
        XCTAssertEqual(self.counts["finalResultCount"], 1);
        XCTAssertGreaterThan(self.counts["connectedCount"]!, 0);

        try! self.reco?.stopContinuousRecognition()

        self.connection!.close()

        self.expectation(for: disconnectedPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        XCTAssertEqual(self.counts["connectedCount"]!, self.counts["disconnectedCount"]!);
    }

    func testSpeechRecognitionWithAutoLanguageDetection() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
        let filePath = Bundle.main.url(forResource: weatherFileName, withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        var autoDetectSrcLanguageConfig: SPXAutoDetectSourceLanguageConfiguration? = nil
        autoDetectSrcLanguageConfig = try! SPXAutoDetectSourceLanguageConfiguration(["en-US", "zh-CN"])
        XCTAssertNotNil(autoDetectSrcLanguageConfig)
        let recognizer = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, autoDetectSourceLanguageConfiguration: autoDetectSrcLanguageConfig!, audioConfiguration: audioConfig!)
        let transcription = try? recognizer.recognizeOnce()
        XCTAssertNotNil(transcription)
        XCTAssertEqual(transcription!.text, weatherTextEnglish)
        XCTAssertEqual(transcription!.reason, SPXResultReason.recognizedSpeech)
        XCTAssertGreaterThan(transcription!.duration, 0)
        XCTAssertGreaterThan(transcription!.offset, 0)
        XCTAssertGreaterThan(transcription!.resultId.count, 0)
        let autoDetectSrcLanguageResult = SPXAutoDetectSourceLanguageResult(transcription!)
        XCTAssertEqual("en-US", autoDetectSrcLanguageResult.language)
    }

    func testSpeechRecognitionWithoutAutoLanguageDetection() {
        let transcription = try? self.reco!.recognizeOnce()
        XCTAssertNotNil(transcription)
        let autoDetectSrcLanguageResult = SPXAutoDetectSourceLanguageResult(transcription!)
        XCTAssertEqual("", autoDetectSrcLanguageResult.language)
    }
}

class SpeechRecognitionPullAudioStreamEndToEndTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""
    let timeoutInSeconds = 20.0
    let weatherFileName = "whatstheweatherlike"
    let weatherTextEnglish = "What's the weather like?"

    var result: [String: String] = ["finalText": ""]
    var counts: [String: Int] = ["finalResultCount": 0,
                                 "sessionStartedCount": 0,
                                 "sessionStoppedCount": 0]

    var sessionStoppedCountPred: NSPredicate? = nil

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
    }
        
    func testContinuousWithPullAudioStream () {
        let filePath = Bundle.main.path(forResource: weatherFileName, ofType: "wav")
        let targetUrl = URL(string: filePath!)
        let audioFile = try! AVAudioFile(forReading: targetUrl!, commonFormat: .pcmFormatInt16, interleaved: false)
        XCTAssertNotNil(audioFile)
        let bytesPerFrame = audioFile.fileFormat.streamDescription.pointee.mBytesPerFrame
        XCTAssertEqual(1, audioFile.fileFormat.channelCount)
        XCTAssertEqual(16000, audioFile.fileFormat.sampleRate)

        let stream: SPXPullAudioInputStream = SPXPullAudioInputStream.init(readHandler: {
            (data:NSMutableData, size:UInt) -> Int in

            let buffer = AVAudioPCMBuffer.init(pcmFormat: audioFile.fileFormat, frameCapacity: AVAudioFrameCount(size)/bytesPerFrame)
            var success = false
            do {
                if buffer != nil {
                    try audioFile.read(into: buffer!)
                    success = true
                } else {
                    print("AVAudioPCMBuffer is nil")
                }
            } catch {
                print("Error when reading AVAudioFile")
            }

            var nBytes = 0
            if success {
                nBytes = Int(buffer!.frameLength * bytesPerFrame)
                var range: NSRange = NSRange()
                range.location = 0
                range.length = nBytes
                XCTAssertEqual(1, buffer!.stride)
                XCTAssertNotEqual(nil, buffer?.int16ChannelData)
                data.replaceBytes(in: range, withBytes: (buffer?.int16ChannelData![0])!)
            }
            return nBytes
        }) {}!

        let audioConfig = SPXAudioConfiguration(streamInput: stream)
        XCTAssertNotNil(audioConfig)
        let speechConfig = try! SPXSpeechConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        XCTAssertNotNil(speechConfig)
        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!)
        XCTAssertNotNil(reco)

        reco.addRecognizedEventHandler() {[unowned self] _, evt in
            print("received result: \(evt.result.text ?? "*no text*") with reason \(evt.result.reason)")
            self.result["finalText"]! = evt.result.text!
            self.counts["finalResultCount"]! += 1
            print("counts in recog handler: \(self.counts)")
        }

        reco.addSessionStartedEventHandler() {[unowned self] _, evt in
            self.counts["sessionStartedCount"]! += 1
            print("counts in sessionStarted handler: \(self.counts)")
        }

        reco.addSessionStoppedEventHandler() {[unowned self] _, evt in
            self.counts["sessionStoppedCount"]! += 1
            print("counts in sessionStopped handler: \(self.counts)")
        }

        let sessionStoppedCountPredBlock: (Any?, [String : Any]?) -> Bool = {evaluatedObject, _ in
            return (1 == (evaluatedObject as! SpeechRecognitionPullAudioStreamEndToEndTests).counts["sessionStoppedCount"])
        }

        sessionStoppedCountPred = NSPredicate(block: sessionStoppedCountPredBlock)
        
        try! reco.startContinuousRecognition()

        self.expectation(for: sessionStoppedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        XCTAssertEqual(self.result["finalText"], self.weatherTextEnglish);
        XCTAssertEqual(self.counts["finalResultCount"], 1);

        try! reco.stopContinuousRecognition()
    }
}
