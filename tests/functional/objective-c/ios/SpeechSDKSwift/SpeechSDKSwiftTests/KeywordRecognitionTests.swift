//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest

class KeywordRecognitionEndToEndTests: XCTestCase {
    let timeoutInSeconds = 20.0
    let kwsWeatherFileName = "kws_whatstheweatherlike"
    let kwsTableFileName = "kws"
    let kwsWeatherTextEnglish = "Computer, What's the weather like?"

    var result: [String: String] = ["finalText": "", "reason": ""]
    var counts: [String: Int] = ["canceledCount": 0,
                                 "keywordRecognizedCount": 0]

    var reco: SPXKeywordRecognizer? = nil
    var keywordModel: SPXKeywordRecognitionModel? = nil

    var keywordRecognizedCountPred: NSPredicate? = nil

    override func setUp() {
        super.setUp()
        let filePath = Bundle.main.url(forResource: kwsWeatherFileName, withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let filePath2 = Bundle.main.url(forResource: kwsTableFileName, withExtension: "table")!
        self.keywordModel = try! SPXKeywordRecognitionModel(fromFile: filePath2.path)
        self.reco = try! SPXKeywordRecognizer(audioConfig!)

        self.reco!.addRecognizedEventHandler() {[unowned self] _, evt in
            print("received result: \(evt.result?.text ?? "*no text*") with reason \(evt.result!.reason.rawValue)")
            if (evt.result!.reason == SPXResultReason.canceled || evt.result!.reason == SPXResultReason.noMatch) {
                return
            }
            if (evt.result!.reason == SPXResultReason.recognizedKeyword) {
                self.result["finalText"]! = evt.result!.text!
                self.result["reason"]! = "\(evt.result!.reason.rawValue)"
                self.counts["keywordRecognizedCount"]! += 1
            }
            print("counts in recog handler: \(self.counts)")
        }

        self.reco!.addCanceledEventHandler() {[unowned self] _, evt in
            self.counts["canceledCount"]! += 1
            print("counts in canceled handler: \(self.counts)")
        }

        let keywordRecognizedCountPredBlock: (Any?, [String : Any]?) -> Bool = {evaluatedObject, _ in
            return (1 == (evaluatedObject as! KeywordRecognitionEndToEndTests).counts["keywordRecognizedCount"])
        }

        keywordRecognizedCountPred = NSPredicate(block: keywordRecognizedCountPredBlock)
    }

    func testKeywordFound() {
        let expectation = XCTestExpectation(description: "Async request")
        var asyncResult:SPXKeywordRecognitionResult!
        try! self.reco?.recognizeOnceAsync( {(keywordRecognitionResult) in
            asyncResult = keywordRecognitionResult
            expectation.fulfill()
        }, keywordModel: self.keywordModel!)

        wait(for: [expectation], timeout: timeoutInSeconds)        

        XCTAssertNotNil(asyncResult!.text?.range(of: "Computer"))
        XCTAssertEqual(asyncResult!.reason, SPXResultReason.recognizedKeyword)
    }
}
