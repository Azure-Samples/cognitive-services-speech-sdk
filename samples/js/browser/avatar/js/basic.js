// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

// Global objects
var avatarSynthesizer
var peerConnection
var useTcpForWebRTC = false
var previousAnimationFrameTimestamp = 0;

// Global array to store captured speech
var capturedSpeechHistory = [{
      "role": "assistant",
      "content": "# [North Carolina Department of Administration](https://www.doa.nc.gov/about)\n\nThe North Carolina Department of Administration (DOA) acts as the business manager for the state government. Established in 1957, the department oversees various government operations, including:\n\n- Building construction\n- Purchasing and contracting for goods and services\n- Maintaining facilities\n- Managing state vehicles\n- Acquiring and disposing of real property\n- Operating auxiliary services such as courier mail delivery and the sale of state and federal surplus property.\n\nAdditionally, the DOA manages numerous advocacy programs that provide assistance and services to traditionally underserved segments of the state's population.\n\n## Mission\nEnhance the lives of North Carolinians by providing foundational support to state government through asset management, advocacy, and operations.\n\n## Vision\nProvide high-quality customer service effectively, efficiently, and economically for the people, agencies, and communities of our state."
    }];

// Logger
const log = msg => {
    document.getElementById('logging').innerHTML += msg + '<br>'
}

// Setup WebRTC
function setupWebRTC(iceServerUrl, iceServerUsername, iceServerCredential) {
    // Create WebRTC peer connection
    peerConnection = new RTCPeerConnection({
        iceServers: [{
            urls: [ useTcpForWebRTC ? iceServerUrl.replace(':3478', ':443?transport=tcp') : iceServerUrl ],
            username: iceServerUsername,
            credential: iceServerCredential
        }],
        iceTransportPolicy: useTcpForWebRTC ? 'relay' : 'all'
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
            canvas.hidden = true

            mediaPlayer.addEventListener('play', () => {
                remoteVideoDiv.style.width = mediaPlayer.videoWidth / 2 + 'px'
            })
        }
        else
        {
            // Mute the audio player to make sure it can auto play, will unmute it when speaking
            // Refer to https://developer.mozilla.org/en-US/docs/Web/Media/Autoplay_guide
            mediaPlayer.muted = true
        }
    }

    // Listen to data channel, to get the event from the server
    peerConnection.addEventListener("datachannel", event => {
        const dataChannel = event.channel
        dataChannel.onmessage = e => {
            const webRTCEvent = JSON.parse(e.data)
            console.log("[" + (new Date()).toISOString() + "] WebRTC event received: " + e.data)
        }
    })

    // This is a workaround to make sure the data channel listening is working by creating a data channel from the client side
    c = peerConnection.createDataChannel("eventChannel")

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

    // start avatar, establish WebRTC connection
    avatarSynthesizer.startAvatarAsync(peerConnection).then((r) => {
        if (r.reason === SpeechSDK.ResultReason.SynthesizingAudioCompleted) {
            console.log("[" + (new Date()).toISOString() + "] Avatar started. Result ID: " + r.resultId)
        } else {
            console.log("[" + (new Date()).toISOString() + "] Unable to start avatar. Result ID: " + r.resultId)
            if (r.reason === SpeechSDK.ResultReason.Canceled) {
                let cancellationDetails = SpeechSDK.CancellationDetails.fromResult(r)
                if (cancellationDetails.reason === SpeechSDK.CancellationReason.Error) {
                    console.log(cancellationDetails.errorDetails)
                };
                log("Unable to start avatar: " + cancellationDetails.errorDetails);
            }
            document.getElementById('startSession').disabled = false;
            document.getElementById('configuration').hidden = false;
        }
    }).catch(
        (error) => {
            console.log("[" + (new Date()).toISOString() + "] Avatar failed to start. Error: " + error)
            document.getElementById('startSession').disabled = false
            document.getElementById('configuration').hidden = false
        }
    );
}

