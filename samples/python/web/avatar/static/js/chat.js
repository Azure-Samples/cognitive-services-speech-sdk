// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

// Global objects
var clientId
var speechRecognizer
var peerConnection
var isSpeaking = false
var sessionActive = false
var lastSpeakTime

// Connect to avatar service
function connectAvatar() {
    document.getElementById('startSession').disabled = true

    fetch('/api/getIceToken', {
        method: 'GET',
    })
    .then(response => {
        if (response.ok) {
            response.json().then(data => {
                const iceServerUrl = data.Urls[0]
                const iceServerUsername = data.Username
                const iceServerCredential = data.Password
                setupWebRTC(iceServerUrl, iceServerUsername, iceServerCredential)
            })
        } else {
            throw new Error(`Failed fetching ICE token: ${response.status} ${response.statusText}`)
        }
    })

    document.getElementById('configuration').hidden = true
}

// Create speech recognizer
function createSpeechRecognizer() {
    fetch('/api/getSpeechToken', {
        method: 'GET',
    })
    .then(response => {
        if (response.ok) {
            const speechRegion = response.headers.get('SpeechRegion')
            response.text().then(text => {
                const speechToken = text
                const speechRecognitionConfig = SpeechSDK.SpeechConfig.fromEndpoint(new URL(`wss://${speechRegion}.stt.speech.microsoft.com/speech/universal/v2`), '')
                speechRecognitionConfig.authorizationToken = speechToken
                speechRecognitionConfig.setProperty(SpeechSDK.PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous")
                var sttLocales = document.getElementById('sttLocales').value.split(',')
                var autoDetectSourceLanguageConfig = SpeechSDK.AutoDetectSourceLanguageConfig.fromLanguages(sttLocales)
                speechRecognizer = SpeechSDK.SpeechRecognizer.FromConfig(speechRecognitionConfig, autoDetectSourceLanguageConfig, SpeechSDK.AudioConfig.fromDefaultMicrophoneInput())
            })
        } else {
            throw new Error(`Failed fetching speech token: ${response.status} ${response.statusText}`)
        }
    })
}

// Disconnect from avatar service
function disconnectAvatar(closeSpeechRecognizer = false) {
    fetch('/api/disconnectAvatar', {
        method: 'POST',
        headers: {
            'ClientId': clientId
        },
        body: ''
    })

    if (speechRecognizer !== undefined) {
        speechRecognizer.stopContinuousRecognitionAsync()
        if (closeSpeechRecognizer) {
            speechRecognizer.close()
        }
    }

    sessionActive = false
}

// Setup WebRTC
function setupWebRTC(iceServerUrl, iceServerUsername, iceServerCredential) {
    // Create WebRTC peer connection
    peerConnection = new RTCPeerConnection({
        iceServers: [{
            urls: [ iceServerUrl ],
            username: iceServerUsername,
            credential: iceServerCredential
        }]
    })

    // Fetch WebRTC video stream and mount it to an HTML video element
    peerConnection.ontrack = function (event) {
        // Clean up existing video element if there is any
        remoteVideoDiv = document.getElementById('remoteVideo')
        for (var i = 0; i < remoteVideoDiv.childNodes.length; i++) {
            if (remoteVideoDiv.childNodes[i].localName === event.track.kind) {
                remoteVideoDiv.removeChild(remoteVideoDiv.childNodes[i])
            }
        }

        if (event.track.kind === 'audio') {
            let audioElement = document.createElement('audio')
            audioElement.id = 'audioPlayer'
            audioElement.srcObject = event.streams[0]
            audioElement.autoplay = true

            audioElement.onplaying = () => {
                console.log(`WebRTC ${event.track.kind} channel connected.`)
            }

            document.getElementById('remoteVideo').appendChild(audioElement)
        }

        if (event.track.kind === 'video') {
            document.getElementById('remoteVideo').style.width = '0.1px'
            if (!document.getElementById('useLocalVideoForIdle').checked) {
                document.getElementById('chatHistory').hidden = true
            }

            let videoElement = document.createElement('video')
            videoElement.id = 'videoPlayer'
            videoElement.srcObject = event.streams[0]
            videoElement.autoplay = true
            videoElement.playsInline = true

            videoElement.onplaying = () => {
                console.log(`WebRTC ${event.track.kind} channel connected.`)
                document.getElementById('microphone').disabled = false
                document.getElementById('stopSession').disabled = false
                document.getElementById('remoteVideo').style.width = '960px'
                document.getElementById('chatHistory').hidden = false
                document.getElementById('showTypeMessage').disabled = false

                if (document.getElementById('useLocalVideoForIdle').checked) {
                    document.getElementById('localVideo').hidden = true
                    if (lastSpeakTime === undefined) {
                        lastSpeakTime = new Date()
                    }
                }

                setTimeout(() => { sessionActive = true }, 5000) // Set session active after 5 seconds
            }

            document.getElementById('remoteVideo').appendChild(videoElement)
        }
    }

    // Make necessary update to the web page when the connection state changes
    peerConnection.oniceconnectionstatechange = e => {
        console.log("WebRTC status: " + peerConnection.iceConnectionState)
        if (peerConnection.iceConnectionState === 'disconnected') {
            if (document.getElementById('useLocalVideoForIdle').checked) {
                document.getElementById('localVideo').hidden = false
                document.getElementById('remoteVideo').style.width = '0.1px'
            }
        }
    }

    // Offer to receive 1 audio, and 1 video track
    peerConnection.addTransceiver('video', { direction: 'sendrecv' })
    peerConnection.addTransceiver('audio', { direction: 'sendrecv' })

    // Set local description
    peerConnection.createOffer().then(sdp => {
        peerConnection.setLocalDescription(sdp).then(() => { setTimeout(() => { connectToAvatarService(peerConnection) }, 1000) })
    })
}

// Connect to TTS Avatar Service
function connectToAvatarService(peerConnection) {
    let headers = {
        'ClientId': clientId,
        'LocalSdp': btoa(JSON.stringify(peerConnection.localDescription)),
        'AvatarCharacter': document.getElementById('talkingAvatarCharacter').value,
        'AvatarStyle': document.getElementById('talkingAvatarStyle').value,
        'IsCustomAvatar': document.getElementById('customizedAvatar').checked
    }

    if (document.getElementById('azureOpenAIDeploymentName').value !== '') {
        headers['AoaiDeploymentName'] = document.getElementById('azureOpenAIDeploymentName').value
    }

    if (document.getElementById('enableOyd').checked && document.getElementById('azureCogSearchIndexName').value !== '') {
        headers['CognitiveSearchIndexName'] = document.getElementById('azureCogSearchIndexName').value
    }

    if (document.getElementById('ttsVoice').value !== '') {
        headers['TtsVoice'] = document.getElementById('ttsVoice').value
    }

    if (document.getElementById('customVoiceEndpointId').value !== '') {
        headers['CustomVoiceEndpointId'] = document.getElementById('customVoiceEndpointId').value
    }

    if (document.getElementById('personalVoiceSpeakerProfileID').value !== '') {
        headers['PersonalVoiceSpeakerProfileId'] = document.getElementById('personalVoiceSpeakerProfileID').value
    }

    fetch('/api/connectAvatar', {
        method: 'POST',
        headers: headers,
        body: ''
    })
    .then(response => {
        if (response.ok) {
            response.text().then(text => {
                const remoteSdp = text
                peerConnection.setRemoteDescription(new RTCSessionDescription(JSON.parse(atob(remoteSdp))))
            })
        } else {
            document.getElementById('startSession').disabled = false;
            document.getElementById('configuration').hidden = false;
            throw new Error(`Failed connecting to the Avatar service: ${response.status} ${response.statusText}`)
        }
    })
}

// Handle user query. Send user query to the chat API and display the response.
function handleUserQuery(userQuery) {
    fetch('/api/chat', {
        method: 'POST',
        headers: {
            'ClientId': clientId,
            'SystemPrompt': document.getElementById('prompt').value,
            'Content-Type': 'text/plain'
        },
        body: userQuery
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`Chat API response status: ${response.status} ${response.statusText}`)
        }

        let chatHistoryTextArea = document.getElementById('chatHistory')
        chatHistoryTextArea.innerHTML += 'Assistant: '

        const reader = response.body.getReader()

        // Function to recursively read chunks from the stream
        function read() {
            return reader.read().then(({ value, done }) => {
                // Check if there is still data to read
                if (done) {
                    // Stream complete
                    return
                }

                // Process the chunk of data (value)
                let chunkString = new TextDecoder().decode(value, { stream: true })

                chatHistoryTextArea.innerHTML += `${chunkString}`
                chatHistoryTextArea.scrollTop = chatHistoryTextArea.scrollHeight

                // Continue reading the next chunk
                return read()
            })
        }

        // Start reading the stream
        return read()
    })
}

