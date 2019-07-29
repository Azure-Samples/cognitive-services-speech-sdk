//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
import UIKit

class ViewController: UIViewController {
    var label: UILabel!
    var fromMicButton: UIButton!
    
    var sub: String!
    var region: String!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // load subscription information
        sub = "YourSubscriptionKey"
        region = "YourServiceRegion"
        
        label = UILabel(frame: CGRect(x: 100, y: 100, width: 200, height: 200))
        label.textColor = UIColor.black
        label.lineBreakMode = .byWordWrapping
        label.numberOfLines = 0
        
        label.text = "Recognition Result"
        
        fromMicButton = UIButton(frame: CGRect(x: 100, y: 400, width: 200, height: 50))
        fromMicButton.setTitle("Recognize", for: .normal)
        fromMicButton.addTarget(self, action:#selector(self.fromMicButtonClicked), for: .touchUpInside)
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
        var speechConfig: SPXSpeechConfiguration?
        do {
            try speechConfig = SPXSpeechConfiguration(subscription: sub, region: region)
        } catch {
            print("error \(error) happened")
            speechConfig = nil
        }
        speechConfig?.speechRecognitionLanguage = "en-US"
        
        let audioConfig = SPXAudioConfiguration()
        
        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig!, audioConfiguration: audioConfig)
        
        reco.addRecognizingEventHandler() {reco, evt in
            print("intermediate recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text, color: .gray)
        }
        
        updateLabel(text: "Listening ...", color: .gray)
        print("Listening...")
        
        let result = try! reco.recognizeOnce()
        print("recognition result: \(result.text ?? "(no result)")")
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