// // Make video background transparent by matting
// function makeBackgroundTransparent(timestamp) {
//     // Throttle the frame rate to 30 FPS to reduce CPU usage
//     if (timestamp - previousAnimationFrameTimestamp > 30) {
//         video = document.getElementById('video')
//         tmpCanvas = document.getElementById('tmpCanvas')
//         tmpCanvasContext = tmpCanvas.getContext('2d', { willReadFrequently: true })
//         tmpCanvasContext.drawImage(video, 0, 0, video.videoWidth, video.videoHeight)
//         if (video.videoWidth > 0) {
//             let frame = tmpCanvasContext.getImageData(0, 0, video.videoWidth, video.videoHeight)
//             for (let i = 0; i < frame.data.length / 4; i++) {
//                 let r = frame.data[i * 4 + 0]
//                 let g = frame.data[i * 4 + 1]
//                 let b = frame.data[i * 4 + 2]
//                 if (g - 150 > r + b) {
//                     // Set alpha to 0 for pixels that are close to green
//                     frame.data[i * 4 + 3] = 0
//                 } else if (g + g > r + b) {
//                     // Reduce green part of the green pixels to avoid green edge issue
//                     adjustment = (g - (r + b) / 2) / 3
//                     r += adjustment
//                     g -= adjustment * 2
//                     b += adjustment
//                     frame.data[i * 4 + 0] = r
//                     frame.data[i * 4 + 1] = g
//                     frame.data[i * 4 + 2] = b
//                     // Reduce alpha part for green pixels to make the edge smoother
//                     a = Math.max(0, 255 - adjustment * 4)
//                     frame.data[i * 4 + 3] = a
//                 }
//             }

//             canvas = document.getElementById('canvas')
//             canvasContext = canvas.getContext('2d')
//             canvasContext.putImageData(frame, 0, 0);
//         }

//         previousAnimationFrameTimestamp = timestamp
//     }

//     window.requestAnimationFrame(makeBackgroundTransparent)
// }

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

window.startSession = () => {
    const cogSvcRegion = "eastus2"
    const cogSvcSubKey = "5W8uUL2UFEjbAzO2N9xUZCdjH7nRSfpYEAhDnBpGIBAknNgS4NqGJQQJ99BFACHYHv6XJ3w3AAAYACOGBr3N"
    if (cogSvcSubKey === '') {
        alert('Please fill in the API key of your speech resource.')
        return
    }

    let speechSynthesisConfig
    speechSynthesisConfig = SpeechSDK.SpeechConfig.fromSubscription(cogSvcSubKey, cogSvcRegion)
    speechSynthesisConfig.endpointId = ""

    const videoFormat = new SpeechSDK.AvatarVideoFormat()
    let videoCropTopLeftX = 0
    let videoCropBottomRightX = 1920
    videoFormat.setCropRange(new SpeechSDK.Coordinate(videoCropTopLeftX, 0), new SpeechSDK.Coordinate(videoCropBottomRightX, 1080));

    const talkingAvatarCharacter = "jeff"
    const talkingAvatarStyle = "business"
    const avatarConfig = new SpeechSDK.AvatarConfig(talkingAvatarCharacter, talkingAvatarStyle, videoFormat)
    avatarConfig.backgroundImage = "https://www.tclf.org/sites/default/files/styles/full_width/public/thumbnails/image/NCStateLegislativeBldg_15_CharlesBirnbaum_2007.jpg?itok=K-AERRj0"

    document.getElementById('startSession').disabled = true
    
    const xhr = new XMLHttpRequest()
    xhr.open("GET", `https://${cogSvcRegion}.tts.speech.microsoft.com/cognitiveservices/avatar/relay/token/v1`)
    xhr.setRequestHeader("Ocp-Apim-Subscription-Key", cogSvcSubKey)
    xhr.addEventListener("readystatechange", function() {
        if (this.readyState === 4) {
            const responseData = JSON.parse(this.responseText)
            const iceServerUrl = responseData.Urls[0]
            const iceServerUsername = responseData.Username
            const iceServerCredential = responseData.Password

            avatarConfig.remoteIceServers = [{
                urls: [ iceServerUrl ],
                username: iceServerUsername,
                credential: iceServerCredential
            }]

            avatarSynthesizer = new SpeechSDK.AvatarSynthesizer(speechSynthesisConfig, avatarConfig)
            avatarSynthesizer.avatarEventReceived = function (s, e) {
                var offsetMessage = ", offset from session start: " + e.offset / 10000 + "ms."
                if (e.offset === 0) {
                    offsetMessage = ""
                }
                console.log("[" + (new Date()).toISOString() + "] Event received: " + e.description + offsetMessage)
            }

            setupWebRTC(iceServerUrl, iceServerUsername, iceServerCredential)
        }
    })
    xhr.send()
    
}

