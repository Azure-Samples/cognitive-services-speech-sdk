// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

// <code>
import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    var label: NSTextField!
    var fromMicButton: NSButton!

    var sub: String!
    var region: String!

    @IBOutlet weak var window: NSWindow!

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        print("loading")
        // load subscription information
        sub = "YourSubscriptionKey"
        region = "YourServiceRegion"

        label = NSTextField(frame: NSRect(x: 100, y: 50, width: 200, height: 200))
        label.textColor = NSColor.black
        label.lineBreakMode = .byWordWrapping

        label.stringValue = "Recognition Result"
        label.isEditable = false

        self.window.contentView?.addSubview(label)

        fromMicButton = NSButton(frame: NSRect(x: 100, y: 300, width: 200, height: 30))
        fromMicButton.title = "Recognize"
        fromMicButton.target = self
        fromMicButton.action = #selector(fromMicButtonClicked)
        self.window.contentView?.addSubview(fromMicButton)
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
        print("recognition result: \(result.text ?? "(no result)"), reason: \(result.reason.rawValue)")
        updateLabel(text: result.text, color: .black)

        if result.reason != SPXResultReason.recognizedSpeech {
            let cancellationDetails = try! SPXCancellationDetails(fromCanceledRecognitionResult: result)
            print("cancelled: \(result.reason), \(cancellationDetails.errorDetails)")
            updateLabel(text: "Error: \(cancellationDetails.errorDetails)", color: .red)
        }
    }

    func updateLabel(text: String?, color: NSColor) {
        DispatchQueue.main.async {
            self.label.stringValue = text!
            self.label.textColor = color
        }
    }
}
// </code>

