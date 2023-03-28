//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
import UIKit
import AVFoundation
import MicrosoftCognitiveServicesSpeech

extension AVAudioPCMBuffer {
    func data() -> Data {
        var nBytes = 0
        nBytes = Int(self.frameLength * (self.format.streamDescription.pointee.mBytesPerFrame))
        var range: NSRange = NSRange()
        range.location = 0
        range.length = nBytes
        let buffer = NSMutableData()
        buffer.replaceBytes(in: range, withBytes: (self.int16ChannelData![0]))
        return buffer as Data
    }
    
    var duration: TimeInterval {
        format.sampleRate > 0 ? .init(frameLength) / format.sampleRate : 0
    }
}

class ViewController: UIViewController {
    var label: UILabel!
    var fromMicButton: UIButton!
    
    var sub: String!
    var region: String!

    var sampleRate = 16000
    var bufferSize = 2048
    var audioEngine: AVAudioEngine = AVAudioEngine()
    var conversionQueue = DispatchQueue(label: "conversionQueue")
    
    var speechConfig: SPXSpeechConfiguration!
    var audioConfig: SPXAudioConfiguration!
    var reco: SPXSpeechRecognizer!
    var pushStream: SPXPushAudioInputStream!
    
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
        
    func readDataFromMicrophone() {
        let inputNode = audioEngine.inputNode
        let inputFormat = inputNode.outputFormat(forBus: 0)
        let recordingFormat = AVAudioFormat(commonFormat: .pcmFormatInt16, sampleRate: Double(self.sampleRate), channels: 1, interleaved: false)

        guard let formatConverter =  AVAudioConverter(from:inputFormat, to: recordingFormat!)
        else {
            return
        }
        // Install a tap on the audio engine with the buffer size and the input format.
        audioEngine.inputNode.installTap(onBus: 0, bufferSize: AVAudioFrameCount(bufferSize), format: inputFormat) { (buffer, time) in
                        
            self.conversionQueue.async { [self] in
                // Convert the microphone input to the recording format required                
                let outputBufferCapacity = AVAudioFrameCount(buffer.duration * recordingFormat!.sampleRate)

                guard let pcmBuffer = AVAudioPCMBuffer(pcmFormat: recordingFormat!, frameCapacity: outputBufferCapacity) else {
                    print("Failed to create new pcm buffer")
                    return
                }
                pcmBuffer.frameLength = outputBufferCapacity
                
                var error: NSError? = nil
                let inputBlock: AVAudioConverterInputBlock = {inNumPackets, outStatus in
                    outStatus.pointee = AVAudioConverterInputStatus.haveData
                    return buffer
                }
                formatConverter.convert(to: pcmBuffer, error: &error, withInputFrom: inputBlock)
                
                if error != nil {
                    print(error!.localizedDescription)
                }
                else {
                    self.pushStream.write((pcmBuffer.data()))
                }
            }
        }
        audioEngine.prepare()
        do {
            try audioEngine.start()
        }
        catch {
            print(error.localizedDescription)
        }
    }

    func recognizeFromMic() {
        try! speechConfig = SPXSpeechConfiguration(subscription: sub, region: region)
        speechConfig?.speechRecognitionLanguage = "en-US"
        
        pushStream = SPXPushAudioInputStream()
        audioConfig = SPXAudioConfiguration(streamInput: pushStream)
        reco = try! SPXSpeechRecognizer(speechConfiguration: speechConfig!, audioConfiguration: audioConfig!)
        
        reco.addRecognizedEventHandler() {reco, evt in
            print("Final recognition result: \(evt.result.text ?? "(no result)")")
            self.updateLabel(text: evt.result.text, color: .gray)
        }
        
        reco.addCanceledEventHandler { reco, evt in
            print("Recognition canceled: \(evt.errorDetails?.description ?? "(no result)")")
            self.updateLabel(text: evt.errorDetails?.description, color: .gray)
        }
        
        try! reco.recognizeOnceAsync({ srresult in
            self.audioEngine.stop()
            self.audioEngine.inputNode.removeTap(onBus: 0)
            self.pushStream.close()
        })
        readDataFromMicrophone()
        updateLabel(text: "Listening ...", color: .gray)
    }
    
    func updateLabel(text: String?, color: UIColor) {
        DispatchQueue.main.async {
            self.label.text = text
            self.label.textColor = color
        }
    }
}
// </code>