window.speak = () => {
    document.getElementById('speak').disabled = true;
    document.getElementById('stopSpeaking').disabled = false
    document.getElementById('audio').muted = false
    let spokenSsml = `<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'><voice name='${ttsVoice}'><mstts:leadingsilence-exact value='0'/>${htmlEncode(spokenText)}</voice></speak>`
    console.log("[" + (new Date()).toISOString() + "] Speak request sent.")
    avatarSynthesizer.speakSsmlAsync(spokenSsml).then(
        (result) => {
            document.getElementById('speak').disabled = false
            document.getElementById('stopSpeaking').disabled = true
            if (result.reason === SpeechSDK.ResultReason.SynthesizingAudioCompleted) {
                console.log("[" + (new Date()).toISOString() + "] Speech synthesized to speaker for text [ " + spokenText + " ]. Result ID: " + result.resultId)
            } else {
                console.log("[" + (new Date()).toISOString() + "] Unable to speak text. Result ID: " + result.resultId)
                if (result.reason === SpeechSDK.ResultReason.Canceled) {
                    let cancellationDetails = SpeechSDK.CancellationDetails.fromResult(result)
                    console.log(cancellationDetails.reason)
                    if (cancellationDetails.reason === SpeechSDK.CancellationReason.Error) {
                        console.log(cancellationDetails.errorDetails)
                    }
                }
            }
        }).catch(log);
}


function getAvatar(text) {
    document.getElementById('speak').disabled = true;
    document.getElementById('stopSpeaking').disabled = false
    document.getElementById('audio').muted = false
    // let spokenText = document.getElementById('spokenText').value
    // let ttsVoice = document.getElementById('ttsVoice').value
    let spokenSsml = `<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'><voice name='en-US-AndrewMultilingualNeural'><mstts:leadingsilence-exact value='0'/>${htmlEncode(text)}</voice></speak>`
    console.log("[" + (new Date()).toISOString() + "] Speak request sent.")
    avatarSynthesizer.speakSsmlAsync(spokenSsml).then(
        (result) => {
            document.getElementById('speak').disabled = false
            document.getElementById('stopSpeaking').disabled = true
            if (result.reason === SpeechSDK.ResultReason.SynthesizingAudioCompleted) {
                console.log("[" + (new Date()).toISOString() + "] Speech synthesized to speaker for text [ " + spokenText + " ]. Result ID: " + result.resultId)
            } else {
                console.log("[" + (new Date()).toISOString() + "] Unable to speak text. Result ID: " + result.resultId)
                if (result.reason === SpeechSDK.ResultReason.Canceled) {
                    let cancellationDetails = SpeechSDK.CancellationDetails.fromResult(result)
                    console.log(cancellationDetails.reason)
                    if (cancellationDetails.reason === SpeechSDK.CancellationReason.Error) {
                        console.log(cancellationDetails.errorDetails)
                    }
                }
            }
        }).catch(log);
}

