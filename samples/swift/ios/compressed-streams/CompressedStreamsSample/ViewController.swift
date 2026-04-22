//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import UIKit
import MicrosoftCognitiveServicesSpeech

class ViewController: UIViewController {
    var label: UILabel!
    var recognizeButton: UIButton!

    let speechKey = "YourSubscriptionKey"
    let serviceRegion = "YourServiceRegion"

    override func viewDidLoad() {
        super.viewDidLoad()

        view.backgroundColor = .white

        recognizeButton = UIButton(frame: CGRect(x: 50, y: 100, width: 300, height: 50))
        recognizeButton.setTitle("Recognize from MP3 file", for: .normal)
        recognizeButton.setTitleColor(.systemBlue, for: .normal)
        recognizeButton.accessibilityIdentifier = "recognize_mp3_button"
        recognizeButton.addTarget(self, action: #selector(recognizeButtonTapped), for: .touchUpInside)
        view.addSubview(recognizeButton)

        label = UILabel(frame: CGRect(x: 50, y: 200, width: 300, height: 400))
        label.textColor = .gray
        label.lineBreakMode = .byWordWrapping
        label.numberOfLines = 0
        label.accessibilityIdentifier = "result_label"
        label.text = "Press a button!"
        view.addSubview(label)
    }

    @objc func recognizeButtonTapped() {
        DispatchQueue.global(qos: .default).async {
            self.recognizeFromMP3File()
        }
    }

    /*
     * Performs speech recognition from an MP3 file using a push stream.
     */
    func recognizeFromMP3File() {
        guard let weatherFilePath = Bundle.main.path(forResource: "whatstheweatherlike", ofType: "mp3") else {
            print("Cannot find audio file!")
            updateLabel(text: "Cannot find audio file", color: .red)
            return
        }

        // <setup-stream>
        let audioFormat = SPXAudioStreamFormat(usingCompressedFormat: .mp3)!
        let pushStream = SPXPushAudioInputStream(audioFormat: audioFormat)!
        let audioConfig = SPXAudioConfiguration(streamInput: pushStream)!
        // </setup-stream>

        guard let speechConfig = try? SPXSpeechConfiguration(subscription: speechKey, region: serviceRegion) else {
            print("Could not load speech config")
            updateLabel(text: "Speech Config Error", color: .red)
            return
        }

        guard let recognizer = try? SPXSpeechRecognizer(speechConfiguration: speechConfig, audioConfiguration: audioConfig) else {
            print("Could not create speech recognizer")
            updateLabel(text: "Speech Recognition Error", color: .red)
            return
        }

        recognizer.addRecognizingEventHandler { [weak self] _, evt in
            print("Recognizing: \(evt.result.text ?? "")")
            self?.updateLabel(text: evt.result.text, color: .gray)
        }

        recognizer.addRecognizedEventHandler { [weak self] _, evt in
            print("Recognized: \(evt.result.text ?? "")")
            self?.updateLabel(text: evt.result.text, color: .black)
        }

        // <push-compressed-stream>
        guard let inputStream = InputStream(fileAtPath: weatherFilePath) else {
            print("Error opening audio file")
            updateLabel(text: "Error opening audio file", color: .red)
            return
        }
        inputStream.open()

        updateLabel(text: "Recognizing from push stream...", color: .gray)
        recognizer.startContinuousRecognition()

        let bufferSize = 1000
        let buffer = UnsafeMutablePointer<UInt8>.allocate(capacity: bufferSize)
        defer { buffer.deallocate() }

        while true {
            let bytesRead = inputStream.read(buffer, maxLength: bufferSize)
            if bytesRead == 0 {
                print("End of stream reached")
                pushStream.close()
                break
            } else if bytesRead < 0 {
                print("Error reading stream: \(inputStream.streamError?.localizedDescription ?? "unknown")")
                pushStream.close()
                break
            } else {
                let data = Data(bytes: buffer, count: bytesRead)
                pushStream.write(data)
            }
            Thread.sleep(forTimeInterval: 0.1)
        }

        recognizer.stopContinuousRecognition()
        // </push-compressed-stream>
    }

    func updateLabel(text: String?, color: UIColor) {
        DispatchQueue.main.async {
            self.label.text = text
            self.label.textColor = color
        }
    }
}
