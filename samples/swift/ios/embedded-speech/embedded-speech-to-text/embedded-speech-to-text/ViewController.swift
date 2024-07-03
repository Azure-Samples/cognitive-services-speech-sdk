//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
import UIKit
import MicrosoftCognitiveServicesSpeech

/// The name of the folder containing local embedded speech recognition models for use in the application.
/// This folder should contain one or more speech recognition models that are used for speech-to-text conversion.
/// By default, this sample assumes that the embedded speech-to-text models are stored under the "STT" folder
/// in the application's resources bundle. You can modify this variable to specify a different folder name.
///
/// Important: The files for each model must be available as individual files within the specified folder,
/// not packaged inside an archive or other container format. Additionally, these files must be readable
/// by the application process for proper functionality.
let EmbeddedSpeechRecognitionModelFolderName = "STT"

/// Name of the embedded speech recognition model to be used for recognition.
/// If changed from the default, this will override SpeechRecognitionLocale.
/// For example: "en-US" or "Microsoft Speech Recognizer en-US FP Model V8"
let EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName"

/// Decryption key of the (encrypted) embedded speech recognition model.
/// WARNING: The key may be visible in the program binary if hard-coded as a plain string.
let EmbeddedSpeechRecognitionModelKey = "YourEmbeddedSpeechRecognitionModelKey"


class ViewController: UIViewController {
    var label: UILabel!
    var fromMicButton: UIButton!
    
    var embeddedSpeechConfig: SPXEmbeddedSpeechConfiguration?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Setup embedded speech to text (STT) model path
        let bundle = Bundle(for: type(of: self))
        if let absoluteModelPath = bundle.path(forResource: EmbeddedSpeechRecognitionModelFolderName, ofType: nil) {
            do {
                embeddedSpeechConfig = try SPXEmbeddedSpeechConfiguration(fromPath: absoluteModelPath)
                embeddedSpeechConfig?.setSpeechRecognitionModel(EmbeddedSpeechRecognitionModelName,
                                                                key: EmbeddedSpeechRecognitionModelKey)
            }
            catch {
                print("Error: \(error) in initializing embedded speech configuration.")
                embeddedSpeechConfig = nil
            }
        } else {
            print("Error: Unable to locate the specified embedded speech recognition model.")
        }

        setupUI()
    }
    
    func setupUI() {
        label = UILabel(frame: CGRect(x: 100, y: 100, width: 200, height: 200))
        label.textColor = UIColor.black
        label.lineBreakMode = .byWordWrapping
        label.numberOfLines = 0
        label.text = "Recognition Result"
        
        fromMicButton = UIButton(frame: CGRect(x: 100, y: 400, width: 200, height: 50))
        fromMicButton.setTitle("Recognize", for: .normal)
        fromMicButton.addTarget(self, action: #selector(self.fromMicButtonClicked), for: .touchUpInside)
        fromMicButton.setTitleColor(UIColor.black, for: .normal)
        
        self.view.addSubview(label)
        self.view.addSubview(fromMicButton)
    }
    
    @objc func fromMicButtonClicked() {
        DispatchQueue.global(qos: .userInitiated).async {
            self.recognizeFromMic()
        }
    }
    
    func recognizeFromMic() {
        let audioConfig = SPXAudioConfiguration()
        let reco = try! SPXSpeechRecognizer(embeddedSpeechConfiguration:embeddedSpeechConfig!, audioConfiguration: audioConfig)
        
        reco.addRecognizingEventHandler() {reco, evt in
            print("Intermediate recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text, color: .gray)
        }
        
        updateLabel(text: "Listening ...", color: .gray)
        print("Listening...")
        
        let result = try! reco.recognizeOnce()
        print("Recognition result: \(result.text ?? "(no result)")")
        updateLabel(text: result.text, color: .black)
    }
    
    func updateLabel(text: String?, color: UIColor) {
        DispatchQueue.main.async {
            self.label.text = text
            self.label.textColor = color
        }
    }
}
// </code>
