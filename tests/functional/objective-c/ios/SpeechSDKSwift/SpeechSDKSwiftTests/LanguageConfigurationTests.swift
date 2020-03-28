//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import XCTest

class SourceLanguageConfigTests: XCTestCase {
    func testEmptySourceLanguage() {
        XCTAssertThrowsError(try SPXSourceLanguageConfiguration("")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testEmptyEndpointId() {
        XCTAssertThrowsError(try SPXSourceLanguageConfiguration(language:"en-US", endpointId: "")) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testValidCase() {
        var srcLanguageConfig: SPXSourceLanguageConfiguration? = nil
        srcLanguageConfig = try! SPXSourceLanguageConfiguration("en-US")
        XCTAssertNotNil(srcLanguageConfig)
        srcLanguageConfig = try! SPXSourceLanguageConfiguration(language:"en-US", endpointId: "myEndpointId")
        XCTAssertNotNil(srcLanguageConfig)
    }

    func testInitSpeechReconginzer() {
        let speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        let serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
        let filePath = Bundle.main.url(forResource: "whatstheweatherlike", withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:speechKey!, region:serviceRegion!)
        
        var reco: SPXSpeechRecognizer? = nil
        var language: String! = ""
        reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, language: "de-DE", audioConfiguration: audioConfig!)
        language = reco?.properties?.getPropertyBy(SPXPropertyId.speechServiceConnectionRecognitionLanguage) 
        XCTAssertEqual("de-DE", language)

        var srcLanguageConfig: SPXSourceLanguageConfiguration? = nil
        srcLanguageConfig = try! SPXSourceLanguageConfiguration("zh-CN")        
        XCTAssertNotNil(srcLanguageConfig)

        reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, sourceLanguageConfiguration: srcLanguageConfig!, audioConfiguration: audioConfig!)
        XCTAssertNotNil(reco)
        language = reco?.properties?.getPropertyBy(SPXPropertyId.speechServiceConnectionRecognitionLanguage) 
        XCTAssertEqual("zh-CN", language)

        srcLanguageConfig = try! SPXSourceLanguageConfiguration(language:"fr-FR", endpointId: "myEndpointId")
        XCTAssertNotNil(srcLanguageConfig)

        reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, sourceLanguageConfiguration: srcLanguageConfig!, audioConfiguration: audioConfig!)
        XCTAssertNotNil(reco)
        language = reco?.properties?.getPropertyBy(SPXPropertyId.speechServiceConnectionRecognitionLanguage)        
        XCTAssertEqual("fr-FR", language)
        let endpointId = reco?.properties?.getPropertyBy(SPXPropertyId.speechServiceConnectionEndpointId) 
        XCTAssertEqual("myEndpointId", endpointId)
    }
}

class AutoDetectSourceLanguageConfigTests: XCTestCase {
    func testEmptySourceLanguageList() {
        XCTAssertThrowsError(try SPXAutoDetectSourceLanguageConfiguration([])) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testEmptySourceLanguage() {
        XCTAssertThrowsError(try SPXAutoDetectSourceLanguageConfiguration(["en-US", ""])) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testEmptySourceLanguageConfigurationList() {
        XCTAssertThrowsError(try SPXAutoDetectSourceLanguageConfiguration(sourceLanguageConfigurations:[])) { error in
            XCTAssertEqual(error.localizedDescription, "Error: Exception with an error code: 0x5 (SPXERR_INVALID_ARG)")
        }
    }

    func testValidCase() {
        var autoDetectSrcLanguageConfig: SPXAutoDetectSourceLanguageConfiguration? = nil
        autoDetectSrcLanguageConfig = try! SPXAutoDetectSourceLanguageConfiguration(["en-US", "zh-CN"])
        XCTAssertNotNil(autoDetectSrcLanguageConfig)

        let enSrcLanguageConfig = try! SPXSourceLanguageConfiguration(language: "en-US", endpointId: "myEndpointId")
        let zhSrcLanguageConfig = try! SPXSourceLanguageConfiguration("zh-CN")
        autoDetectSrcLanguageConfig = try! SPXAutoDetectSourceLanguageConfiguration(sourceLanguageConfigurations:[enSrcLanguageConfig, zhSrcLanguageConfig])
        XCTAssertNotNil(autoDetectSrcLanguageConfig)
    }

    func testInitSpeechReconginzer() {
        let speechKey = ProcessInfo.processInfo.environment["subscriptionKey"]
        let serviceRegion = ProcessInfo.processInfo.environment["serviceRegion"]
        let filePath = Bundle.main.url(forResource: "whatstheweatherlike", withExtension: "wav")!
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)
        let speechConfig = try! SPXSpeechConfiguration(subscription:speechKey!, region:serviceRegion!)

        var reco: SPXSpeechRecognizer? = nil
        var languages: String! = ""
        var autoDetectSrcLanguageConfig: SPXAutoDetectSourceLanguageConfiguration? = nil
        autoDetectSrcLanguageConfig = try! SPXAutoDetectSourceLanguageConfiguration(["en-US", "zh-CN"])
        XCTAssertNotNil(autoDetectSrcLanguageConfig)
        reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, autoDetectSourceLanguageConfiguration: autoDetectSrcLanguageConfig!, audioConfiguration: audioConfig!)
        languages = reco?.properties?.getPropertyBy(SPXPropertyId.speechServiceConnectionAutoDetectSourceLanguages)
        XCTAssertEqual("en-US,zh-CN", languages)
        XCTAssertEqual("", reco?.properties?.getPropertyByName("en-USSPEECH-ModelId"))
        XCTAssertEqual("", reco?.properties?.getPropertyByName("zh-CNSPEECH-ModelId"))

        let enSrcLanguageConfig = try! SPXSourceLanguageConfiguration(language: "fr-FR", endpointId: "myEndpointId")
        let zhSrcLanguageConfig = try! SPXSourceLanguageConfiguration("de-DE")
        autoDetectSrcLanguageConfig = try! SPXAutoDetectSourceLanguageConfiguration(sourceLanguageConfigurations:[enSrcLanguageConfig, zhSrcLanguageConfig])
        XCTAssertNotNil(autoDetectSrcLanguageConfig)
        reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, autoDetectSourceLanguageConfiguration: autoDetectSrcLanguageConfig!, audioConfiguration: audioConfig!)
        languages = reco?.properties?.getPropertyBy(SPXPropertyId.speechServiceConnectionAutoDetectSourceLanguages)
        XCTAssertEqual("fr-FR,de-DE", languages)
        XCTAssertEqual("", reco?.properties?.getPropertyByName("de-DESPEECH-ModelId"))
        XCTAssertEqual("myEndpointId", reco?.properties?.getPropertyByName("fr-FRSPEECH-ModelId"))
    }
}