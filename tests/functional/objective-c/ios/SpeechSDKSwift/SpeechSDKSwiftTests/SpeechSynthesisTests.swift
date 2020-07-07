//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest

class SpeechSynthesisEndToEndTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""

    var speechConfig: SPXSpeechConfiguration? = nil

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
        let endpoint = String(format:"wss://%@.tts.speech.microsoft.com/cognitiveservices/websocket/v1?TrafficType=Test", arguments:[self.serviceRegion])
        self.speechConfig = try! SPXSpeechConfiguration(endpoint:endpoint, subscription:self.speechKey)
    }
    
    func testSynthesisOutputToResult() {
        let synthesizer = try! SPXSpeechSynthesizer(speechConfiguration: self.speechConfig!, audioConfiguration: nil)
        synthesizer.addSynthesisStartedEventHandler() {_, evt in
            XCTAssertEqual(evt.result.reason, SPXResultReason.synthesizingAudioStarted)
        }
        synthesizer.addSynthesizingEventHandler() {_, evt in
            XCTAssertEqual(evt.result.reason, SPXResultReason.synthesizingAudio)
            XCTAssertGreaterThan((evt.result.audioData?.count)!, 0)
        }
        synthesizer.addSynthesisCompletedEventHandler() {_, evt in
            XCTAssertEqual(evt.result.reason, SPXResultReason.synthesizingAudioCompleted)
            XCTAssertGreaterThan((evt.result.audioData?.count)!, 0)
        }
        synthesizer.addSynthesisCanceledEventHandler() {_, ent in
            XCTAssertTrue(false)
        }
        let result1 = try! synthesizer.speakText("{{{text1}}}")
        XCTAssertNotEqual(result1.reason, SPXResultReason.canceled)
        XCTAssertGreaterThan((result1.audioData?.count)!, 0)
        let result2 = try! synthesizer.speakText("{{{text1}}}")
        XCTAssertNotEqual(result2.reason, SPXResultReason.canceled)
        XCTAssertGreaterThan((result2.audioData?.count)!, 0)
    }
    
    func testSynthesisOutputToAudioDataStream() {
        let synthesizer = try! SPXSpeechSynthesizer(speechConfiguration: self.speechConfig!, audioConfiguration: nil)
        let result1 = try! synthesizer.speakText("{{{text1}}}")
        XCTAssertNotEqual(result1.reason, SPXResultReason.canceled)
        let stream = try!SPXAudioDataStream(from: result1)
        let data = NSMutableData(capacity: 1024)
        var audioLength: Int = 0
        while (stream.read(data!, length: 1024) > 0) {
            audioLength += data!.length
        }
        XCTAssertGreaterThan(audioLength, 0)
    }
    
    func testSynthesisOutputToPullStream() {
        let stream: SPXPullAudioOutputStream? = SPXPullAudioOutputStream()
        let streamConfig: SPXAudioConfiguration? = try!SPXAudioConfiguration(streamOutput: stream!)
        var synthesizer: SPXSpeechSynthesizer? = try! SPXSpeechSynthesizer(speechConfiguration: self.speechConfig!, audioConfiguration: streamConfig!)
        try! synthesizer?.startSpeakingText("{{{text1}}}")
        synthesizer = nil
        let data = NSMutableData(capacity: 1024)
        var audioLength: Int = 0
        while ((stream?.read(data!, length: 1024))! > 0) {
            audioLength += data!.length
        }
        XCTAssertGreaterThan(audioLength, 0)
    }
    
    func testSynthesisOutputToPushStream() {
        var isClosed = false
        let stream = SPXPushAudioOutputStream(writeHandler:{data in
            XCTAssertGreaterThan(data.count, 0)
            return UInt(data.count)},
            closeHandler: {
                isClosed = true
        })
        let streamConfig: SPXAudioConfiguration? = try!SPXAudioConfiguration(streamOutput: stream!)
        var synthesizer: SPXSpeechSynthesizer? = try! SPXSpeechSynthesizer(speechConfiguration: self.speechConfig!, audioConfiguration: streamConfig!)
        try! synthesizer?.startSpeakingText("{{{text1}}}")
        synthesizer = nil
        XCTAssertTrue(isClosed)
    }

    func testSpeechRecognitionWithAutoLanguageDetection() {
        let autoDetectSrcLanguageConfig: SPXAutoDetectSourceLanguageConfiguration? = try! SPXAutoDetectSourceLanguageConfiguration(openRange: ())
        let synthesizer: SPXSpeechSynthesizer? = try! SPXSpeechSynthesizer(speechConfiguration: self.speechConfig!,
                                                                           autoDetectSourceLanguageConfiguration: autoDetectSrcLanguageConfig!,
                                                                           audioConfiguration: nil)

        var lastTextOffset: UInt = 0
        synthesizer?.addSynthesisWordBoundaryEventHandler({ (synt, evt) in
            lastTextOffset = evt.textOffset
        })

        let result = try! synthesizer?.speakText("你好世界。")
        XCTAssertNotEqual(result!.reason, SPXResultReason.canceled)
        XCTAssertGreaterThan((result!.audioData?.count)!, 32000)  // longer than 1s
        XCTAssertGreaterThanOrEqual(lastTextOffset, 2)
    }
}
