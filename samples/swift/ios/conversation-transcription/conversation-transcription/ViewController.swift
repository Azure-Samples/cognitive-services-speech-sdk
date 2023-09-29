//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import UIKit
import AVFoundation
import MicrosoftCognitiveServicesSpeech

class ViewController: UIViewController {
    var label: UILabel!
    var fromFileButton: UIButton!
    
    var sub: String!
    var region: String!

    var conversationSub: String! = "YourSubscriptionKey"
    var conversationRegion: String! = "YourServiceRegion"
    
    var transcriber: SPXConversationTranscriber? = nil
    var audioConfig: SPXAudioConfiguration? = nil
    var speechConfig: SPXSpeechConfiguration? = nil;
    var operationCompleted = false
    let audioFilename = "katiesteve_mono"
 
    override func viewDidLoad() {
        super.viewDidLoad()
        
        label = UILabel(frame: CGRect(x: 50, y: 100, width: 200, height: 600))
        label.textColor = UIColor.black
        label.lineBreakMode = .byWordWrapping
        label.numberOfLines = 0
        
        label.text = "Transcription Result"
        
        fromFileButton = UIButton(frame: CGRect(x: 50, y: 700, width: 200, height: 50))
        fromFileButton.setTitle("Start Transcription", for: .normal)
        fromFileButton.addTarget(self, action:#selector(self.fromFileButtonClicked), for: .touchUpInside)
        fromFileButton.setTitleColor(UIColor.black, for: .normal)
        
        self.view.addSubview(label)
        self.view.addSubview(fromFileButton)
    }
    
    @objc func fromFileButtonClicked() {
        DispatchQueue.global(qos: .userInitiated).async {
            self.configureAudio()
            self.configureTranscriber()
            self.transcribeFromFile()
        }
    }

    func configureTranscriber() {
        do {
            try self.speechConfig = SPXSpeechConfiguration(subscription: conversationSub, region: conversationRegion)
        } catch {
            print("error \(error) happened")
            speechConfig = nil
        }
        self.transcriber = try! SPXConversationTranscriber(speechConfiguration: self.speechConfig!, audioConfiguration: self.audioConfig!)
        
        self.transcriber!.addTranscribedEventHandler() {reco, evt in
            print("final transcription result: \(evt.result?.text ?? "(no result)") speakerid: \(evt.result?.speakerId ?? "(no result)")")
            self.updateLabel(text: (evt.result?.text)! + "\nspeakerId:" + (evt.result?.speakerId)!, color: .gray)
        }
        self.transcriber!.addSessionStartedEventHandler() {_, evt in
            print("session started")
        }
        self.transcriber!.addSessionStoppedEventHandler() {[unowned self] _, evt in
            print("session stopped")
            self.operationCompleted = true
        }
        self.transcriber!.addCanceledEventHandler() {_, evt in
            print("session canceled")
        }
    }

    func configureAudio() {
        let filePath = Bundle.main.path(forResource: audioFilename, ofType: "wav")
        let targetUrl = URL(string: filePath!)
        let audioFile = try! AVAudioFile(forReading: targetUrl!, commonFormat: .pcmFormatInt16, interleaved: false)
        let bytesPerFrame = audioFile.fileFormat.streamDescription.pointee.mBytesPerFrame

        let stream: SPXPullAudioInputStream = SPXPullAudioInputStream.init(readHandler: {
            (data:NSMutableData, size:UInt) -> Int in

            let buffer = AVAudioPCMBuffer.init(pcmFormat: audioFile.fileFormat, frameCapacity: AVAudioFrameCount(size)/bytesPerFrame)
            var success = false
            do {
                if buffer != nil {
                    try audioFile.read(into: buffer!)
                    success = true
                } else {
                    print("AVAudioPCMBuffer is nil")
                }
            } catch {
                if success == false {
                    print("Error when reading AVAudioFile")
                }
            }

            var nBytes = 0
            if success {
                nBytes = Int(buffer!.frameLength * bytesPerFrame)
                var range: NSRange = NSRange()
                range.location = 0
                range.length = nBytes
                data.replaceBytes(in: range, withBytes: (buffer?.int16ChannelData![0])!)
            }
            return nBytes
        }) {}!

        self.audioConfig = SPXAudioConfiguration(streamInput: stream)
    }

    func transcribeFromFile() {
        
        print("starting transcription...")
        
        try! self.transcriber?.startTranscribingAsync({ (started, err) in
            print("transcription started")
        })
        
        waitForOperationCompletion(message:"transcription completed")
        
        try! self.transcriber?.stopTranscribingAsync({ (started, err) in
            print("transcription stopped")
        })
    }
    
    func waitForOperationCompletion(message: String) {
        while !self.operationCompleted {
            Thread.sleep(forTimeInterval: 1.0)
        }
        print(message)
        self.operationCompleted = false
    }
    
    func updateLabel(text: String?, color: UIColor) {
        DispatchQueue.main.async {
            self.label.text = text
            self.label.textColor = color
        }
    }
}
