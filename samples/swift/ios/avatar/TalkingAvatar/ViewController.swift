//
//  ViewController.swift
//  TalkingAvatar1
//
//  Created by test on 7/26/24.
//

import UIKit
import WebRTC

import AVFoundation
import AVKit

import MicrosoftCognitiveServicesSpeech
import Foundation

class ViewController: UIViewController {
    
    // Replace below with your own subscription key
    private let speechSubscriptionKey = ""
    // Replace below with your own service region (e.g., "westus2").
    private let serviceRegion = ""
    
    // Update below values if you want to use a different avatar
    private let avatarCharacter = "lisa"
    private let avatarStyle = "casual-sitting"
    // Set below parameter to true if you want to use custom avatar
    private let customAvatar = false
    
    // Update below value if you want to use a different TTS voice
    private let ttsVoice = "en-US-AvaMultilingualNeural"
    // Fill below value if you want to use custom TTS voice
    private let ttsEndpointID = ""
    
    private let VIDEO_TRACK_ID = "ARDAMSv0"
    private let AUDIO_TRACK_ID = "ARDAMSa0"
    private let AUDIO_CODEC_ISAC = "ISAC"
    private let VIDEO_CODEC_H264 = "H264"
    
    private var iceUrl: String = ""
    private var iceUsername: String = ""
    private var icePassword: String = ""
    
    private var speechConfig: SPXSpeechConfiguration?
    private var synthesizer: SPXSpeechSynthesizer?
    private var connection: SPXConnection?
    private var iceCandidateCount: Int = 0
    
    private static let peerConnectionFactory: RTCPeerConnectionFactory = {
        RTCInitializeSSL()
        let videoDecoderFactory = RTCDefaultVideoDecoderFactory()
        let videoEncoderFactory = RTCDefaultVideoEncoderFactory()
        
        return RTCPeerConnectionFactory(
            encoderFactory: videoEncoderFactory,
            decoderFactory: videoDecoderFactory)
    }()
    private var peerConnection: RTCPeerConnection?
    private var audioTrack: RTCAudioTrack?
    private var videoTrack: RTCVideoTrack!
    private var rtcRender: RTCMTLVideoView!
    
    @IBOutlet weak var startSessionButton: UIButton!
    @IBOutlet weak var stopSessionButton: UIButton!
    @IBOutlet weak var speakButton: UIButton!
    @IBOutlet weak var stopSpeakingButton: UIButton!
    @IBOutlet weak var inputTextField: UITextField!
    @IBOutlet weak var outputMessage: UITextView!
    @IBOutlet weak var rtcVideoView: UIView!
    

    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.backgroundColor = UIColor.white
        
        requestPermission()
                
        setButtonAvailability(stopSessionButton, false)
        setButtonAvailability(speakButton, false)
        setButtonAvailability(stopSpeakingButton, false)
        
        rtcRender = RTCMTLVideoView(frame: self.rtcVideoView?.frame ?? CGRect.zero)
        rtcRender.contentMode = .scaleAspectFill
//        rtcRender.backgroundColor = UIColor.yellow

        add(rtcRender, container: rtcVideoView)
        