// Handle local video. If the user is not speaking for 15 seconds, switch to local video.
function handleLocalVideo() {
    if (lastSpeakTime === undefined) {
        return
    }

    let currentTime = new Date()
    if (currentTime - lastSpeakTime > 15000) {
        if (document.getElementById('useLocalVideoForIdle').checked && sessionActive && !isSpeaking) {
            disconnectAvatar()
            document.getElementById('localVideo').hidden = false
            document.getElementById('remoteVideo').style.width = '0.1px'
            sessionActive = false
        }
    }
}

// Check whether the avatar video stream is hung
function checkHung() {
    // Check whether the avatar video stream is hung, by checking whether the video time is advancing
    let videoElement = document.getElementById('videoPlayer')
    if (videoElement !== null && videoElement !== undefined && sessionActive) {
        let videoTime = videoElement.currentTime
        setTimeout(() => {
            // Check whether the video time is advancing
            if (videoElement.currentTime === videoTime) {
                // Check whether the session is active to avoid duplicatedly triggering reconnect
                if (sessionActive) {
                    sessionActive = false
                    if (document.getElementById('autoReconnectAvatar').checked) {
                        console.log(`[${(new Date()).toISOString()}] The video stream got disconnected, need reconnect.`)
                        connectAvatar()
                        createSpeechRecognizer()
                    }
                }
            }
        }, 5000)
    }
}