window.stopSpeaking = () => {
    document.getElementById('stopSpeaking').disabled = true

    avatarSynthesizer.stopSpeakingAsync().then(
        log("[" + (new Date()).toISOString() + "] Stop speaking request sent.")
    ).catch(log);
}

window.stopSession = () => {
    document.getElementById('speak').disabled = true
    document.getElementById('stopSession').disabled = true
    document.getElementById('stopSpeaking').disabled = true
    avatarSynthesizer.close()
    capturedSpeechHistory = [];
}

// --- Microphone Speech Recognition ---
window.addEventListener('DOMContentLoaded', () => {
    // Create microphone button
    const micBtn = document.createElement('button')
    micBtn.id = 'micBtn'
    micBtn.textContent = 'Ask Jeff a question!'
    micBtn.style.marginLeft = '8px'
    const spokenTextInput = document.getElementById('spokenText')
    if (spokenTextInput && spokenTextInput.parentNode) {
        spokenTextInput.parentNode.insertBefore(micBtn, spokenTextInput.nextSibling)
    }

    let recognition
    let recognizing = false
    if ('webkitSpeechRecognition' in window || 'SpeechRecognition' in window) {
        const SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition
        recognition = new SpeechRecognition()
        recognition.lang = 'en-US'
        recognition.interimResults = false
        recognition.maxAlternatives = 1

        recognition.onresult = function(event) {
            const transcript = event.results[0][0].transcript
            spokenTextInput.value = transcript
            // Add to global array and log
            capturedSpeechHistory.push({ "role": "user", "content": transcript })
            console.log("capturedSpeechHistory:", capturedSpeechHistory)

            // --- Send POST request with capturedSpeechHistory ---
            const myHeaders = new Headers();
            myHeaders.append("Content-Type", "application/json");

            const raw = JSON.stringify({
              "messages": capturedSpeechHistory,
              "agency_name": "NC Department of Administration",
              "chatbot_focus": "Non-Public Education",
              "conversation_id": "3bb0ef50-e649-4904-975f-dee0d40d014c",
              "search_source": "cosmos://ncdoa",
              "genai_model": "https://ditaichat.openai.azure.com/openai/deployments/gpt-4o-mini/chat/completions?api-version=2025-01-01-preview",
              "genai_key": "D5Au6zhagzXDrW7k92uAx4AQiQbDUK3QD6D1477QFaxVP6TljErlJQQJ99AKACYeBjFXJ3w3AAABACOGGSuc"
            });

            const requestOptions = {
              method: "POST",
              headers: myHeaders,
              body: raw,
              redirect: "follow"
            };

            fetch("https://zammo-azure.azurewebsites.net/api/aiForChat?code=Ce6g5y2u2lsUcwNp4SEZevq5nNKH6EkWNEy3n-zOS8yGAzFuU13YWg==", requestOptions)
              .then((response) => response.text())
              .then((result) => {
                console.log(result);
                try {
                  const parsed = JSON.parse(result);
                  if (parsed && parsed.content) {
                    getAvatar(parsed.content);
                    capturedSpeechHistory.push({ role: "assistant", content: parsed.content });
                    console.log("capturedSpeechHistory (after assistant):", capturedSpeechHistory);
                  }
                } catch (e) {
                  console.error("Failed to parse response as JSON or missing content field.", e);
                }
              })
              .catch((error) => console.error(error));
            // --- END POST request ---
        }
        recognition.onerror = function(event) {
            log('Microphone error: ' + event.error)
        }
        recognition.onend = function() {
            recognizing = false
            micBtn.textContent = 'Ask Jeff a question!'
        }

        micBtn.onclick = function() {
            if (!recognizing) {
                recognition.start()
                recognizing = true
                micBtn.textContent = '🛑 Stop'
            } else {
                recognition.stop()
                recognizing = false
                micBtn.textContent = 'Ask Jeff a question!'
            }
        }
    } else {
        micBtn.disabled = true
        micBtn.textContent = '🎤 Not supported'
        log('SpeechRecognition API not supported in this browser.')
    }
})
