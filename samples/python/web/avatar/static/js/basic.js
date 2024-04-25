// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

// Global objects
var clientId
var peerConnection
var previousAnimationFrameTimestamp = 0;

// Logger
const log = msg => {
    document.getElementById('logging').innerHTML += msg + '<br>'
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

        const mediaPlayer = document.createElement(event.track.kind)
        mediaPlayer.id = event.track.kind
        mediaPlayer.srcObject = event.streams[0]
        mediaPlayer.autoplay = true
        document.getElementById('remoteVideo').appendChild(mediaPlayer)
        document.getElementById('videoLabel').hidden = true
        document.getElementById('overlayArea').hidden = false

        if (event.track.kind === 'video') {
            mediaPlayer.playsInline = true
            remoteVideoDiv = document.getElementById('remoteVideo')
            canvas = document.getElementById('canvas')
            if (document.getElementById('transparentBackground').checked) {
                remoteVideoDiv.style.width = '0.1px'
                canvas.getContext('2d').clearRect(0, 0, canvas.width, canvas.height)
                canvas.hidden = false
            } else {
                canvas.hidden = true
            }

            mediaPlayer.addEventListener('play', () => {
                if (document.getElementById('transparentBackground').checked) {
                    window.requestAnimationFrame(makeBackgroundTransparent)
                } else {
                    remoteVideoDiv.style.width = mediaPlayer.videoWidth / 2 + 'px'
                }
            })
        }
        else
        {
            // Mute the audio player to make sure it can auto play, will unmute it when speaking
            // Refer to https://developer.mozilla.org/en-US/docs/Web/Media/Autoplay_guide
            mediaPlayer.muted = true
        }
    }

    // Make necessary update to the web page when the connection state changes
    peerConnection.oniceconnectionstatechange = e => {
        log("WebRTC status: " + peerConnection.iceConnectionState)

        if (peerConnection.iceConnectionState === 'connected') {
            document.getElementById('stopSession').disabled = false
            document.getElementById('speak').disabled = false
            document.getElementById('configuration').hidden = true
        }

        if (peerConnection.iceConnectionState === 'disconnected' || peerConnection.iceConnectionState === 'failed') {
            document.getElementById('speak').disabled = true
            document.getElementById('stopSpeaking').disabled = true
            document.getElementById('stopSession').disabled = true
            document.getElementById('startSession').disabled = false
            document.getElementById('configuration').hidden = false
        }
    }

    // Offer to receive 1 audio, and 1 video track
    peerConnection.addTransceiver('video', { direction: 'sendrecv' })
    peerConnection.addTransceiver('audio', { direction: 'sendrecv' })

    // Set local description
    peerConnection.createOffer().then(sdp => {
        peerConnection.setLocalDescription(sdp).then(() => { setTimeout(() => { connectToAvatarService(peerConnection) }, 1000) })
    }).catch(log)
}

