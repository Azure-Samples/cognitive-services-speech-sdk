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
    var duration = 0.0
}

class ViewController: UIViewController {
    var label: UITextView!
    var continuousPronunciationAssessmentButton: UIButton!
    var pronunciationAssessmentWithStreamButton: UIButton!
    var pronunciationAssessmentWithMicrophoneButton: UIButton!

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

        label = UITextView(frame: CGRect(x: 30, y: 280, width: 300, height: 400))
        label.textColor = UIColor.black
        label.isEditable = false
        label.text = "Recognition Result"

        self.view.addSubview(label)
        self.view.addSubview(continuousPronunciationAssessmentButton)
        self.view.addSubview(pronunciationAssessmentWithStreamButton)
        self.view.addSubview(pronunciationAssessmentWithMicrophoneButton)
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

    func getReferenceWords(waveFilename: String, referenceText: String, language: String, speechConfig: SPXSpeechConfiguration) -> [String]? {
        // Configure audio input
        let audioConfig = SPXAudioConfiguration(wavFileInput: waveFilename)
        
        // Check if audioConfig is nil
        if audioConfig == nil {
            print("Error creating audio configuration.")
            return nil
        }

        // Set speech recognition language
        speechConfig.speechRecognitionLanguage = language

        // Create speech recognizer
        guard let speechRecognizer = try? SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig!) else {
            print("Error creating speech recognizer.")
            return nil
        }

        // Configure pronunciation assessment, set grading system, granularity, and enable miscue
        let enableMiscue = true
        guard let pronunciationConfig = try? SPXPronunciationAssessmentConfiguration(referenceText, gradingSystem: .hundredMark, granularity: .phoneme, enableMiscue: enableMiscue) else {
            print("Error creating pronunciation assessment configuration.")
            return nil
        }

        // Apply pronunciation assessment config to speech recognizer
        try! pronunciationConfig.apply(to: speechRecognizer)

        // Perform speech recognition
        var referenceWords = [String]()
        let group = DispatchGroup()
        group.enter()

        try! speechRecognizer.recognizeOnceAsync { result in
            defer { group.leave() }

            if result.reason == SPXResultReason.recognizedSpeech {
                if let responseJson = result.properties?.getPropertyBy(SPXPropertyId.speechServiceResponseJsonResult) {
                    // Parse the JSON result to extract NBest and Words
                    if let data = responseJson.data(using: .utf8),
                       let json = try? JSONSerialization.jsonObject(with: data, options: []),
                       let jsonDict = json as? [String: Any],
                       let nBestArray = jsonDict["NBest"] as? [[String: Any]],
                       let wordsArray = nBestArray.first?["Words"] as? [[String: Any]] {
                        for wordInfo in wordsArray {
                            if let word = wordInfo["Word"] as? String,
                               let errorType = (wordInfo["PronunciationAssessment"] as? [String: Any])?["ErrorType"] as? String {
                                if errorType != "Insertion" {
                                    referenceWords.append(word)
                                }
                            }
                        }
                    }
                }
            } else if result.reason == SPXResultReason.noMatch {
                print("No speech could be recognized.")
            } else if result.reason == SPXResultReason.canceled {
                let cancellationDetails = try! SPXCancellationDetails(fromCanceledRecognitionResult: result)
                print("Speech recognition canceled: \(cancellationDetails.reason)")
                if cancellationDetails.reason == SPXCancellationReason.error {
                    print("Error details: \(String(describing: cancellationDetails.errorDetails))")
                }
            }
        }

