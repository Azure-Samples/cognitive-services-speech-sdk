//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import UIKit
import MicrosoftCognitiveServicesSpeech
import Differ

extension StringProtocol {
    var words: [SubSequence] {
        split(whereSeparator: \.isWhitespace)
    }
}

struct Word {
    var word: String
    var errorType: String
    var accuracyScore = 0.0
}

class ViewController: UIViewController {
    var label: UILabel!
    var continuousPronunciationAssessmentButton: UIButton!
    var pronunciationAssessmentWithStreamButton: UIButton!
    var pronunciationAssessmentWithMicrophoneButton: UIButton!
    var pronunciationAssessmentWithContentAssessmentButton: UIButton!

    var sub: String!
    var region: String!

    override func viewDidLoad() {
        super.viewDidLoad()

        // load subscription information
        sub = "YourSubscriptionKey"
        region = "YourServiceRegion"
        
        continuousPronunciationAssessmentButton = UIButton(frame: CGRect(x: 30, y: 100, width: 300, height: 50))
        continuousPronunciationAssessmentButton.setTitle("Continuous Pron-Assessment", for: .normal)
        continuousPronunciationAssessmentButton.addTarget(self, action:#selector(self.continuousPronunciationAssessmentButtonClicked), for: .touchUpInside)
        continuousPronunciationAssessmentButton.setTitleColor(UIColor.black, for: .normal)

        pronunciationAssessmentWithStreamButton = UIButton(frame: CGRect(x: 30, y: 140, width: 300, height: 50))
        pronunciationAssessmentWithStreamButton.setTitle("Pron-Assessment With Stream", for: .normal)
        pronunciationAssessmentWithStreamButton.addTarget(self, action: #selector(self.pronunciationAssessmentWithStreamButtonClicked), for: .touchUpInside)
        pronunciationAssessmentWithStreamButton.setTitleColor(UIColor.black, for: .normal)
        
        pronunciationAssessmentWithMicrophoneButton = UIButton(frame: CGRect(x: 30, y: 180, width: 300, height: 50))
        pronunciationAssessmentWithMicrophoneButton.setTitle("Pron-Assessment With Microphone", for: .normal)
        pronunciationAssessmentWithMicrophoneButton.addTarget(self, action: #selector(self.pronunciationAssessmentWithMicrophoneButtonClicked), for: .touchUpInside)
        pronunciationAssessmentWithMicrophoneButton.setTitleColor(UIColor.black, for: .normal)
        
        pronunciationAssessmentWithContentAssessmentButton = UIButton(frame: CGRect(x: 30, y: 220, width: 300, height: 50))
        pronunciationAssessmentWithContentAssessmentButton.setTitle("Pron-Assessment With Content", for: .normal)
        pronunciationAssessmentWithContentAssessmentButton.addTarget(self, action: #selector(self.pronunciationAssessmentWithContentAssessmentButtonClicked), for: .touchUpInside)
        pronunciationAssessmentWithContentAssessmentButton.setTitleColor(UIColor.black, for: .normal)


        label = UILabel(frame: CGRect(x: 30, y: 240, width: 300, height: 400))
        label.textColor = UIColor.black
        label.lineBreakMode = .byWordWrapping
        label.numberOfLines = 0
        label.text = "Recognition Result"

        self.view.addSubview(label)
        self.view.addSubview(continuousPronunciationAssessmentButton)
        self.view.addSubview(pronunciationAssessmentWithStreamButton)
        self.view.addSubview(pronunciationAssessmentWithMicrophoneButton)
        self.view.addSubview(pronunciationAssessmentWithContentAssessmentButton)
    }

    @objc func continuousPronunciationAssessmentButtonClicked() {
        DispatchQueue.global(qos: .userInitiated).async {
            self.continuousPronunciationAssessment()
        }
    }

    @objc func pronunciationAssessmentWithStreamButtonClicked(){
        //self.updateLabel(text: "filling in the stream result", color: UIColor.black)
        DispatchQueue.global(qos: .userInitiated).async {
            self.pronunciationAssessmentWithStream()
        }
    }
    
    @objc func pronunciationAssessmentWithMicrophoneButtonClicked(){
        DispatchQueue.global(qos: .userInitiated).async {
            self.pronunciationAssessmentWithMicrophone()
        }
    }
    
    @objc func pronunciationAssessmentWithContentAssessmentButtonClicked(){
        DispatchQueue.global(qos: .userInitiated).async {
            self.pronunciationAssessmentWithContentAssessment()
        }
    }

    /**
     * Continuous Pronunciation Assessment Sample.
     *
     * This example demonstrates how to perform continuous pronunciation assessment from a wav file.
     */
    func continuousPronunciationAssessment() {
        var speechConfig: SPXSpeechConfiguration?
        do {
            try speechConfig = SPXSpeechConfiguration(subscription: sub, region: region)
        } catch {
            print("error \(error) happened")
            speechConfig = nil
        }
        speechConfig?.speechRecognitionLanguage = "en-US"

        let bundle = Bundle.main
        let path = bundle.path(forResource: "pronunciation_assessment", ofType: "wav")
        if (path == nil) {
            print("Cannot find audio file!");
            self.updateLabel(text: "Cannot find audio file", color: UIColor.red)
            return;
        }
        print("pronunciation assessment audio file path: ", path!)

        let audioConfig = SPXAudioConfiguration.init(wavFileInput: path!)

        let referenceText = "Hello world hello. Today is a nice day."

        var pronConfig: SPXPronunciationAssessmentConfiguration?
        do {
            try pronConfig = SPXPronunciationAssessmentConfiguration.init(referenceText, gradingSystem: SPXPronunciationAssessmentGradingSystem.hundredMark, granularity: SPXPronunciationAssessmentGranularity.word, enableMiscue: true)            
        } catch {
            print("error \(error) happened")
            pronConfig = nil
            return
        }
        
        pronConfig?.enableProsodyAssessment()

        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig!, audioConfiguration: audioConfig!)
        try! pronConfig!.apply(to: reco)

        reco.addRecognizingEventHandler() {reco, evt in
            print("intermediate recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text, color: .gray)
        }

        var sumAccuracy: Double = 0;
        var sumDuration: TimeInterval = 0, validDuration: TimeInterval = 0;
        var startOffset: TimeInterval = 0, endOffset: TimeInterval = 0;
        var pronWords: [SPXWordLevelTimingResult] = []
        var recognizedWords: [String] = []
        var prosodyScores: [Double] = []
        reco.addRecognizedEventHandler() {reco, evt in
            print("Received final result event. SessionId: \(evt.sessionId), recognition result: \(evt.result.text!). Status \(evt.result.reason). offset \(evt.result.offset) duration \(evt.result.duration) resultid: \(evt.result.resultId)");
            let pronunciationResult = SPXPronunciationAssessmentResult.init(evt.result)!;
            let resultText = "Received final result event. \nrecognition result: \(evt.result.reason), Accuracy score: \(pronunciationResult.accuracyScore), Prosody score: \(pronunciationResult.prosodyScore), Pronunciation score: \(pronunciationResult.pronunciationScore), Completeness Score: \(pronunciationResult.completenessScore), Fluency score: \(pronunciationResult.fluencyScore)"
            self.updateLabel(text: resultText, color: UIColor.black)
            
            prosodyScores.append(pronunciationResult.prosodyScore)

            for word in pronunciationResult.words! {
                pronWords.append(word)
                recognizedWords.append(word.word!)
                sumAccuracy += word.accuracyScore * Double(word.duration)
                sumDuration += word.duration;
                if (word.errorType == "None") {
                    validDuration += word.duration + 0.01
                }
                endOffset = word.offset + word.duration + 0.01
            }
        }

        var end = false
        reco.addSessionStoppedEventHandler() {reco, evt in
            end = true
        }

        updateLabel(text: "Assessing ...", color: .gray)
        print("Assessing...")

        try! reco.startContinuousRecognition()
        while !end {
            Thread.sleep(forTimeInterval: 1.0)
        }
        try! reco.stopContinuousRecognition()

        var referenceWords: [String] = []
        for w in referenceText.words {
            referenceWords.append(String(w).lowercased().trimmingCharacters(in: .punctuationCharacters))
        }
        let diff = referenceWords.outputDiffPathTraces(to: recognizedWords)
        var finalWords: [Word] = []
        var validWordCount = 0
        for e in diff {
            if e.from.x + 1 == e.to.x && e.from.y + 1 == e.to.y {
                let v = pronWords[e.from.y]
                finalWords.append(Word(word: v.word!,errorType: v.errorType!,accuracyScore: v.accuracyScore))
                validWordCount += 1
            } else if e.from.y < e.to.y {
                let v = pronWords[e.from.y]
                finalWords.append(Word(word: v.word!,errorType: "Insertion"))
            } else {
                finalWords.append(Word(word:referenceWords[e.from.x],errorType: "Omission"))
            }
        }

        if (recognizedWords.count > 0) {
            // Overall accuracy and fluency scores are the weighted average of scores of all sentences.
            startOffset = pronWords[0].offset
            let fluencyScore: Double = Double(validDuration) / Double(endOffset - startOffset) * 100.0
            let completenessScore: Double = Double(validWordCount) / Double(referenceWords.count)
            
            let prosodyScore: Double = Double(prosodyScores.reduce(0, +)) / Double(prosodyScores.count)
            
            var resultText = "Assessment finished. \nOverall accuracy score: \(sumAccuracy / Double(sumDuration)), prosody score: \(prosodyScore), fluency score: \(fluencyScore), completeness score: \(completenessScore)"

            for w in finalWords {
                resultText += "\n"
                resultText += " word: \(w.word)\taccuracy score: \(w.accuracyScore)\terror type: \(w.errorType);"
            }
            self.updateLabel(text: resultText, color: UIColor.black)
        }
    }

    func pronunciationAssessmentWithStream() {
        // Replace with your own subscription key and service region (e.g., "westus").
        // Creates an instance of a speech config with specified subscription key and service region.
        let speechConfig = try! SPXSpeechConfiguration(subscription: sub, region: region)
        
        // Read audio data from file. In real scenario this can be from memory or network
        //let audioDataWithHeader = try! Data(contentsOf: URL(fileURLWithPath: "whatstheweatherlike.wav"))
        
        let bundle = Bundle.main
        let path = bundle.path(forResource: "whatstheweatherlike", ofType: "wav")
        if (path == nil) {
            print("Cannot find audio file!");
            self.updateLabel(text: "Cannot find audio file", color: UIColor.red)
            return;
        }
        print("pronunciation assessment audio file path: ", path!)
        
        let audioDataWithHeader = try! Data(contentsOf: URL(fileURLWithPath: path!))
        let audioData = Array(audioDataWithHeader[46..<audioDataWithHeader.count])
        
        let startTime = Date()
        
        let audioFormat = SPXAudioStreamFormat(usingPCMWithSampleRate: 16000, bitsPerSample: 16, channels: 1)!
        guard let audioInputStream = SPXPushAudioInputStream(audioFormat: audioFormat) else {
            print("Error: Failed to create audio input stream.")
            return
        }
        
        guard let audioConfig = SPXAudioConfiguration(streamInput: audioInputStream) else {
            print("Error: audioConfig is Nil")
            return
        }
        
        let speechRecognizer = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, language: "en-US", audioConfiguration: audioConfig)
        
        let referenceText = "what's the weather like"
        let pronAssessmentConfig = try! SPXPronunciationAssessmentConfiguration(referenceText, gradingSystem: SPXPronunciationAssessmentGradingSystem.hundredMark, granularity: SPXPronunciationAssessmentGranularity.word, enableMiscue: true)
        
        pronAssessmentConfig.enableProsodyAssessment()
        
        try! pronAssessmentConfig.apply(to: speechRecognizer)
        
        audioInputStream.write(Data(audioData))
        audioInputStream.write(Data())
        
        self.updateLabel(text: "Analysising", color: UIColor.black)
        // Handle the recognition result
        try! speechRecognizer.recognizeOnceAsync { result in
            guard let pronunciationResult = SPXPronunciationAssessmentResult(result) else {
                print("Error: pronunciationResult is Nil")
                return
            }
            self.updateLabel(text: "generating result...", color: UIColor.black)
            var finalResult = ""
            let resultText = "Accuracy score: \(pronunciationResult.accuracyScore), Prosody score: \(pronunciationResult.prosodyScore), Pronunciation score: \(pronunciationResult.pronunciationScore), Completeness Score: \(pronunciationResult.completenessScore), Fluency score: \(pronunciationResult.fluencyScore)"
            print(resultText)
            finalResult.append("\(resultText)\n")
            finalResult.append("\nword    accuracyScore   errorType\n")
            
            if let words = pronunciationResult.words {
                for word in words {
                    let wordString = word.word ?? ""
                    let errorType = word.errorType ?? ""
                    finalResult.append("\(wordString)    \(word.accuracyScore)   \(errorType)\n")
                }
            }
            
            self.updateLabel(text: finalResult, color: UIColor.black)
            
            let endTime = Date()
            let timeCost = endTime.timeIntervalSince(startTime) * 1000
            print("Time cost: \(timeCost)ms")
        }
    }
    
    func pronunciationAssessmentConfiguredWithJson() {
        // Creates an instance of a speech config with specified subscription key and service region.
        // Replace with your own subscription key and service region (e.g., "westus").
        let speechConfig = try! SPXSpeechConfiguration(subscription: sub, region: region)
        
        let bundle = Bundle.main
        let path = bundle.path(forResource: "whatstheweatherlike", ofType: "wav")
        if (path == nil) {
            print("Cannot find audio file!");
            self.updateLabel(text: "Cannot find audio file", color: UIColor.red)
            return;
        }
        print("pronunciation assessment audio file path: ", path!)
        
        // Replace the language with your language in BCP-47 format, e.g., en-US.
        let language = "en-US"
        
        // Creates a speech recognizer using wav file.
        guard let audioInput = SPXAudioConfiguration(wavFileInput: path!) else {
            print("Error: audioInput is Nil")
            return
        }
        let referenceText = "what's the weather like"
        
        // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
        let jsonConfig = "{\"GradingSystem\":\"HundredMark\",\"Granularity\":\"Phoneme\",\"EnableMiscue\":true,\"ScenarioId\":\"[scenario ID will be assigned by product team]\"}"
        let pronAssessmentConfig = try! SPXPronunciationAssessmentConfiguration(json: jsonConfig)
        pronAssessmentConfig.referenceText = referenceText
        
        pronAssessmentConfig.enableProsodyAssessment()
        
        // Creates a speech recognizer for the specified language
        let recognizer = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, language: language, audioConfiguration: audioInput)
        
        try! pronAssessmentConfig.apply(to: recognizer)
        
        self.updateLabel(text: "Analysising", color: UIColor.black)
        // Handle the recognition result
        try! recognizer.recognizeOnceAsync { result in
            guard let pronunciationResult = SPXPronunciationAssessmentResult(result) else {
                print("Error: pronunciationResult is Nil")
                return
            }
            self.updateLabel(text: "generating result...", color: UIColor.black)
            var finalResult = ""
            let resultText = "Accuracy score: \(pronunciationResult.accuracyScore), Prosody score: \(pronunciationResult.prosodyScore), Pronunciation score: \(pronunciationResult.pronunciationScore), Completeness Score: \(pronunciationResult.completenessScore), Fluency score: \(pronunciationResult.fluencyScore)"
            print(resultText)
            finalResult.append("\(resultText)\n")
            finalResult.append("\nword    accuracyScore   errorType\n")
            
            if let words = pronunciationResult.words {
                for word in words {
                    let wordString = word.word ?? ""
                    let errorType = word.errorType ?? ""
                    finalResult.append("\(wordString)    \(word.accuracyScore)   \(errorType)\n")
                }
            }
            
            self.updateLabel(text: finalResult, color: UIColor.black)
        }
    }
    
    func pronunciationAssessmentWithMicrophone() {
        // Creates an instance of a speech config with specified subscription key and service region.
        // Replace with your own subscription key and service region (e.g., "westus").
        let speechConfig = try! SPXSpeechConfiguration(subscription: sub, region: region)
        
        // Starts speech recognition, and returns after a single utterance is recognized.
        // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead
        speechConfig.setPropertyTo("3000", by: SPXPropertyId.speechServiceConnectionEndSilenceTimeoutMs)
        
        let language = "en-US"
        
        // Replace with your reference text
        let referenceText = "what's the weather like"

        // Create a pronunciation assessment config
        let pronunciationConfig = try! SPXPronunciationAssessmentConfiguration(
            referenceText,
            gradingSystem: .hundredMark,
            granularity: .phoneme,
            enableMiscue: true
        )
        
        pronunciationConfig.enableProsodyAssessment();
        
        // Create a speech recognizer
        let recognizer = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, language: language)
        
        // Apply the pronunciation assessment config object
        try! pronunciationConfig.apply(to: recognizer)
        
        self.updateLabel(text: "Speaking...", color: UIColor.black)
        // Handle the recognition result
        try! recognizer.recognizeOnceAsync { result in
            guard let pronunciationResult = SPXPronunciationAssessmentResult(result) else {
                print("Error: pronunciationResult is Nil")
                return
            }
            self.updateLabel(text: "generating result...", color: UIColor.black)
            var finalResult = ""
            let resultText = "Accuracy score: \(pronunciationResult.accuracyScore), Prosody score: \(pronunciationResult.prosodyScore), Pronunciation score: \(pronunciationResult.pronunciationScore), Completeness Score: \(pronunciationResult.completenessScore), Fluency score: \(pronunciationResult.fluencyScore)"
            print(resultText)
            finalResult.append("\(resultText)\n")
            finalResult.append("\nword    accuracyScore   errorType\n")
            
            if let words = pronunciationResult.words {
                for word in words {
                    let wordString = word.word ?? ""
                    let errorType = word.errorType ?? ""
                    finalResult.append("\(wordString)    \(word.accuracyScore)   \(errorType)\n")
                }
            }
            self.updateLabel(text: finalResult, color: UIColor.black)
        }
    }
    
    func pronunciationAssessmentWithContentAssessment() {
        // Creates an instance of a speech config with specified subscription key and service region.
        // Replace with your own subscription key and service region (e.g., "westus").
        let speechConfig = try! SPXSpeechConfiguration(subscription: sub, region: region)
        
        let bundle = Bundle.main
        let path = bundle.path(forResource: "pronunciation_assessment_fall", ofType: "wav")
        if (path == nil) {
            print("Cannot find audio file!");
            self.updateLabel(text: "Cannot find audio file", color: UIColor.red)
            return;
        }
        print("pronunciation assessment audio file path: ", path!)
        
        // Replace the language with your language in BCP-47 format, e.g., en-US.
        let language = "en-US"
        
        // Creates a speech recognizer using wav file.
        guard let audioInput = SPXAudioConfiguration(wavFileInput: path!) else {
            print("Error: audioInput is Nil")
            return
        }
        
        let theTopic = "the season of the fall"
        
        // Create a pronunciation assessment config
        let pronAssessmentConfig = try! SPXPronunciationAssessmentConfiguration(
            "",
            gradingSystem: .hundredMark,
            granularity: .phoneme,
            enableMiscue: false
        )
        
        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig, language: "en-US", audioConfiguration: audioInput)
        
        pronAssessmentConfig.enableProsodyAssessment()
        pronAssessmentConfig.enableContentAssessment(withTopic: theTopic)
        
        try! pronAssessmentConfig.apply(to: reco)
        
        var recognizedTexts: [String] = []
        var pronContents: [SPXContentAssessmentResult] = []
        var end = false
        
        reco.addRecognizedEventHandler() {reco, evt in
            let text: String = evt.result.text ?? ""
            if (!text.isEmpty && text != ".") {
                recognizedTexts.append(text)
            }
            
            if let pronResult = SPXPronunciationAssessmentResult(evt.result) {
                if let contentAssessmentResult = pronResult.contentAssessmentResult {
                    pronContents.append(contentAssessmentResult)
                }
            }
        }
        
        reco.addSessionStoppedEventHandler() {reco, evt in
            end = true
        }

        updateLabel(text: "Assessing ...", color: .gray)
        print("Assessing...")

        try! reco.startContinuousRecognition()
            
        while !end {
            Thread.sleep(forTimeInterval: 1.0)
        }
            
        try! reco.stopContinuousRecognition()
        
        var finalResult = ""
        let recognizedText = recognizedTexts.joined(separator: " ")
        print(recognizedText)
        finalResult.append("\(recognizedText)\n")
        
        var contentResult = pronContents[pronContents.count - 1]
        let resultText = "Assessment Result: Grammar score: \(contentResult.grammarScore), vocabulary  score: \(contentResult.vocabularyScore), topic score: \(contentResult.topicScore)"
        print(resultText)
        
        finalResult.append(resultText)
        
        updateLabel(text: finalResult, color: UIColor.black)
    }

    func updateLabel(text: String?, color: UIColor) {
        DispatchQueue.main.async {
            self.label.text = text
            self.label.textColor = color
        }
    }
}