// Connect to TTS Avatar Service
function connectToAvatarService(peerConnection) {
    let headers = {
        'ClientId': clientId,
        'LocalSdp': btoa(JSON.stringify(peerConnection.localDescription)),
        'AvatarCharacter': document.getElementById('talkingAvatarCharacter').value,
        'AvatarStyle': document.getElementById('talkingAvatarStyle').value,
        'BackgroundColor': document.getElementById('backgroundColor').value,
        'BackgroundImageUrl': document.getElementById('backgroundImageUrl').value,
        'IsCustomAvatar': document.getElementById('customizedAvatar').checked,
        'TransparentBackground': document.getElementById('transparentBackground').checked,
        'VideoCrop': document.getElementById('videoCrop').checked
    }

    if (document.getElementById('customVoiceEndpointId').value !== '') {
        headers['CustomVoiceEndpointId'] = document.getElementById('customVoiceEndpointId').value
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

// Make video background transparent by matting
function makeBackgroundTransparent(timestamp) {
    // Throttle the frame rate to 30 FPS to reduce CPU usage
    if (timestamp - previousAnimationFrameTimestamp > 30) {
        video = document.getElementById('video')
        tmpCanvas = document.getElementById('tmpCanvas')
        tmpCanvasContext = tmpCanvas.getContext('2d', { willReadFrequently: true })
        tmpCanvasContext.drawImage(video, 0, 0, video.videoWidth, video.videoHeight)
        if (video.videoWidth > 0) {
            let frame = tmpCanvasContext.getImageData(0, 0, video.videoWidth, video.videoHeight)
            for (let i = 0; i < frame.data.length / 4; i++) {
                let r = frame.data[i * 4 + 0]
                let g = frame.data[i * 4 + 1]
                let b = frame.data[i * 4 + 2]
                if (g - 150 > r + b) {
                    // Set alpha to 0 for pixels that are close to green
                    frame.data[i * 4 + 3] = 0
                } else if (g + g > r + b) {
                    // Reduce green part of the green pixels to avoid green edge issue
                    adjustment = (g - (r + b) / 2) / 3
                    r += adjustment
                    g -= adjustment * 2
                    b += adjustment
                    frame.data[i * 4 + 0] = r
                    frame.data[i * 4 + 1] = g
                    frame.data[i * 4 + 2] = b
                    // Reduce alpha part for green pixels to make the edge smoother
                    a = Math.max(0, 255 - adjustment * 4)
                    frame.data[i * 4 + 3] = a
                }
            }

            canvas = document.getElementById('canvas')
            canvasContext = canvas.getContext('2d')
            canvasContext.putImageData(frame, 0, 0);
        }

        previousAnimationFrameTimestamp = timestamp
    }

    window.requestAnimationFrame(makeBackgroundTransparent)
}

// Do HTML encoding on given text
function htmlEncode(text) {
    const entityMap = {
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#39;',
        '/': '&#x2F;'
    };

    return String(text).replace(/[&<>"'\/]/g, (match) => entityMap[match])
}

window.onload = () => {
    clientId = document.getElementById('clientId').value
}

window.startSession = () => {
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
}

window.speak = () => {
    document.getElementById('speak').disabled = true;
    document.getElementById('stopSpeaking').disabled = false
    document.getElementById('audio').muted = false
    let spokenText = document.getElementById('spokenText').value
    let ttsVoice = document.getElementById('ttsVoice').value
    let personalVoiceSpeakerProfileID = document.getElementById('personalVoiceSpeakerProfileID').value
    let spokenSsml = `<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'><voice name='${ttsVoice}'><mstts:ttsembedding speakerProfileId='${personalVoiceSpeakerProfileID}'><mstts:leadingsilence-exact value='0'/>${htmlEncode(spokenText)}</mstts:ttsembedding></voice></speak>`
    console.log("[" + (new Date()).toISOString() + "] Speak request sent.")

    fetch('/api/speak', {
        method: 'POST',
        headers: {
            'ClientId': clientId,
            'Content-Type': 'application/ssml+xml'
        },
        body: spokenSsml
    })
    .then(response => {
        document.getElementById('speak').disabled = false
        document.getElementById('stopSpeaking').disabled = true
        if (response.ok) {
            response.text().then(text => {
                console.log(`[${new Date().toISOString()}] Speech synthesized to speaker for text [ ${spokenText} ]. Result ID: ${text}`)
            })
        } else {
            throw new Error(`[${new Date().toISOString()}] Unable to speak text. ${response.status} ${response.statusText}`)
        }
    })
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
            console.log(`[${new Date().toISOString()}] Speaking stopped.`)
            document.getElementById('speak').disabled = false
            document.getElementById('stopSpeaking').disabled = false
        } else {
            throw new Error(`[${new Date().toISOString()}] Unable to stop speaking. ${response.status} ${response.statusText}`)
        }
    })
}

window.stopSession = () => {
    document.getElementById('speak').disabled = true
    document.getElementById('stopSpeaking').disabled = true
    document.getElementById('stopSession').disabled = true

    fetch('/api/disconnectAvatar', {
        method: 'POST',
        headers: {
            'ClientId': clientId
        },
        body: ''
    })
}

window.updataTransparentBackground = () => {
    if (document.getElementById('transparentBackground').checked) {
        document.body.background = './static/image/background.png'
        document.getElementById('backgroundColor').value = '#00FF00FF'
        document.getElementById('backgroundColor').disabled = true
        document.getElementById('backgroundImageUrl').value = ''
        document.getElementById('backgroundImageUrl').disabled = true
    } else {
        document.body.background = ''
        document.getElementById('backgroundColor').value = '#FFFFFFFF'
        document.getElementById('backgroundColor').disabled = false
        document.getElementById('backgroundImageUrl').disabled = false
    }
}
