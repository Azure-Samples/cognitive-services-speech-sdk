//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest

class ConversationTranslationEndToEndTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""
    let timeoutInSeconds = 20.0
    let weatherFileName = "whatstheweatherlike"
    let weatherTextEnglish = "What's the weather like?"

    var result: [String: String] = ["finalText": "", "reason": ""]
    var counts: [String: Int] = ["finalResultCount": 0,
                                 "textMessageReceivedCount": 0,
                                 "sessionStartedCount": 0,
                                 "sessionStoppedCount": 0]

    var conversation: SPXConversation? = nil
    var translator: SPXConversationTranslator? = nil
    var audioConfig: SPXAudioConfiguration? = nil

    var finalResultCountPred: NSPredicate? = nil
    var sessionStoppedCountPred: NSPredicate? = nil
    var textMessageReceivedCountPred: NSPredicate? = nil

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
        let speechConfig = try! SPXSpeechTranslationConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        speechConfig.speechRecognitionLanguage = "en-US"
        speechConfig.addTargetLanguage("en-US")
        speechConfig.addTargetLanguage("de-DE")
        speechConfig.addTargetLanguage("ja-JP")

        let filePath = Bundle.main.url(forResource: weatherFileName, withExtension: "wav")!
        self.audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        self.conversation = try! SPXConversation(speechConfig)
        self.translator = try! SPXConversationTranslator(audioConfiguration: self.audioConfig!)
        
        self.translator!.addTranscribingEventHandler() {_, evt in }
        
        self.translator!.addTranscribedEventHandler() {[unowned self] _, evt in
            print("received result: \(evt.result?.text ?? "*no text*") with reason \(evt.result!.reason.rawValue)")
            if (evt.result!.reason == SPXResultReason.canceled || evt.result!.reason == SPXResultReason.noMatch) {
                return
            }

            self.result["finalText"]! = evt.result!.text!
            self.result["reason"]! = "\(evt.result!.reason.rawValue)"
            self.counts["finalResultCount"]! += 1
            print("counts in transcribed handler: \(self.counts)")
        }

        self.translator!.addSessionStartedEventHandler() {[unowned self] _, evt in
            self.counts["sessionStartedCount"]! += 1
            print("counts in sessionStarted handler: \(self.counts)")
        }

        self.translator!.addSessionStoppedEventHandler() {[unowned self] _, evt in
            self.counts["sessionStoppedCount"]! += 1
            print("counts in sessionStopped handler: \(self.counts)")
        }
        
        self.translator!.addTextMessageReceivedEventHandler() {[unowned self] _, evt in
            self.counts["textMessageReceivedCount"]! += 1
            print("counts in sessionStarted handler: \(self.counts)")
        }
        
        let finalResultCountPredBlock: (Any?, [String : Any]?) -> Bool = {evaluatedObject, _ in
            return (1 == (evaluatedObject as! ConversationTranslationEndToEndTests).counts["finalResultCount"])
        }

        finalResultCountPred = NSPredicate(block: finalResultCountPredBlock)

        let sessionStoppedCountPredBlock: (Any?, [String : Any]?) -> Bool = {evaluatedObject, _ in
            return (1 == (evaluatedObject as! ConversationTranslationEndToEndTests).counts["sessionStoppedCount"])
        }

        sessionStoppedCountPred = NSPredicate(block: sessionStoppedCountPredBlock)
        
        let textMessageReceivedCountBlock: (Any?, [String : Any]?) -> Bool = {evaluatedObject, _ in
            return (1 == (evaluatedObject as! ConversationTranslationEndToEndTests).counts["textMessageReceivedCount"])
        }

        textMessageReceivedCountPred = NSPredicate(block: textMessageReceivedCountBlock)
    }

    func testJoinConversationFailure() {
        var asyncResult = false
        let expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.joinConversationAsync( {(joined, err) in
            asyncResult = joined
            expectation.fulfill()
        }, conversationId: "_", nickname: "testJoinFailure", language: "en-US")
        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertFalse(asyncResult)
    }

    func testStartTranscribeFailure() {
        var asyncResult = false
        let expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.startTranscribingAsync({ (started, err) in
            asyncResult = started
            expectation.fulfill()
        })
        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertFalse(asyncResult)
    }

    func testStopTranscribeFailure() {
        var asyncResult = false
        let expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.stopTranscribingAsync({ (stopped, err) in
            asyncResult = stopped
            expectation.fulfill()
        })
        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertFalse(asyncResult)
    }
    
    func testCreateSendTextTranscribe() {
        var asyncResult = false
        var expectation = XCTestExpectation(description: "Async request")
        try! self.conversation?.startAsync({ (started, err) in
            asyncResult = started
            expectation.fulfill()
        })
        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertTrue(asyncResult)
        
        asyncResult = false;
        expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.joinConversationAsync({ (joined, err) in
            asyncResult = joined
            expectation.fulfill()
        }, conversation: self.conversation!, nickname: "IamHost")

        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertTrue(asyncResult)

        asyncResult = false;
        expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.sendTextMessageAsync({ (sent, err) in
            asyncResult = sent
            expectation.fulfill()
        }, message: "Can you see this text message?")

        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertTrue(asyncResult)

        self.expectation(for: textMessageReceivedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)
        
        asyncResult = false;
        expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.startTranscribingAsync({ (started, err) in
            asyncResult = started
            expectation.fulfill()
        })
        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertTrue(asyncResult)

        self.expectation(for: finalResultCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)
        
        asyncResult = false;
        expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.stopTranscribingAsync({ (stopped, err) in
            asyncResult = stopped
            expectation.fulfill()
        })
        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertTrue(asyncResult)

        asyncResult = false;
        expectation = XCTestExpectation(description: "Async request")
        try! self.translator?.leaveConversationAsync({ (left, err) in
            asyncResult = left
            expectation.fulfill()
        })
        wait(for: [expectation], timeout: timeoutInSeconds)
        XCTAssertTrue(asyncResult)

        self.expectation(for: sessionStoppedCountPred!, evaluatedWith: self, handler: nil)
        self.waitForExpectations(timeout: timeoutInSeconds, handler: nil)
        
        XCTAssertEqual(self.result["finalText"], self.weatherTextEnglish);
    }
}