        // Initialize PeerConnectionFactory
        initializePeerConnectionFactory()
        
    }
    
    private func add(_ view: UIView, container: UIView) {
        view.translatesAutoresizingMaskIntoConstraints = false
        container.addSubview(view)
        NSLayoutConstraint.activate([
            view.topAnchor.constraint(equalTo: container.topAnchor),
            view.trailingAnchor.constraint(equalTo: container.trailingAnchor),
            view.bottomAnchor.constraint(equalTo: container.bottomAnchor),
            view.leadingAnchor.constraint(equalTo: container.leadingAnchor),
        ])
    }

    @IBAction func startSessionTapped(_ sender: UIButton){
        DispatchQueue.main.async {
            self.setVideoRendererVisibility(true)
            self.setButtonAvailability(self.startSessionButton, false)
            self.startSession()
        }
    }
    
    @IBAction func stopSessionTapped(_ sender: UIButton){
        DispatchQueue.main.async {
            self.setButtonAvailability(self.stopSessionButton, false)
            self.setButtonAvailability(self.speakButton, false)
            self.stopSession()
        }
    }
    
    @IBAction func speakTapped(_ sender: UIButton){
        DispatchQueue.main.async {
            self.speak()
        }
    }
    
    @IBAction func stopSpeakingTapped(_ sender: UIButton){
        DispatchQueue.main.async {
            self.setButtonAvailability(self.stopSpeakingButton, false)
            self.stopSpeaking()
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)

        // Release speech synthesizer and its dependencies
        self.synthesizer = nil
        connection?.close()

        peerConnection?.close()
    }
    
    

    private func initializePeerConnectionFactory() {

        // Create audio track
        let audioConstraints = RTCMediaConstraints(mandatoryConstraints: [
            "googEchoCancellation": "true",
            "googAutoGainControl": "true",
            "googHighpassFilter": "true",
            "googNoiseSuppression": "true"
        ], optionalConstraints: nil)
        
        let audioSource = ViewController.peerConnectionFactory.audioSource(with: audioConstraints)
        self.audioTrack = ViewController.peerConnectionFactory.audioTrack(with: audioSource, trackId: AUDIO_TRACK_ID)

        // Create video track
        let videoSource = ViewController.peerConnectionFactory.videoSource()
        self.videoTrack = ViewController.peerConnectionFactory.videoTrack(with: videoSource, trackId: VIDEO_TRACK_ID)
    }
    
    private func startSession() {
        setButtonAvailability(startSessionButton, false)
        
        fetchIceToken()
    }

    private func speak() {
        guard let synthesizer = synthesizer else {
            updateOutputMessage("Please start the avatar session first", isError: true, shouldAppend: true)
            return
        }

        // Assuming you have an IBOutlet for the inputText field
        guard let inputText = inputTextField.text else {
            updateOutputMessage("Input text field is empty", isError: true, shouldAppend: true)
            return
        }
        
        _ = try? synthesizer.startSpeakingText(inputText)
    }
    
    private func stopSpeaking(){
        try? connection?.sendMessage("synthesis.control", payload: "{\"action\": \"stop\"}")
    }
    
    private func stopSession() {
        print("Stopping session...")
        
        iceCandidateCount = 0
        guard synthesizer != nil || peerConnection != nil else {
            updateOutputMessage("Session not started yet", isError: true, shouldAppend: true)
            return
        }

        if let synthesizer = synthesizer {
            do {
                try synthesizer.stopSpeaking()
                print("Speech synthesizer stopped.")
                DispatchQueue.main.asyncAfter(deadline: .now() + 1) {
                    self.synthesizer = nil
                }
            } catch {
                updateOutputMessage("Error stopping speech synthesizer: \(error)", isError: true, shouldAppend: true)
                return
            }
        }

        if let peerConnection = peerConnection {
            print("Closing peer connection...")
            rtcRender.renderFrame(nil)
            peerConnection.close()
            self.peerConnection = nil
            print("Peer connection closed.")
        }

        if let connection = self.connection {
            print("Closing connection...")
            connection.close()
            self.connection = nil
        }
        
        self.iceUrl = ""
        self.iceUsername = ""
        self.icePassword = ""

        updateOutputMessage("Session stopped successfully.", isError: false, shouldAppend: true)
        print("Session stopped.")
        
        DispatchQueue.main.asyncAfter(deadline: .now() + 2) {
            self.setButtonAvailability(self.startSessionButton, true)
        }
       
    }

    private func setButtonAvailability(_ button: UIButton, _ isEnabled: Bool) {
        DispatchQueue.main.async {
            button.isEnabled = isEnabled
        }
    }

    private func updateOutputMessage(_ text: String, isError: Bool, shouldAppend: Bool) {
        DispatchQueue.main.async {
            if shouldAppend {
                if !self.outputMessage.text!.isEmpty {
                    self.outputMessage.text!.append("\n")
                }
                self.outputMessage.text!.append(text)
            } else {
                self.outputMessage.text = text
            }

            if isError {
                let attributedString = NSMutableAttributedString(string: self.outputMessage.text!)
                let range = NSRange(location: self.outputMessage.text!.count - text.count, length: text.count)
                attributedString.addAttribute(.foregroundColor, value: UIColor.red, range: range)
                self.outputMessage.attributedText = attributedString
            }

            // Scroll to bottom to show latest message
            let bottom = NSRange(location: self.outputMessage.text!.count - 1, length: 1)
            self.outputMessage.scrollRangeToVisible(bottom)
        }
    }

    private func fetchIceToken() {
        DispatchQueue.global().async { [weak self] in
            guard let self = self else { return }
            
            if self.iceUrl.isEmpty {
                DispatchQueue.main.async {
                    self.updateOutputMessage("Fetching ICE token ...", isError: false, shouldAppend: true)
                }
                
                let endpoint = "https://\(self.serviceRegion).tts.speech.microsoft.com"
                guard let url = URL(string: "\(endpoint)/cognitiveservices/avatar/relay/token/v1") else { return }
                
                var request = URLRequest(url: url)
                request.httpMethod = "GET"
                request.addValue(self.speechSubscriptionKey, forHTTPHeaderField: "Ocp-Apim-Subscription-Key")
                
                let task = URLSession.shared.dataTask(with: request) { data, response, error in
                    if let error = error {
                        print("[ICE Token] Error: \(error)")
                        return
                    }
                    
                    guard let data = data else {
                        print("[ICE Token] No data received")
                        return
                    }
                    
                    do {
                        if let responseDict = try JSONSerialization.jsonObject(with: data, options: []) as? [String: Any],
                           let urls = responseDict["Urls"] as? [String],
                           let username = responseDict["Username"] as? String,
                           let password = responseDict["Password"] as? String {
                            
                            self.iceUrl = urls.first!
                            self.iceUsername = username
                            self.icePassword = password
                            
                            DispatchQueue.main.async {
                                self.updateOutputMessage("ICE token successfully fetched", isError: false, shouldAppend: true)
                                self.setupWebRTC()
                            }
                        }
                    } catch {
                        print("[ICE Token] JSON parsing error: \(error)")
                    }
                }
                task.resume()
            } else {
                DispatchQueue.main.async {
                    self.setupWebRTC()
                }
            }
        }
    }
    
    private func connectAvatar(localSDP: String) {
        let endpoint = "wss://\(serviceRegion).tts.speech.microsoft.com"
        let uriString = "\(endpoint)/cognitiveservices/websocket/v1?enableTalkingAvatar=true"
        
        // Initialize speech config and synthesizer
        guard let config = try? SPXSpeechConfiguration(endpoint: uriString, subscription: speechSubscriptionKey) else {
            print("[SpeechConfig] Initialization failed")
            return
        }
        self.speechConfig = config
    
        speechConfig?.speechSynthesisVoiceName = ttsVoice
        
        if !ttsEndpointID.isEmpty {
            speechConfig!.endpointId = ttsEndpointID
        }

        synthesizer = try? SPXSpeechSynthesizer(speechConfiguration: speechConfig!, audioConfiguration: nil)

        connection = try? SPXConnection(from: synthesizer!)
        
        synthesizer?.addSynthesisCanceledEventHandler{ event, arg  in
            let cancellationDetails = SPXSpeechSynthesisCancellationDetails.description()
            self.updateOutputMessage(cancellationDetails, isError: true, shouldAppend: true)
            print("[Synthesizer] \(cancellationDetails)")
        }
        
        synthesizer?.addSynthesisStartedEventHandler { [weak self] event, arg  in
            self?.setButtonAvailability((self?.stopSessionButton)!, false)
            self?.setButtonAvailability((self?.speakButton)!, false)
            self?.setButtonAvailability((self?.stopSpeakingButton)!, true)
        }
        
        synthesizer?.addSynthesisCompletedEventHandler { [weak self] event,  arg  in
            self?.setButtonAvailability((self?.stopSessionButton)!, true)
            self?.setButtonAvailability((self?.speakButton)!, true)
            self?.setButtonAvailability((self?.stopSpeakingButton)!, false)
        }

        let avatarConfig = self.buildAvatarConfig(localSDP: localSDP)
        try? connection!.setMessageProperty("speech.config", propertyName: "context", propertyValue: avatarConfig)
        

        _ = try? synthesizer!.speakText("")

        if let turnStartMessage = synthesizer?.properties?.getPropertyByName("SpeechSDKInternal-ExtraTurnStartMessage") as? String,
           let turnStartMessageJson = try? JSONSerialization.jsonObject(with: turnStartMessage.data(using: .utf8)!, options: []) as? [String: Any] {
            if let webrtcDict = turnStartMessageJson["webrtc"] as? [String: Any],
               let remoteSdpBase64 = webrtcDict["connectionString"] as? String {
                if let remoteSdpJsonStr = String(data: Data(base64Encoded: remoteSdpBase64)!, encoding: .utf8),
                   let remoteSdpJson = try? JSONSerialization.jsonObject(with: remoteSdpJsonStr.data(using: .utf8)!, options: []) as? [String: Any],
                   let remoteSdp = remoteSdpJson["sdp"] as? String {
                    print("[WebRTC][Remote SDP] Remote SDP: \(remoteSdp)")

                    // Create RTCSessionDescription and set remote description
                    let sdpDescription = RTCSessionDescription(type: .answer, sdp: remoteSdp)
                    if self.peerConnection?.signalingState == .haveLocalOffer || self.peerConnection?.signalingState == .haveRemoteOffer
                    {
                        self.peerConnection!.setRemoteDescription(sdpDescription, completionHandler: { error in
                            if let error = error {
                                print("[WebRTC][Remote SDP] Error: \(error.localizedDescription)")
                            } else {
                                print("[WebRTC][Remote SDP] Remote SDP set successfully.")
                            }
                        })
                    } else
                    {
                        print("Cannot set remote SDP in the current signaling state: \(String(describing: self.peerConnection?.signalingState))")
                    }
                }
            }
        } else {
            print("[WebRTC][Remote SDP] JSON parsing error")
        }
    }
    
    private func setVideoRendererVisibility(_ visible: Bool) {
        DispatchQueue.main.async {
            if visible {
                self.rtcRender.backgroundColor = UIColor.clear // Transparent
            } else {
                self.rtcRender.backgroundColor = UIColor.white // White
            }
        }
    }
    
    private func buildAvatarConfig(localSDP: String) -> String {
        return """
        {
            "synthesis": {
                "video": {
                    "protocol": {
                        "name": "WebRTC",
                        "webrtcConfig": {
                            "clientDescription": "\(localSDP)",
                            "iceServers": [{
                                "urls": ["\(iceUrl)"],
                                "username": "\(iceUsername)",
                                "credential": "\(icePassword)"
                            }]
                        }
                    },
                    "format": {
                        "crop": {
                            "topLeft": {
                                "x": 640,
                                "y": 0
                            },
                            "bottomRight": {
                                "x": 1280,
                                "y": 1080
                            }
                        },
                        "bitrate": 1000000
                    },
                    "talkingAvatar": {
                        "customized": \(customAvatar),
                        "character": "\(avatarCharacter)",
                        "style": "\(avatarStyle)",
                        "background": {
                            "color": "#FFFFFFFF",
                            "image": {
                                "url": ""
                            }
                        }
                    }
                }
            }
        }
        """
    }
    
    private func preferCodec(sdp: String, codec: String, isAudio: Bool) -> String {
        let lines = sdp.split(separator: "\r\n").map(String.init)
        
        guard let mLineIndex = findMediaDescriptionLine(isAudio: isAudio, sdpLines: lines) else {
            print("[WebRTC][Local SDP] No mediaDescription line, so can't prefer \(codec)")
            return sdp
        }
        
        // A list with all the payload types with name `codec`. The payload types are integers in the
        // range 96-127, but they are stored as strings here.
        var codecPayloadTypes: [String] = []
        let codecPattern = try! NSRegularExpression(pattern: "^a=rtpmap:(\\d+) \(codec)(/\\d+)+[\r]?$", options: [])
        for line in lines {
            let nsLine = line as NSString
            let matches = codecPattern.matches(in: line, options: [], range: NSRange(location: 0, length: nsLine.length))
            if let match = matches.first {
                let payloadType = nsLine.substring(with: match.range(at: 1))
                codecPayloadTypes.append(payloadType)
            }
        }
        
        if codecPayloadTypes.isEmpty {
            print("[WebRTC][Local SDP] No payload types with name \(codec)")
            return sdp
        }
        
        guard let newMLine = movePayloadTypesToFront(preferredPayloadTypes: codecPayloadTypes, mLine: lines[mLineIndex]) else {
            return sdp
        }
        
        print("[WebRTC][Local SDP] Change media description from: \(lines[mLineIndex]) to: \(newMLine)")
        
        var modifiedLines = lines
        modifiedLines[mLineIndex] = newMLine
        return modifiedLines.joined(separator: "\r\n") + "\r\n"
    }

    // Returns the index of the line containing "m=audio|video", or -1 if no such line exists.
    private func findMediaDescriptionLine(isAudio: Bool, sdpLines: [String]) -> Int? {
        let mediaDescription = isAudio ? "m=audio " : "m=video "
        for (index, line) in sdpLines.enumerated() {
            if line.hasPrefix(mediaDescription) {
                return index
            }
        }
        return nil
    }

    private func movePayloadTypesToFront(preferredPayloadTypes: [String], mLine: String) -> String? {
        // The format of the media description line should be: m=<media> <port> <proto> <fmt> ...
        let origLineParts = mLine.split(separator: " ").map{String($0)}
        
        guard origLineParts.count > 3 else {
            print("[WebRTC][Local SDP] Wrong SDP media description format: \(mLine)")
            return nil
        }
        
        let header = Array(origLineParts.prefix(3))
        var unpreferredPayloadTypes = Array(origLineParts.dropFirst(3))
        unpreferredPayloadTypes.removeAll { preferredPayloadTypes.contains($0) }
        
        // Reconstruct the line with `preferredPayloadTypes` moved to the beginning of the payload types.
        var newLineParts = header
        newLineParts.append(contentsOf: preferredPayloadTypes)
        newLineParts.append(contentsOf: unpreferredPayloadTypes)
        
        return newLineParts.joined(separator: " ")
    }
    
    private func setupWebRTC() {
        // Create ICE servers
        let iceServer = RTCIceServer(urlStrings: [iceUrl], username: iceUsername, credential: icePassword)
        let rtcConfig = RTCConfiguration()
        rtcConfig.iceServers = [iceServer]
        rtcConfig.continualGatheringPolicy = .gatherContinually
        rtcConfig.iceTransportPolicy = .relay
        rtcConfig.audioJitterBufferMaxPackets = 0
        rtcConfig.sdpSemantics = .unifiedPlan
        
        let constraints1 = RTCMediaConstraints(mandatoryConstraints: nil, optionalConstraints: ["DtlsSrtpKeyAgreement":kRTCMediaConstraintsValueTrue])
        
        // PeerConnection
        peerConnection = ViewController.peerConnectionFactory.peerConnection(with: rtcConfig, constraints: constraints1, delegate: self)
        
        guard peerConnection != nil else {
            print("peerConnection is nil")
            return
        }
        
        guard let audioTrack = audioTrack else {
            print("audioTrack is nil")
            return
        }
        guard let videoTrack = videoTrack else {
            print("videoTrack is nil")
            return
        }

        // Add tracks
        peerConnection!.add(audioTrack, streamIds: ["streamId"])
        peerConnection!.add(videoTrack, streamIds: ["streamId"])
        
        let constraints = RTCMediaConstraints(mandatoryConstraints: [
            kRTCMediaConstraintsOfferToReceiveAudio: kRTCMediaConstraintsValueTrue,
            kRTCMediaConstraintsOfferToReceiveVideo: kRTCMediaConstraintsValueTrue
        ], optionalConstraints: nil)
        
        // Create offer
        peerConnection!.offer(for: constraints) { [weak self] (sessionDescription, error) in
            guard let self = self else { return }
            if let error = error {
                print("[WebRTC][SdpObserver] Offer creation failed: \(error)")
                return
            }
            
            if let sessionDescription = sessionDescription {
                var sdp = sessionDescription.sdp
                sdp = self.preferCodec(sdp: sdp, codec: self.AUDIO_CODEC_ISAC, isAudio: true)
                sdp = self.preferCodec(sdp: sdp, codec: self.VIDEO_CODEC_H264, isAudio: false)
                let newSessionDescription = RTCSessionDescription(type: .offer, sdp: sdp)
                self.peerConnection?.setLocalDescription(newSessionDescription) { error in
                    if let error = error {
                        print("[WebRTC][Sdp] Set local description failed: \(error)")
                    } else {
                        print("[WebRTC][Sdp] Offer creation succeeded: \(sdp)")
                    }
                }
            } else  if let error = error{
                print("Failed to create offer: \(error.localizedDescription)")
            }
        }
    }

    private func requestPermission(){
        AVAudioSession.sharedInstance().requestRecordPermission{ granted in
            if granted {
                print("Micrphone access granted.")
            }
            else {
                print("Microphone access denied.")
            }
        }
        
        AVCaptureDevice.requestAccess(for: .video) { granted in
            if !granted {
                print("Camera permission denied")
            }
            else {
                print("Camera granted")
            }
        }
    }
}

