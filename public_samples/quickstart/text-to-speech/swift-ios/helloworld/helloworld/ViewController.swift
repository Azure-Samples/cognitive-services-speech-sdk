//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
import UIKit
import AVFoundation

var player: AVAudioPlayer?

class ViewController: UIViewController, UITextFieldDelegate, AVAudioPlayerDelegate{
    var textField: UITextField!
    var synthesisButton: UIButton!
    
    var inputText: String!
    
    var sub: String!
    var region: String!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // load subscription information
        sub = "YourSubscriptionKey"
        region = "YourServiceRegion"
        
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
        var speechConfig: SPXSpeechConfiguration?
        do {
            try speechConfig = SPXSpeechConfiguration(subscription: sub, region: region)
        } catch {
            print("error \(error) happened")
            speechConfig = nil
        }
        speechConfig?.setSpeechSynthesisOutputFormat( SPXSpeechSynthesisOutputFormat.audio16Khz32KBitRateMonoMp3)
        let synthesizer = try! SPXSpeechSynthesizer(speechConfiguration: speechConfig!, audioConfiguration: nil)
        if inputText.isEmpty {
            return
        }
        let result = try! synthesizer.speakText(inputText)
        do {
            try player = AVAudioPlayer(data: result.audioData!)
            player?.delegate = self
            player?.prepareToPlay()
        } catch {
            print("error \(error) happened")
            player = nil
        }
        player?.delegate = self
        player?.play()
    }
}
// </code>
