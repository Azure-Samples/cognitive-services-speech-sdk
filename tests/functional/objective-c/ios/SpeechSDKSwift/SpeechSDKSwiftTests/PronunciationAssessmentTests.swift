//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest

class PronunciationAssessmentTests: XCTestCase {
    var speechKey: String! = ""
    var serviceRegion: String! = ""
    let weatherFileName = "whatstheweatherlike"

    override func setUp() {
        super.setUp()
        speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
    }

    func testPronunciationAssessmentConfig() {
        var pronConfig: SPXPronunciationAssessmentConfiguration? = try! SPXPronunciationAssessmentConfiguration("reference", gradingSystem: SPXPronunciationAssessmentGradingSystem.fivePoint, granularity: SPXPronunciationAssessmentGranularity.phoneme)

        XCTAssertNotNil(pronConfig)
        XCTAssertNotNil(pronConfig?.toJson()!.range(of: "FivePoint"))
        XCTAssertNotNil(pronConfig?.toJson()!.range(of: "Phoneme"))
        XCTAssertNotNil(pronConfig?.toJson()!.range(of: "Comprehensive"))
        XCTAssertNotNil(pronConfig?.toJson()!.range(of: "reference"))

        pronConfig = try! SPXPronunciationAssessmentConfiguration("reference",
                                                                  gradingSystem:SPXPronunciationAssessmentGradingSystem.hundredMark,
                                                                  granularity:SPXPronunciationAssessmentGranularity.word,
                                                                  enableMiscue:true)

        XCTAssertNotNil(pronConfig)
        pronConfig?.referenceText = "new reference"
        XCTAssertNotNil(pronConfig?.toJson()!.range(of: "HundredMark"))
        XCTAssertNotNil(pronConfig?.toJson()!.range(of: "Word"))
        XCTAssertNotNil(pronConfig?.toJson()!.range(of: "new reference"))

        let pronConfig2 = try! SPXPronunciationAssessmentConfiguration(json: (pronConfig?.toJson())!)

        XCTAssertNotNil(pronConfig2)
        XCTAssertEqual(pronConfig?.toJson()!, pronConfig2.toJson()!)
    }

    func testPronunciationAssessment() {
        let filePath = Bundle.main.url(forResource: weatherFileName, withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:self.speechKey, region:self.serviceRegion)
        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!)

        let pronConfig = try! SPXPronunciationAssessmentConfiguration("")
        try! pronConfig.apply(to: reco)

        let result = try? reco.recognizeOnce()

        let pronResult = SPXPronunciationAssessmentResult(result!)

        XCTAssertGreaterThan(pronResult!.accuracyScore, 0)
        XCTAssertGreaterThan(pronResult!.pronunciationScore, 0)
        XCTAssertGreaterThan(pronResult!.completenessScore, 0)
        XCTAssertGreaterThan(pronResult!.fluencyScore, 0)
    }
}