// Fetch speaking status from backend.
function checkSpeakingStatus() {
    fetch('/api/getSpeakingStatus', {
        method: 'GET',
        headers: {
            'ClientId': clientId
        }
    })
    .then(response => {
        if (response.ok) {
            response.json().then(data => {
                isSpeaking = data.isSpeaking
                if (data.lastSpeakTime !== null) {
                    lastSpeakTime = new Date(data.lastSpeakTime)
                }

                if (isSpeaking) {
                    document.getElementById('stopSpeaking').disabled = false
                } else {
                    document.getElementById('stopSpeaking').disabled = true
                }

                handleLocalVideo()
            })
        } else {
            throw new Error(`Failed to get speaking status: ${response.status} ${response.statusText}`)
        }
    })
}

window.onload = () => {
    clientId = document.getElementById('clientId').value
    setInterval(() => {
        checkHung()
        checkSpeakingStatus()
    }, 5000) // Check session activity every 5 seconds
}

window.startSession = () => {
    createSpeechRecognizer()
    if (document.getElementById('useLocalVideoForIdle').checked) {
        document.getElementById('startSession').disabled = true
        document.getElementById('configuration').hidden = true
        document.getElementById('microphone').disabled = false
        document.getElementById('stopSession').disabled = false
        document.getElementById('localVideo').hidden = false
        document.getElementById('remoteVideo').style.width = '0.1px'
        document.getElementById('chatHistory').hidden = false
        document.getElementById('showTypeMessage').disabled = false
        return
    }

    connectAvatar()
}

window.stopSpeaking = () => {
    document.getElementById('stopSpeaking').disabled = true

    fetch('/api/stopSpeaking', {
        method: 'POST',
        headers: {
            'ClientId': clientId
        },
        body: ''
    })
    .then(response => {
        if (response.ok) {
            checkSpeakingStatus()
        } else {
            throw new Error(`Failed to stop speaking: ${response.status} ${response.statusText}`)
        }
    })
}

window.stopSession = () => {
    document.getElementById('startSession').disabled = false
    document.getElementById('microphone').disabled = true
    document.getElementById('stopSession').disabled = true
    document.getElementById('configuration').hidden = false
    document.getElementById('chatHistory').hidden = true
    document.getElementById('showTypeMessage').checked = false
    document.getElementById('showTypeMessage').disabled = true
    document.getElementById('userMessageBox').hidden = true
    if (document.getElementById('useLocalVideoForIdle').checked) {
        document.getElementById('localVideo').hidden = true
    }

    disconnectAvatar(true)
}

