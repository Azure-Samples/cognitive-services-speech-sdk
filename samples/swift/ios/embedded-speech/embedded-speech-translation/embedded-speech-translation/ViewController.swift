//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
import UIKit
import MicrosoftCognitiveServicesSpeech

/// The name of the folder containing local embedded speech translation models for use in the application.
/// This may be a single model folder or a top-level folder for several models.
/// By default, this sample assumes that the embedded speech translation models are stored under the "ST" folder
/// in the application's resources bundle. You can modify this variable to specify a different folder name.
///
/// Important: The files for each model must be available as individual files within the specified folder,
/// not packaged inside an archive or other container format. Additionally, these files must be readable
/// by the application process for proper functionality.
let EmbeddedSpeechTranslationModelFolderName = "ST"

/// Name of the embedded speech translation model to be used for translation.
/// For example: "Microsoft Speech Translator Many-to-English Model V3"
let EmbeddedSpeechTranslationModelName = "YourEmbeddedSpeechTranslationModelName"

/// Decryption key of the (encrypted) embedded speech translation model.
/// WARNING: The key may be visible in the program binary if hard-coded as a plain string.
let EmbeddedSpeechTranslationModelKey = "YourEmbeddedSpeechTranslationModelKey"


class ViewController: UIViewController {
    var label: UILabel!
    var fromMicButton: UIButton!
    
    var embeddedSpeechConfig: SPXEmbeddedSpeechConfiguration?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Setup embedded speech translation (ST) model path
        let bundle = Bundle(for: type(of: self))
        if let absoluteModelPath = bundle.path(forResource: EmbeddedSpeechTranslationModelFolderName, ofType: nil) {
            do {
                embeddedSpeechConfig = try SPXEmbeddedSpeechConfiguration(fromPath: absoluteModelPath)
                embeddedSpeechConfig?.setSpeechTranslationModel(EmbeddedSpeechTranslationModelName,
                                                                key: EmbeddedSpeechTranslationModelKey)
            }
            catch {
                print("Error: \(error) in initializing embedded speech configuration.")
                embeddedSpeechConfig = nil
            }
        } else {
            print("Error: Unable to locate the specified embedded speech translation model.")
        }

        setupUI()
    }
    
    func setupUI() {
        label = UILabel(frame: CGRect(x: 100, y: 100, width: 200, height: 200))
        label.textColor = UIColor.black
        label.lineBreakMode = .byWordWrapping
        label.numberOfLines = 0
        label.text = "Translation result"
        
        fromMicButton = UIButton(frame: CGRect(x: 100, y: 400, width: 200, height: 50))
        fromMicButton.setTitle("Translate", for: .normal)
        fromMicButton.addTarget(self, action: #selector(self.fromMicButtonClicked), for: .touchUpInside)
        fromMicButton.setTitleColor(UIColor.black, for: .normal)
        
        self.view.addSubview(label)
        self.view.addSubview(fromMicButton)
    }
    
    @objc func fromMicButtonClicked() {
        DispatchQueue.global(qos: .userInitiated).async {
            self.translateFromMic()
        }
    }
    
    func translateFromMic() {
        let audioConfig = SPXAudioConfiguration()
        let translator = try! SPXTranslationRecognizer(embeddedSpeechConfiguration:embeddedSpeechConfig!, audioConfiguration: audioConfig)
        
        translator.addRecognizingEventHandler() {reco, evt in
            print("Intermediate translation result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text, color: .gray)
        }
        
        updateLabel(text: "Listening ...", color: .gray)
        print("Listening...")
        
        let result = try! translator.recognizeOnce()
        let translationDictionary = result.translations
        let translationResult = translationDictionary["en"] as? String
        print("English translation result: \(translationResult ?? "(no result)")")
        updateLabel(text: translationResult, color: .black)
    }
    
    func updateLabel(text: String?, color: UIColor) {
        DispatchQueue.main.async {
            self.label.text = text
            self.label.textColor = color
        }
    }
}
// </code>