extension ViewController: RTCPeerConnectionDelegate {
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange stateChanged: RTCSignalingState) {
        print("[WebRTC][PeerConnectionDelegate] Signaling state changed: \(stateChanged)")
        updateOutputMessage("Signaling state changed: \(stateChanged)", isError: false, shouldAppend: true)
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange newState: RTCIceConnectionState) {
        print("[WebRTC][PeerConnectionDelegate] WebRTC connection state: \(newState)")
        updateOutputMessage("WebRTC connection state: \(newState)", isError: false, shouldAppend: true)

        if newState == .disconnected {
            setVideoRendererVisibility(false)
            setButtonAvailability(startSessionButton, true)
            setButtonAvailability(stopSessionButton, false)
            setButtonAvailability(speakButton, false)
            setButtonAvailability(stopSpeakingButton, false)
        }
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didGenerate candidate: RTCIceCandidate) {
        print("[WebRTC][PeerConnectionDelegate] ICE candidate: \(candidate)")
        iceCandidateCount += 1
        
        if iceCandidateCount >= 2 {
            // Encode SDP and send to service
            guard let localDescription = peerConnection.localDescription else { return }
            let sdpJsonObj: [String: String] = ["type": "offer", "sdp": localDescription.sdp]
            if let sdpJsonData = try? JSONSerialization.data(withJSONObject: sdpJsonObj, options: []) {
                let sdpBase64Str = sdpJsonData.base64EncodedString()
                connectAvatar(localSDP: sdpBase64Str)
            }
        }
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange iceGatheringState: RTCIceGatheringState) {
        print("[WebRTC][PeerConnectionDelegate] ICE gathering state: \(iceGatheringState)")
        updateOutputMessage("ICE gathering state: \(iceGatheringState.rawValue)", isError: false, shouldAppend: true)
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didAdd stream: RTCMediaStream) {
        self.videoTrack = self.peerConnection!.transceivers.first { $0.mediaType == .video }?.receiver.track as? RTCVideoTrack
        self.videoTrack?.add(rtcRender)
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didRemove stream: RTCMediaStream) {

    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didOpen dataChannel: RTCDataChannel) {
        print("[WebRTC][PeerConnectionDelegate] Data channel opened: \(dataChannel.label)")
        // Handle data channel messages
    }
    
    func peerConnectionShouldNegotiate(_ peerConnection: RTCPeerConnection) {

    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didRemove candidates: [RTCIceCandidate]) {

    }
}


