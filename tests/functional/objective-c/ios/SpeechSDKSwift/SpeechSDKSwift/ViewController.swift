//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import UIKit

class ViewController: UIViewController {
    var label: UILabel!
    var fromFileButton: UIButton!
    var fromMicButton: UIButton!

    var sub: String!
    var region: String!

    override func viewDidLoad() {
        super.viewDidLoad()

        // load subscription information
        sub = ProcessInfo.processInfo.environment["subscriptionKey"]
        if sub == nil {
            sub = "YourSubscriptionKey"
        }
        region = ProcessInfo.processInfo.environment["serviceRegion"]
        if region == nil {
            region = "YouServiceRegion"
        }

        label = UILabel(frame: CGRect(x: 100, y: 100, width: 200, height: 200))
        label.textColor = UIColor.black
        label.lineBreakMode = .byWordWrapping
        label.numberOfLines = 0

        label.text = "Recognition Result"

        fromFileButton = UIButton(frame: CGRect(x: 100, y: 350, width: 100, height: 50))
        fromFileButton.setTitle("Recognize (File)!", for: .normal)
        fromFileButton.addTarget(self, action:#selector(self.fromFileButtonClicked), for: .touchUpInside)
        fromFileButton.setTitleColor(UIColor.black, for: .normal)

        fromMicButton = UIButton(frame: CGRect(x: 100, y: 400, width: 100, height: 50))
        fromMicButton.setTitle("Recognize (Mic)!", for: .normal)
        fromMicButton.addTarget(self, action:#selector(self.fromMicButtonClicked), for: .touchUpInside)
        fromMicButton.setTitleColor(UIColor.black, for: .normal)

        self.view.addSubview(label)
        self.view.addSubview(fromFileButton)
        self.view.addSubview(fromMicButton)
    }

    @objc func fromFileButtonClicked() {
        DispatchQueue.global(qos: .userInitiated).async {
            self.recognizeFromFile()
        }
    }

    func recognizeFromFile() {
        var speechConfig: SPXSpeechConfiguration?
        do {
            try speechConfig = SPXSpeechConfiguration(subscription: sub, region: region)
        } catch {
            print("error \(error) happened")
            speechConfig = nil
        }

        speechConfig?.speechRecognitionLanguage = "en-US"
        let filePath = Bundle.main.url(forResource: "whatstheweatherlike", withExtension: "wav")!
        print("found wavfile: \(filePath.path)")
        let audioConfig = SPXAudioConfiguration(wavFileInput: filePath.path)

        var reco: SPXSpeechRecognizer!
        do {
            reco = try SPXSpeechRecognizer(speechConfiguration: speechConfig!, audioConfiguration: audioConfig!)
        } catch {
            print("error \(error) happened")
        }

        var conn: SPXConnection?
        do {
            conn = try SPXConnection(from: reco!)
        } catch {
            print("conn: error \(error) happened")
        }

        conn!.addConnectedEventHandler({conn, evt in
            print("connected to session \(evt.sessionId)!")
        })
        conn!.addDisconnectedEventHandler() { conn, evt in
            print("disconnected from session \(evt.sessionId)!")
        }

        conn!.open(false)

        reco.addRecognizingEventHandler() {reco, evt in
            print("intermediate recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text, color: .gray)
        }
        updateLabel(text: "Listening ...", color: .gray)

        DispatchQueue.main.async {
            self.label.text = "Listening ..."
            self.label.textColor = .gray
        }

        updateLabel(text: "Listening ...", color: .gray)
        print("Listening...")

        let result = try! reco.recognizeOnce()
        print("recognition result: \(result.text ?? "(no result)")")
        updateLabel(text: result.text, color: .black)

        conn!.close()
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

        let reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig!, audioConfiguration: audioConfig!)

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

