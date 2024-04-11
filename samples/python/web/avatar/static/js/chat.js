// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

// Global objects
var speechRecognizer
var peerConnection
var isSpeaking = false
var sessionActive = false
var lastSpeakTime

// Connect to avatar service
function connectAvatar() {
    document.getElementById('startSession').disabled = true

    const xhr = new XMLHttpRequest()
    xhr.open("GET", "/api/getIceToken")

    let responseReceived = false
    xhr.addEventListener("readystatechange", function() {
        if (xhr.status == 200) {
            if (xhr.responseText !== '') {
                if (!responseReceived) {
                    responseReceived = true
                    const responseData = JSON.parse(this.responseText)
                    const iceServerUrl = responseData.Urls[0]
                    const iceServerUsername = responseData.Username
                    const iceServerCredential = responseData.Password
                    setupWebRTC(iceServerUrl, iceServerUsername, iceServerCredential)
                }
            }
        } else {
            if (xhr.responseText !== '') {
                if (!responseReceived) {
                    responseReceived = true
                    console.log("[" + (new Date()).toISOString() + "] Failed fetching ICE token. " + xhr.responseText)
                }
            }
        }
    })
    xhr.send()

    document.getElementById('configuration').hidden = true
}

// Create speech recognizer
function createSpeechRecognizer() {
    const xhr = new XMLHttpRequest()
    xhr.open("GET", "/api/getSpeechToken")

    let responseReceived = false
    xhr.addEventListener("readystatechange", function() {
        if (xhr.status == 200) {
            if (xhr.responseText !== '') {
                if (!responseReceived) {
                    responseReceived = true
                    const speechRegion = xhr.getResponseHeader('SpeechRegion')
                    const speechToken = this.responseText
                    const speechRecognitionConfig = SpeechSDK.SpeechConfig.fromEndpoint(new URL(`wss://${speechRegion}.stt.speech.microsoft.com/speech/universal/v2`), '')
                    speechRecognitionConfig.authorizationToken = speechToken
                    speechRecognitionConfig.setProperty(SpeechSDK.PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous")
                    var sttLocales = document.getElementById('sttLocales').value.split(',')
                    var autoDetectSourceLanguageConfig = SpeechSDK.AutoDetectSourceLanguageConfig.fromLanguages(sttLocales)
                    speechRecognizer = SpeechSDK.SpeechRecognizer.FromConfig(speechRecognitionConfig, autoDetectSourceLanguageConfig, SpeechSDK.AudioConfig.fromDefaultMicrophoneInput())
                }
            }
        } else {
            if (xhr.responseText !== '') {
                if (!responseReceived) {
                    responseReceived = true
                    console.log("[" + (new Date()).toISOString() + "] Failed fetching speech token. " + xhr.responseText)
                }
            }
        }
    })
    xhr.send()
}

// Disconnect from avatar service
function disconnectAvatar() {
    const xhr = new XMLHttpRequest()
    xhr.open("POST", "/api/disconnectAvatar")
    xhr.send()

    if (speechRecognizer !== undefined) {
        speechRecognizer.stopContinuousRecognitionAsync()
        speechRecognizer.close()
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
    const xhr = new XMLHttpRequest()
    xhr.open("POST", "/api/connectAvatar")

    if (document.getElementById('customVoiceEndpointId').value !== '') {
        xhr.setRequestHeader("CustomVoiceEndpointId", document.getElementById('customVoiceEndpointId').value)
    }

    xhr.setRequestHeader("LocalSdp", btoa(JSON.stringify(peerConnection.localDescription)))
    xhr.setRequestHeader("AvatarCharacter", document.getElementById('talkingAvatarCharacter').value)
    xhr.setRequestHeader("AvatarStyle", document.getElementById('talkingAvatarStyle').value)
    xhr.setRequestHeader("IsCustomAvatar", document.getElementById('customizedAvatar').checked)

    let responseReceived = false
    xhr.addEventListener("readystatechange", function() {
        if (xhr.status == 200) {
            if (xhr.responseText !== '') {
                if (!responseReceived) {
                    responseReceived = true
                    const remoteSdp = this.responseText
                    peerConnection.setRemoteDescription(new RTCSessionDescription(JSON.parse(atob(remoteSdp))))
                }
            }
        } else {
            if (xhr.responseText !== '') {
                if (!responseReceived) {
                    responseReceived = true
                    console.log("Failed to connect to the Avatar service. " + xhr.responseText)

                    document.getElementById('startSession').disabled = false;
                    document.getElementById('configuration').hidden = false;
                }
            }
        }
    })
    xhr.send()
}

function handleUserQuery(userQuery) {
    fetch('/api/chat', {
        method: 'POST',
        headers: {
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

function stopSpeaking() {
    // To be implemented
}

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
                    }
                }
            }
        }, 5000)
    }
}

function checkLastSpeak() {
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

window.onload = () => {
    setInterval(() => {
        checkHung()
        checkLastSpeak()
    }, 5000) // Check session activity every 5 seconds
}

window.startSession = () => {
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
    createSpeechRecognizer()
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

    disconnectAvatar()
}

window.clearChatHistory = () => {
    document.getElementById('chatHistory').innerHTML = ''
    // Call backend API to clear the chat history
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

                    chatHistoryTextArea.innerHTML += "User: " + userQuery + '\n\n'
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
