// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

// <code>
import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    let speechKey: String! = "YourSubscriptionKey"
    let serviceRegion: String! = "YourServiceRegion"

    let keywordModelName: String! = "kws"
    let keywordText: String! = "computer"
    let useAsyncAPI: Bool = true

    var recognizer: SPXSpeechRecognizer? = nil
    var keywordModel: SPXKeywordRecognitionModel? = nil
    var isRecognitionActive: Bool = false

    @IBOutlet weak var window: NSWindow!
    var label: NSTextField!
    var button: NSButton!

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        var speechConfig: SPXSpeechConfiguration?
        do {
            try speechConfig = SPXSpeechConfiguration(subscription: speechKey, region: serviceRegion)
        } catch {
            print("error \(error) happened")
            speechConfig = nil
        }
        speechConfig!.speechRecognitionLanguage = "en-US"
        let audioConfig = SPXAudioConfiguration()

        self.recognizer = try! SPXSpeechRecognizer(speechConfiguration: speechConfig!, audioConfiguration: audioConfig)

        self.recognizer!.addRecognizingEventHandler() {reco, evt in
            print("intermediate recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text!)
        }
        self.recognizer!.addRecognizedEventHandler() {reco, evt in
            let prefix = (evt.result.reason == SPXResultReason.recognizedKeyword) ? "KEYWORD:" : ""
            print("final recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: "\(prefix)\(evt.result.text!)")
        }
        self.recognizer!.addCanceledEventHandler {reco, evt in
            let cancellationDetails = "reason=\(evt.reason), errorCode=\(evt.errorCode), errorDetails=\(evt.errorDetails!))"
            print("canceled: \(cancellationDetails)")
            if (evt.reason == SPXCancellationReason.error) {
                self.updateLabel(text: "Error: \(cancellationDetails)")
            }
        }

        let keywordModelFile = Bundle.main.url(forResource: self.keywordModelName, withExtension: "table")
        self.keywordModel = try! SPXKeywordRecognitionModel(fromFile: keywordModelFile!.path)

        let labelWidth: CGFloat = 400;
        let labelHeight: CGFloat = 100;
        let buttonWidth: CGFloat = 200
        let buttonHeight: CGFloat = 30
        let verticalSpacing: CGFloat = 20

        let contentViewSize = self.window.contentView?.bounds.size
        let labelX = (contentViewSize!.width - labelWidth) / 2
        let labelY = (contentViewSize!.height - labelHeight - verticalSpacing - buttonHeight) / 2 + verticalSpacing + buttonHeight
        let buttonX = (contentViewSize!.width - buttonWidth) / 2
        let buttonY = labelY - verticalSpacing - buttonHeight

        label = NSTextField(frame: NSRect(x: labelX, y: labelY, width: labelWidth, height: labelHeight))
        label.alignment = .center
        label.drawsBackground = false
        label.isBezeled = false
        label.isEditable = false
        label.lineBreakMode = .byWordWrapping
        label.stringValue = "Press the button to start recognition."
        self.window.contentView?.addSubview(label)

        button = NSButton(frame: NSRect(x: buttonX, y: buttonY, width: buttonWidth, height: buttonHeight))
        button.title = "Start Recognition"
        button.target = self
        button.action = #selector(buttonPressed)
        self.window.contentView?.addSubview(button)
    }

    @objc func buttonPressed() {
        DispatchQueue.global(qos: .userInitiated).async {
            self.recognizeFromMic()
        }
    }

    func recognizeFromMic() {
        if (!self.isRecognitionActive) {
            if (self.useAsyncAPI) {
                try! self.recognizer!.startKeywordRecognitionAsync(self.keywordModel!, completionHandler:{
                    print("started listening")
                })
            } else {
                try! self.recognizer!.startKeywordRecognition(self.keywordModel!)
                print("started listening")
            }
            self.isRecognitionActive = true
            updateButton(text: "Stop Recognition")
            updateLabel(text: "Say something starting with '\(self.keywordText!)', press the button to stop...")
        } else {
            if (self.useAsyncAPI) {
                try! self.recognizer!.stopKeywordRecognitionAsync(/* completionHandler */{
                    print("stopped listening")
                })
            } else {
                try! self.recognizer!.stopKeywordRecognition()
                print("stopped listening")
            }
            self.isRecognitionActive = false
            updateButton(text: "Start Recognition")
            updateLabel(text: "Press the button to start recognition.")
        }
    }

    func updateLabel(text: String?) {
        DispatchQueue.main.async {
            self.label.stringValue = text!
        }
    }

    func updateButton(text: String?) {
        DispatchQueue.main.async {
            self.button.title = text!
        }
    }

    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        return true
    }
}
// </code>
