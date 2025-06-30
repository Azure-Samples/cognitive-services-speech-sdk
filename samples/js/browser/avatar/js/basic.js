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
            if (document.getElementById('configuration')) {
                document.getElementById('configuration').hidden = true
            }
        }

        if (peerConnection.iceConnectionState === 'disconnected' || peerConnection.iceConnectionState === 'failed') {
            document.getElementById('speak').disabled = true
            document.getElementById('stopSpeaking').disabled = true
            document.getElementById('stopSession').disabled = true
            document.getElementById('startSession').disabled = false
            if (document.getElementById('configuration')) {
                document.getElementById('configuration').hidden = false
            }
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
            // document.getElementById('configuration').hidden = false;
        }
    }).catch(
        (error) => {
            console.log("[" + (new Date()).toISOString() + "] Avatar failed to start. Error: " + error)
            document.getElementById('startSession').disabled = false
            document.getElementById('configuration').hidden = false
        }
    );
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

function startSession(){
    // Use window.env from config.js for browser-based secrets
    if (!window.env) {
        alert('Environment variables are not loaded. Please ensure config.js is loaded before basic.js.');
        console.warn('window.env is undefined. Check your script order and config.js file.');
        return;
    }
    const cogSvcRegion = window.env.COG_SVC_REGION;
    const cogSvcSubKey = window.env.COG_SVC_SUB_KEY;
    if (!cogSvcSubKey || !cogSvcRegion) {
        alert('Please fill in the API key and region of your speech resource in config.js.');
        return;
    }

    let speechSynthesisConfig;
    speechSynthesisConfig = SpeechSDK.SpeechConfig.fromSubscription(cogSvcSubKey, cogSvcRegion);
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

    // Remove Speak and Stop Speaking buttons if present
    // const speakBtn = document.getElementById('speak');
    // if (speakBtn) speakBtn.remove();
    // const stopSpeakingBtn = document.getElementById('stopSpeaking');
    // if (stopSpeakingBtn) stopSpeakingBtn.remove();

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
              "agency_name": window.env.AGENCY_NAME,
              "chatbot_focus": window.env.CHATBOT_FOCUS,
              "conversation_id": window.env.CONVERSATION_ID,
              "search_source": window.env.SEARCH_SOURCE,
              "genai_model": window.env.GENAI_MODEL,
              "genai_key": window.env.GENAI_KEY
            });

            const requestOptions = {
              method: "POST",
              headers: myHeaders,
              body: raw,
              redirect: "follow"
            };

            fetch(`https://zammo-azure.azurewebsites.net/api/aiForChat?code=${window.env.AZURE_FUNC_API_KEY}`,
                requestOptions)
              .then((response) => response.text())
              .then((result) => {
                console.log(result);
                try {
                  const parsed = JSON.parse(result);
                  if (parsed && parsed.content) {
                    // Jeff automatically responds with the AI answer
                    console.log("Got Avatar with", parsed.content);
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
    startSession();
})
