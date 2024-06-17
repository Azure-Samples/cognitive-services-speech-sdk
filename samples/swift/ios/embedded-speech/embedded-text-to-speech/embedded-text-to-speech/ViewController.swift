// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import UIKit
import MicrosoftCognitiveServicesSpeech

/// The name of the folder containing local embedded speech synthesis voices for use in the application.
/// This folder should contain one or more speech synthesis voices that are used for text-to-speech conversion.
/// By default, this sample assumes that the embedded text-to-speech voices are stored under the "TTS" folder
/// in the application's resources bundle. You can modify this variable to specify a different folder name.
///
/// Files belonging to a specific voice must be available as normal individual files in a voice folder,
/// not inside an archive, and they must be readable by the application process.
let EmbeddedSpeechSynthesisVoicesFolderName = "TTS"

/// Name of the embedded speech synthesis voice to be used for synthesis.
/// If changed from the default, this will override SpeechSynthesisLocale.
/// For example: "en-US-JennyNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)"
let EmbeddedSpeechSynthesisVoiceName = "YourEmbeddedSpeechSynthesisVoiceName"

/// Decryption key of the (encrypted) embedded speech synthesis voice.
/// WARNING: The key may be visible in the program binary if hard-coded as a plain string.
let EmbeddedSpeechSynthesisVoiceKey = "YourEmbeddedSpeechSynthesisVoiceKey"


class ViewController: UIViewController, UITextFieldDelegate{
    var textField: UITextField!
    var synthesisButton: UIButton!
    
    var inputText: String!
    var embeddedSpeechConfig: SPXEmbeddedSpeechConfiguration?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Setup embedded speech to text (STT) model path
        let bundle = Bundle(for: type(of: self))
        if let absoluteModelPath = bundle.path(forResource: EmbeddedSpeechSynthesisVoicesFolderName, ofType: nil) {
            do {
                embeddedSpeechConfig = try SPXEmbeddedSpeechConfiguration(fromPath: absoluteModelPath)
                embeddedSpeechConfig?.setSpeechSynthesisVoice(EmbeddedSpeechSynthesisVoiceName,
                                                                key: EmbeddedSpeechSynthesisVoiceKey)
            }
            catch {
                print("Error: \(error) in initializing embedded speech configuration.")
                embeddedSpeechConfig = nil
            }
        } else {
            print("Error: Unable to locate the specified embedded speech synthesis voice.")
        }
        setupUI()
    }
    
    func setupUI() {
        textField = UITextField(frame: CGRect(x: 100, y:250, width: 200, height: 50))
        textField.textColor = UIColor.black
        textField.borderStyle = UITextField.BorderStyle.roundedRect
        textField.placeholder = "Type something to synthesize."
        textField.delegate = self
        
        inputText = ""
        
        synthesisButton = UIButton(frame: CGRect(x: 100, y: 400, width: 200, height: 50))
        synthesisButton.setTitle("Synthesize", for: .normal)
        synthesisButton.addTarget(self, action:#selector(self.synthesisButtonClicked), for: .touchUpInside)
        synthesisButton.setTitleColor(UIColor.black, for: .normal)
        
        self.view.addSubview(textField)
        self.view.addSubview(synthesisButton)
    }
    
    func textField(_ textField: UITextField, shouldChangeCharactersIn range: NSRange, replacementString string: String) -> Bool {
        if let text = textField.text,
        let textRange = Range(range, in: text) {
            self.inputText = text.replacingCharacters(in: textRange, with: string)
        }
        return true
    }
    
    @objc func synthesisButtonClicked() {
        DispatchQueue.global(qos: .userInitiated).async {
            self.synthesisToSpeaker()
        }
    }
    
    func synthesisToSpeaker() {
        let synthesizer = try! SPXSpeechSynthesizer(embeddedSpeechConfiguration: embeddedSpeechConfig!)
        if inputText.isEmpty {
            return
        }
        let result = try! synthesizer.speakText(inputText)
        if result.reason == SPXResultReason.canceled
        {
            let cancellationDetails = try! SPXSpeechSynthesisCancellationDetails(fromCanceledSynthesisResult: result)
            print("cancelled, detail: \(cancellationDetails.errorDetails!) ")
        }
    }
}