        group.wait()
        return convertReferenceWords(referenceText: referenceText, referenceWords: referenceWords)
    }

    // Converts the reference text into segmented words using the provided reference words list.
    func convertReferenceWords(referenceText: String, referenceWords: [String]) -> [String] {
        let dictionary = Set(referenceWords)
        let maxLength = dictionary.map { $0.count }.max() ?? 0
        
        let cleanedText = removePunctuation(text: referenceText)
        return segmentWords(referenceText: cleanedText, dictionary: dictionary, maxLength: maxLength)
    }

    // Removes punctuation from the text, leaving only letters, digits, and whitespaces.
    func removePunctuation(text: String) -> String {
        var result = ""
        for char in text {
            if char.isLetter || char.isNumber || char.isWhitespace {
                result.append(char)
            }
        }
        return result
    }

    // Segments the reference text into words using the provided dictionary. It attempts both left-to-right and right-to-left segmentation and returns the best result.
    func segmentWords(referenceText: String, dictionary: Set<String>, maxLength: Int) -> [String] {
        let leftToRight = leftToRightSegmentation(referenceText: referenceText, dictionary: dictionary, maxLength: maxLength)
        let rightToLeft = rightToLeftSegmentation(referenceText: referenceText, dictionary: dictionary, maxLength: maxLength)
        
        // If the left-to-right segmentation matches the original text, return that result
        if leftToRight.joined() == referenceText {
            return leftToRight
        }
        
        // If the right-to-left segmentation matches the original text, return that result
        if rightToLeft.joined() == referenceText {
            return rightToLeft
        }
        
        print("WW failed to segment the text with the dictionary")
        
        // If neither segmentation matches, return the one with fewer words
        if leftToRight.count < rightToLeft.count {
            return leftToRight
        } else if leftToRight.count > rightToLeft.count {
            return rightToLeft
        }
        
        // If both segmentations are equal, return the one with fewer single-character words
        let leftToRightSingle = leftToRight.filter { $0.count == 1 }.count
        let rightToLeftSingle = rightToLeft.filter { $0.count == 1 }.count
        
        return leftToRightSingle < rightToLeftSingle ? leftToRight : rightToLeft
    }

    // Segments the reference text from left to right, using the longest possible matches from the dictionary.
    func leftToRightSegmentation(referenceText: String, dictionary: Set<String>, maxLength: Int) -> [String] {
        var result = [String]()
        var text = referenceText
        
        while !text.isEmpty {
            // If the length of the text is less than the maxLength, use the whole text as the substring
            let subText = text.count < maxLength ? text : String(text.prefix(maxLength))
            
            var currentSubText = subText
            while !currentSubText.isEmpty {
                // If the substring is in the dictionary or is a single character, add it to the result
                if dictionary.contains(currentSubText) || currentSubText.count == 1 {
                    result.append(currentSubText)
                    // Remove the substring from the text
                    text = String(text.dropFirst(currentSubText.count))
                    break
                }
                // If the substring is not in the dictionary, shorten it by removing the last character
                currentSubText = String(currentSubText.dropLast())
            }
        }
        
        return result
    }

    // Segments the reference text from right to left, using the longest possible matches from the dictionary.
    func rightToLeftSegmentation(referenceText: String, dictionary: Set<String>, maxLength: Int) -> [String] {
        var result = [String]()
        var text = referenceText
        
        while !text.isEmpty {
            // If the length of the text is less than the maxLength, use the whole text as the substring
            let subText = text.count < maxLength ? text : String(text.suffix(maxLength))
            
            var currentSubText = subText
            while !currentSubText.isEmpty {
                // If the substring is in the dictionary or is a single character, add it to the result
                if dictionary.contains(currentSubText) || currentSubText.count == 1 {
                    result.append(currentSubText)
                    // Remove the substring from the text
                    text = String(text.dropLast(currentSubText.count))
                    break
                }
                // If the substring is not in the dictionary, shorten it by removing the first character
                currentSubText = String(currentSubText.dropFirst())
            }
        }
        
        // Reverse the result to ensure the correct order
        result.reverse()
        return result
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
        
        // You can adjust the segmentation silence timeout based on your real scenario
        speechConfig?.setPropertyTo("1500", by: SPXPropertyId.speechSegmentationSilenceTimeoutMs)
        
        let language = "zh-CN"
        
        speechConfig?.speechRecognitionLanguage = language

        let bundle = Bundle.main
        let path = bundle.path(forResource: "zhcn_continuous_mode_sample", ofType: "wav")
        if (path == nil) {
            print("Cannot find audio file!");
            self.updateLabel(text: "Cannot find audio file", color: UIColor.red)
            return;
        }
        print("pronunciation assessment audio file path: ", path!)

        let short_audio = bundle.path(forResource: "zhcn_short_dummy_sample", ofType: "wav")
        let referencePath = bundle.path(forResource: "zhcn_continuous_mode_sample", ofType: "txt")
        
        let audioConfig = SPXAudioConfiguration.init(wavFileInput: path!)

        let referenceText = try! String(contentsOfFile: referencePath!, encoding: .utf8)

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
        
        reco.addSessionStartedEventHandler { (reco, evt) in
            print("SESSION ID: \(evt.sessionId)")
        }
        
        try! pronConfig!.apply(to: reco)

        reco.addRecognizingEventHandler() {reco, evt in
            print("intermediate recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text, color: .gray)
        }

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
                recognizedWords.append(word.word!.lowercased())
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
        if (language == "zh-CN"){
             // Split words for Chinese using the reference text and any short wave file
             referenceWords = getReferenceWords(waveFilename: short_audio!, referenceText: referenceText, language: language, speechConfig: speechConfig!)!
         }
         else {
             for w in referenceText.words {
                 referenceWords.append(String(w).lowercased().trimmingCharacters(in: .punctuationCharacters))
             }
         }
         let enableMiscue = true
         var finalWords: [Word] = []
         var validWordCount = 0
         
         if (enableMiscue){
             let diff = referenceWords.outputDiffPathTraces(to: recognizedWords)
             for e in diff {
                 if e.from.x + 1 == e.to.x && e.from.y + 1 == e.to.y {
                     let v = pronWords[e.from.y]
                     finalWords.append(Word(word: v.word!,errorType: v.errorType!,accuracyScore: v.accuracyScore,duration: v.duration))
                     validWordCount += 1
                 } else if e.from.y < e.to.y {
                     let v = pronWords[e.from.y]
                     finalWords.append(Word(word: v.word!,errorType: "Insertion"))
                 } else {
                     finalWords.append(Word(word:referenceWords[e.from.x],errorType: "Omission"))
                 }
             }
         }
         else {
             for v in pronWords {
                 finalWords.append(Word(word: v.word!,errorType: v.errorType!,accuracyScore: v.accuracyScore,duration: v.duration))
             }
         }


         if (recognizedWords.count > 0) {
             // Overall accuracy and fluency scores are the weighted average of scores of all sentences.
             var totalAccuracyScore: Double = 0.0
             var accuracyCount: Int = 0
             var validCount: Int = 0
             var durationSum: Double = 0.0
             
             for word in finalWords {
                 if word.errorType != "Insertion" {
                     totalAccuracyScore += word.accuracyScore
                     accuracyCount += 1
                 }
                 
                 if word.errorType == "None" {
                     durationSum += word.duration + 0.01
                     validCount += 1
                 }
             }
             let accuracyScore = totalAccuracyScore / Double(accuracyCount)
             
             startOffset = pronWords[0].offset
             let fluencyScore: Double = durationSum / Double(endOffset - startOffset) * 100.0
             
             let completenessScore: Double = Double(validCount) / Double(accuracyCount) * 100.0
             
             let scores_all: [Double] = [accuracyScore, fluencyScore, completenessScore]
             let pronunciationScore = scores_all.map{$0 * 0.2}.reduce(0, +) + scores_all.min()! * 0.4
             
             var resultText = String(format: "Assessment finished. \nOverall accuracy score: %.2f, fluency score: %.2f, completeness score: %.2f, pronunciation score: %.2f", accuracyScore, fluencyScore, completenessScore, pronunciationScore)

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
        
        let stopRecognitionSemaphore = DispatchSemaphore(value: 0)
        speechRecognizer.addSessionStartedEventHandler { (recognizer, eventArgs) in
            print("SESSION ID: \(eventArgs.sessionId)")
        }
        
        let referenceText = "what's the weather like"
        let pronAssessmentConfig = try! SPXPronunciationAssessmentConfiguration(referenceText, gradingSystem: SPXPronunciationAssessmentGradingSystem.hundredMark, granularity: SPXPronunciationAssessmentGranularity.word, enableMiscue: true)
        
        pronAssessmentConfig.enableProsodyAssessment()
        
        try! pronAssessmentConfig.apply(to: speechRecognizer)
        
        audioInputStream.write(Data(audioData))
        audioInputStream.write(Data())
        
        self.updateLabel(text: "Analysising", color: UIColor.black)
        
        DispatchQueue.global().async {
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
                stopRecognitionSemaphore.signal()
            }
        }
        stopRecognitionSemaphore.wait()
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
        
        let stopRecognitionSemaphore = DispatchSemaphore(value: 0)
        recognizer.addSessionStartedEventHandler { (sender, evt) in
            print("SESSION ID: \(evt.sessionId)")
        }
        
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
            stopRecognitionSemaphore.signal()
        }
        stopRecognitionSemaphore.wait()
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
        
        let stopRecognitionSemaphore = DispatchSemaphore(value: 0)
        recognizer.addSessionStartedEventHandler { (sender, evt) in
            print("SESSION ID: \(evt.sessionId)")
        }
        
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
            stopRecognitionSemaphore.signal()
        }
        stopRecognitionSemaphore.wait()
    }

    func updateLabel(text: String?, color: UIColor) {
        DispatchQueue.main.async {
            self.label.text = text
            self.label.textColor = color
        }
    }
}

