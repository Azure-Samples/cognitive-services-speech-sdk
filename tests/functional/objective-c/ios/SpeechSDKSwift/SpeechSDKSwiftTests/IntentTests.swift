//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest

class IntentEndToEndTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""
    var languageUnderstandingAppId: String! = ""
    let timeoutInSeconds = 20.0
    let weatherFileName = "whatstheweatherlike"
    let weatherTextEnglish = "What's the weather like?"

    var result: [String: String] = ["finalText": "", "intentId": "", "reason": ""]
    var counts: [String: Int] = ["connectedCount": 0,
                                 "disconnectedCount": 0,
                                 "finalResultCount": 0,
                                 "sessionStartedCount": 0,
                                 "sessionStoppedCount": 0]

    var connection: SPXConnection? = nil
    var reco: SPXIntentRecognizer? = nil

    var sessionStoppedCountPred: NSPredicate? = nil
    var disconnectedPred: NSPredicate? = nil
    var connectedPred: NSPredicate? = nil

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["luisKey"]
        serviceRegion = ProcessInfo.processInfo.environment["luisRegion"]
        languageUnderstandingAppId = ProcessInfo.processInfo.environment["languageUnderstandingAppId"]
        let filePath = Bundle.main.url(forResource: weatherFileName, withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let intentConfig = try! SPXSpeechConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        intentConfig.speechRecognitionLanguage = "en-US"

        self.reco = try! SPXIntentRecognizer(speechConfiguration: intentConfig, audioConfiguration: audioConfig!)

        let model = SPXLanguageUnderstandingModel(appId: languageUnderstandingAppId)
        reco?.addIntent("HomeAutomation.TurnOn", from: model!)
        reco?.addIntent("HomeAutomation.TurnOff", from: model!)

        reco?.addIntent(fromPhrase: "This is a test.", mappingToId: "test")
        reco?.addIntent(fromPhrase: "Switch the to channel 34.", mappingToId: "34")
        reco?.addIntent(fromPhrase: "what's the weather like", mappingToId: "weather")

        self.connection = try! SPXConnection(from: reco!)

        // NB: Intent does not support opening the connection in advance
        self.connection!.addConnectedEventHandler() {[unowned self] _, evt in
            print("connected to session \(evt.sessionId)!")
            self.counts["connectedCount"]! += 1
        }

        self.connection!.addDisconnectedEventHandler() {[unowned self] _, evt in
            print("disconnected from session \(evt.sessionId)!")
            self.counts["disconnectedCount"]! += 1
        }

        self.reco!.addRecognizedEventHandler() {[unowned self] _, evt in
            print("received result: \(evt.result?.text ?? "*no text*") with intent \(String(describing: evt.result?.intentId)) with reason \(evt.result!.reason.rawValue)")
            if (evt.result!.reason == SPXResultReason.canceled || evt.result!.reason == SPXResultReason.noMatch) {
                return
            }

            self.result["finalText"]! = evt.result!.text!
            self.result["intentId"]! = evt.result!.intentId!
            self.result["reason"]! = "\(evt.result!.reason.rawValue)"
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
            return (1 == (evaluatedObject as! IntentEndToEndTests).counts["sessionStoppedCount"])
        }

        sessionStoppedCountPred = NSPredicate(block: sessionStoppedCountPredBlock)
        disconnectedPred = NSPredicate(block: {evaluatedObject, _ in
            return (1 == (evaluatedObject as! IntentEndToEndTests).counts["disconnectedCount"])
        })
        connectedPred = NSPredicate(block: {evaluatedObject, _ in
            return (1 == (evaluatedObject as! IntentEndToEndTests).counts["connectedCount"])
        })
    }

    func testRecognizeOnce() {
        let intentResult = try? self.reco!.recognizeOnce()

        XCTAssertEqual(intentResult!.text, weatherTextEnglish)
        XCTAssertEqual(intentResult!.intentId, "weather")
        XCTAssertEqual(intentResult!.reason, SPXResultReason.recognizedSpeech)
        XCTAssertGreaterThan(intentResult!.duration, 0)
        XCTAssertGreaterThan(intentResult!.offset, 0)
        XCTAssertGreaterThan(intentResult!.resultId.count, 0)
        }

    func testContinuous () {
        try! self.reco?.startContinuousRecognition()

        self.expectation(for: sessionStoppedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        XCTAssertEqual(self.result["finalText"], self.weatherTextEnglish);
        XCTAssertEqual(self.result["intentId"], "weather")
        XCTAssertEqual(self.result["reason"], "\(SPXResultReason.recognizedSpeech.rawValue)")
        XCTAssertEqual(self.counts["finalResultCount"], 1);
        XCTAssertGreaterThan(self.counts["connectedCount"]!, 0);

        try! self.reco?.stopContinuousRecognition()

        XCTAssertGreaterThanOrEqual(self.counts["connectedCount"]!, self.counts["disconnectedCount"]!);
    }
}
