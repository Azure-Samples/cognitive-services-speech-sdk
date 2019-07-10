//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest

class TranslationEndToEndTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""
    let timeoutInSeconds = 20.0
    let weatherFileName = "whatstheweatherlike"
    let weatherTextEnglish = "What's the weather like?"
    let weatherTextGerman = "Wie ist das Wetter?";
    let weatherTextChinese =  "天气怎么样?";

    var result: [String: String] = ["finalText": ""]
    var counts: [String: Int] = ["connectedCount": 0,
                                 "disconnectedCount": 0,
                                 "finalResultCount": 0,
                                 "sessionStartedCount": 0,
                                 "sessionStoppedCount": 0]
    var translationDictionary: [String: String] = [:];

    var connection: SPXConnection? = nil
    var reco: SPXTranslationRecognizer? = nil

    var sessionStoppedCountPred: NSPredicate? = nil
    var disconnectedPred: NSPredicate? = nil
    var connectedPred: NSPredicate? = nil

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
        let filePath = Bundle.main.url(forResource: weatherFileName, withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let translationConfig = try! SPXSpeechTranslationConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        translationConfig.speechRecognitionLanguage = "en-US"
        translationConfig.addTargetLanguage("de");
        translationConfig.removeTargetLanguage("de");
        translationConfig.addTargetLanguage("de");
        translationConfig.addTargetLanguage("zh-Hans");

        self.reco = try! SPXTranslationRecognizer(speechTranslationConfiguration: translationConfig, audioConfiguration: audioConfig!)

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
            self.translationDictionary = evt.result.translations as! [String: String];
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
            return (1 == (evaluatedObject as! TranslationEndToEndTests).counts["sessionStoppedCount"])
        }

        sessionStoppedCountPred = NSPredicate(block: sessionStoppedCountPredBlock)
        disconnectedPred = NSPredicate(block: {evaluatedObject, _ in
            return (1 == (evaluatedObject as! TranslationEndToEndTests).counts["disconnectedCount"])
        })
        connectedPred = NSPredicate(block: {evaluatedObject, _ in
            return (1 == (evaluatedObject as! TranslationEndToEndTests).counts["connectedCount"])
        })

    }

    func testRecognizeOnce() {
        let translationResult = try? self.reco!.recognizeOnce()

        XCTAssertEqual(translationResult!.text, weatherTextEnglish)
        XCTAssertEqual(translationResult!.reason, SPXResultReason.translatedSpeech)
        XCTAssertGreaterThan(translationResult!.duration, 0)
        XCTAssertGreaterThan(translationResult!.offset, 0)
        XCTAssertGreaterThan(translationResult!.resultId.count, 0)

        let translationDictionary = translationResult!.translations;
        let germanTranslation = translationDictionary["de"] as! String;
        let chineseTranslation = translationDictionary["zh-Hans"] as! String;

        XCTAssertEqual(germanTranslation, weatherTextGerman);
        XCTAssertEqual(chineseTranslation, weatherTextChinese);
    }

    func testContinuous () {
        try! self.reco?.startContinuousRecognition()

        self.expectation(for: sessionStoppedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        XCTAssertEqual(self.result["finalText"], self.weatherTextEnglish);
        XCTAssertEqual(self.counts["finalResultCount"], 1);
        XCTAssertGreaterThan(self.counts["connectedCount"]!, 0);

        let germanTranslation = translationDictionary["de"];
        let chineseTranslation = translationDictionary["zh-Hans"];
        XCTAssertEqual(germanTranslation, weatherTextGerman);
        XCTAssertEqual(chineseTranslation, weatherTextChinese);

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

        let germanTranslation = translationDictionary["de"];
        let chineseTranslation = translationDictionary["zh-Hans"];
        XCTAssertEqual(germanTranslation, weatherTextGerman);
        XCTAssertEqual(chineseTranslation, weatherTextChinese);

        try! self.reco?.stopContinuousRecognition()

        self.connection!.close()

        self.expectation(for: disconnectedPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)

        XCTAssertEqual(self.counts["connectedCount"]!, self.counts["disconnectedCount"]!);
    }

    func testChangingTargetLanuages () {
        self.reco?.addTargetLanguage("fr")
        self.reco?.removeTargetLanguage("de")

        try! self.reco?.startContinuousRecognition()
        self.expectation(for: sessionStoppedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)
        try! self.reco?.stopContinuousRecognition()

        let targetLang = self.reco?.targetLanguages as? [String];
        XCTAssertEqual(targetLang, ["zh-Hans", "fr"]);
        let langKeys = translationDictionary.keys;
        XCTAssertEqual(Array(langKeys), ["zh-Hans", "fr"]);
        let chineseTranslation = translationDictionary["zh-Hans"];
        XCTAssertEqual(chineseTranslation, weatherTextChinese);
        let frenchTranslation = translationDictionary["fr"];
        XCTAssertTrue(frenchTranslation!.count > 0);
    }
}