window.clearChatHistory = () => {
    fetch('/api/chat/clearHistory', {
        method: 'POST',
        headers: {
            'ClientId': clientId,
            'SystemPrompt': document.getElementById('prompt').value
        },
        body: ''
    })
    .then(response => {
        if (response.ok) {
            document.getElementById('chatHistory').innerHTML = ''
        } else {
            throw new Error(`Failed to clear chat history: ${response.status} ${response.statusText}`)
        }
    })
}

window.microphone = () => {
    if (document.getElementById('microphone').innerHTML === 'Stop Microphone') {
        // Stop microphone
        document.getElementById('microphone').disabled = true
        speechRecognizer.stopContinuousRecognitionAsync(
            () => {
                document.getElementById('microphone').innerHTML = 'Start Microphone'
                document.getElementById('microphone').disabled = false
            }, (err) => {
                console.log("Failed to stop continuous recognition:", err)
                document.getElementById('microphone').disabled = false
            })

        return
    }

    if (document.getElementById('useLocalVideoForIdle').checked) {
        if (!sessionActive) {
            connectAvatar()
        }

        setTimeout(() => {
            document.getElementById('audioPlayer').play()
        }, 5000)
    } else {
        document.getElementById('audioPlayer').play()
    }

    document.getElementById('microphone').disabled = true
    speechRecognizer.recognized = async (s, e) => {
        if (e.result.reason === SpeechSDK.ResultReason.RecognizedSpeech) {
            let userQuery = e.result.text.trim()
            if (userQuery === '') {
                return
            }

            // Auto stop microphone when a phrase is recognized, when it's not continuous conversation mode
            if (!document.getElementById('continuousConversation').checked) {
                document.getElementById('microphone').disabled = true
                speechRecognizer.stopContinuousRecognitionAsync(
                    () => {
                        document.getElementById('microphone').innerHTML = 'Start Microphone'
                        document.getElementById('microphone').disabled = false
                    }, (err) => {
                        console.log("Failed to stop continuous recognition:", err)
                        document.getElementById('microphone').disabled = false
                    })
            }

            let chatHistoryTextArea = document.getElementById('chatHistory')
            if (chatHistoryTextArea.innerHTML !== '' && !chatHistoryTextArea.innerHTML.endsWith('\n\n')) {
                chatHistoryTextArea.innerHTML += '\n\n'
            }

            chatHistoryTextArea.innerHTML += "User: " + userQuery + '\n\n'
            chatHistoryTextArea.scrollTop = chatHistoryTextArea.scrollHeight

            handleUserQuery(userQuery)
        }
    }

    speechRecognizer.startContinuousRecognitionAsync(
        () => {
            document.getElementById('microphone').innerHTML = 'Stop Microphone'
            document.getElementById('microphone').disabled = false
        }, (err) => {
            console.log("Failed to start continuous recognition:", err)
            document.getElementById('microphone').disabled = false
        })
}

window.updataEnableOyd = () => {
    if (document.getElementById('enableOyd').checked) {
        document.getElementById('cogSearchConfig').hidden = false
    } else {
        document.getElementById('cogSearchConfig').hidden = true
    }
}

window.updateTypeMessageBox = () => {
    if (document.getElementById('showTypeMessage').checked) {
        document.getElementById('userMessageBox').hidden = false
        document.getElementById('userMessageBox').addEventListener('keyup', (e) => {
            if (e.key === 'Enter') {
                const userQuery = document.getElementById('userMessageBox').value
                if (userQuery !== '') {
                    let chatHistoryTextArea = document.getElementById('chatHistory')
                    if (chatHistoryTextArea.innerHTML !== '' && !chatHistoryTextArea.innerHTML.endsWith('\n\n')) {
                        chatHistoryTextArea.innerHTML += '\n\n'
                    }

                    chatHistoryTextArea.innerHTML += "User: " + userQuery.trim('\n') + '\n\n'
                    chatHistoryTextArea.scrollTop = chatHistoryTextArea.scrollHeight

                    handleUserQuery(userQuery.trim('\n'))
                    document.getElementById('userMessageBox').value = ''
                }
            }
        })
    } else {
        document.getElementById('userMessageBox').hidden = true
    }
}

window.updateLocalVideoForIdle = () => {
    if (document.getElementById('useLocalVideoForIdle').checked) {
        document.getElementById('showTypeMessageCheckbox').hidden = true
    } else {
        document.getElementById('showTypeMessageCheckbox').hidden = false
    }
}
