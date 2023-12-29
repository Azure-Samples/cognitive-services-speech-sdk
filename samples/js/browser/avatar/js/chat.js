// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

// Global objects
var speechRecognizer
var avatarSynthesizer
var peerConnection
var messages = []
var messageInitiated = false
var dataSources = []
var sentenceLevelPunctuations = [ '.', '?', '!', ':', ';', '。', '？', '！', '：', '；' ]
var enableQuickReply = false
var quickReplies = [ 'Let me take a look.', 'Let me check.', 'One moment, please.' ]
var byodDocRegex = new RegExp(/\[doc(\d+)\]/g)
var isSpeaking = false
var spokenTextQueue = []
var sessionActive = false

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
            document.getElementById('chatHistory').hidden = true

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

                setTimeout(() => { sessionActive = true }, 5000) // Set session active after 5 seconds
            }

            document.getElementById('remoteVideo').appendChild(videoElement)
        }
    }

    // Make necessary update to the web page when the connection state changes
    peerConnection.oniceconnectionstatechange = e => {
        console.log("WebRTC status: " + peerConnection.iceConnectionState)
    }

    // Offer to receive 1 audio, and 1 video track
    peerConnection.addTransceiver('video', { direction: 'sendrecv' })
    peerConnection.addTransceiver('audio', { direction: 'sendrecv' })

    // start avatar, establish WebRTC connection
    avatarSynthesizer.startAvatarAsync(peerConnection).then((r) => {
        if (r.reason === SpeechSDK.ResultReason.SynthesizingAudioCompleted) {
            console.log("[" + (new Date()).toISOString() + "] Avatar started.")
        } else {
            console.log("[" + (new Date()).toISOString() + "] Unable to start avatar.")
            if (r.reason === SpeechSDK.ResultReason.Canceled) {
                let cancellationDetails = SpeechSDK.CancellationDetails.fromResult(r)
                if (cancellationDetails.reason === SpeechSDK.CancellationReason.Error) {
                    console.log(cancellationDetails.errorDetails)
                };

                console.log("Unable to start avatar: " + cancellationDetails.errorDetails);
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
    )
}

// Initialize messages
function initMessages() {
    messages = []

    if (dataSources.length === 0) {
        let systemPrompt = document.getElementById('prompt').value
        let systemMessage = {
            role: 'system',
            content: systemPrompt
        }

        messages.push(systemMessage)
    }
}

// Set data sources for chat API
function setDataSources(azureCogSearchEndpoint, azureCogSearchApiKey, azureCogSearchIndexName) {
    let dataSource = {
        type: 'AzureCognitiveSearch',
        parameters: {
            endpoint: azureCogSearchEndpoint,
            key: azureCogSearchApiKey,
            indexName: azureCogSearchIndexName,
            semanticConfiguration: '',
            queryType: 'simple',
            fieldsMapping: {
                contentFieldsSeparator: '\n',
                contentFields: ['content'],
                filepathField: null,
                titleField: 'title',
                urlField: null
            },
            inScope: true,
            roleInformation: document.getElementById('prompt').value
        }
    }

    dataSources.push(dataSource)
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

// Speak the given text
function speak(text, endingSilenceMs = 0) {
    if (isSpeaking) {
        spokenTextQueue.push(text)
        return
    }

    speakNext(text, endingSilenceMs)
}

function speakNext(text, endingSilenceMs = 0) {
    let ttsVoice = document.getElementById('ttsVoice').value
    let ssml = `<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'><voice name='${ttsVoice}'><mstts:leadingsilence-exact value='0'/>${htmlEncode(text)}</voice></speak>`
    if (endingSilenceMs > 0) {
        ssml = `<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'><voice name='${ttsVoice}'><mstts:leadingsilence-exact value='0'/>${htmlEncode(text)}<break time='${endingSilenceMs}ms' /></voice></speak>`
    }

    isSpeaking = true
    document.getElementById('stopSpeaking').disabled = false
    avatarSynthesizer.speakSsmlAsync(ssml).then(
        (result) => {
            if (result.reason === SpeechSDK.ResultReason.SynthesizingAudioCompleted) {
                console.log(`Speech synthesized to speaker for text [ ${text} ]`)
            } else {
                console.log(`Error occurred while speaking the SSML.`)
            }

            if (spokenTextQueue.length > 0) {
                speakNext(spokenTextQueue.shift())
            } else {
                isSpeaking = false
                document.getElementById('stopSpeaking').disabled = true
            }
        }).catch(
            (error) => {
                console.log(`Error occurred while speaking the SSML: [ ${error} ]`)

                if (spokenTextQueue.length > 0) {
                    speakNext(spokenTextQueue.shift())
                } else {
                    isSpeaking = false
                    document.getElementById('stopSpeaking').disabled = true
                }
            }
        )
}

function stopSpeaking() {
    spokenTextQueue = []
    avatarSynthesizer.stopSpeakingAsync().then(
        () => {
            isSpeaking = false
            document.getElementById('stopSpeaking').disabled = true
            console.log("[" + (new Date()).toISOString() + "] Stop speaking request sent.")
        }
    ).catch(
        (error) => {
            console.log("Error occurred while stopping speaking: " + error)
        }
    )
}

function handleUserQuery(userQuery) {
    let chatMessage = {
        role: 'user',
        content: userQuery
    }

    messages.push(chatMessage)
    let chatHistoryTextArea = document.getElementById('chatHistory')
    if (chatHistoryTextArea.innerHTML !== '' && !chatHistoryTextArea.innerHTML.endsWith('\n\n')) {
        chatHistoryTextArea.innerHTML += '\n\n'
    }

    chatHistoryTextArea.innerHTML += "User: " + userQuery + '\n\n'
    chatHistoryTextArea.scrollTop = chatHistoryTextArea.scrollHeight

    // Stop previous speaking if there is any
    if (isSpeaking) {
        stopSpeaking()
    }

    // For 'bring your data' scenario, chat API currently has long (4s+) latency
    // We return some quick reply here before the chat API returns to mitigate.
    if (dataSources.length > 0 && enableQuickReply) {
        speak(getQuickReply(), 2000)
    }

    const azureOpenAIEndpoint = document.getElementById('azureOpenAIEndpoint').value
    const azureOpenAIApiKey = document.getElementById('azureOpenAIApiKey').value
    const azureOpenAIDeploymentName = document.getElementById('azureOpenAIDeploymentName').value

    let url = "{AOAIEndpoint}/openai/deployments/{AOAIDeployment}/chat/completions?api-version=2023-03-15-preview".replace("{AOAIEndpoint}", azureOpenAIEndpoint).replace("{AOAIDeployment}", azureOpenAIDeploymentName)
    let body = JSON.stringify({
        messages: messages,
        stream: true
    })

    if (dataSources.length > 0) {
        url = "{AOAIEndpoint}/openai/deployments/{AOAIDeployment}/extensions/chat/completions?api-version=2023-06-01-preview".replace("{AOAIEndpoint}", azureOpenAIEndpoint).replace("{AOAIDeployment}", azureOpenAIDeploymentName)
        body = JSON.stringify({
            dataSources: dataSources,
            messages: messages,
            stream: true
        })
    }

    let assistantReply = ''
    let toolContent = ''
    let spokenSentence = ''
    let displaySentence = ''

    fetch(url, {
        method: 'POST',
        headers: {
            'api-key': azureOpenAIApiKey,
            'Content-Type': 'application/json'
        },
        body: body
    })
    .then(response => {
        if (!response.ok) {
            throw new Error(`Chat API response status: ${response.status} ${response.statusText}`)
        }

        let chatHistoryTextArea = document.getElementById('chatHistory')
        chatHistoryTextArea.innerHTML += 'Assistant: '

        const reader = response.body.getReader()

        // Function to recursively read chunks from the stream
        function read(previousChunkString = '') {
            return reader.read().then(({ value, done }) => {
                // Check if there is still data to read
                if (done) {
                    // Stream complete
                    return
                }

                // Process the chunk of data (value)
                let chunkString = new TextDecoder().decode(value, { stream: true })
                if (previousChunkString !== '') {
                    // Concatenate the previous chunk string in case it is incomplete
                    chunkString = previousChunkString + chunkString
                }

                if (!chunkString.endsWith('}}]}\n\n') && !chunkString.endsWith('[DONE]\n\n')) {
                    // This is a incomplete chunk, read the next chunk
                    return read(chunkString)
                }

                chunkString.split('\n\n').forEach((line) => {
                    try {
                        if (line.startsWith('data:') && !line.endsWith('[DONE]')) {
                            const responseJson = JSON.parse(line.substring(5).trim())
                            let responseToken = undefined
                            if (dataSources.length === 0) {
                                responseToken = responseJson.choices[0].delta.content
                            } else {
                                let role = responseJson.choices[0].messages[0].delta.role
                                if (role === 'tool') {
                                    toolContent = responseJson.choices[0].messages[0].delta.content
                                } else {
                                    responseToken = responseJson.choices[0].messages[0].delta.content
                                    if (responseToken !== undefined) {
                                        if (byodDocRegex.test(responseToken)) {
                                            responseToken = responseToken.replace(byodDocRegex, '').trim()
                                        }

                                        if (responseToken === '[DONE]') {
                                            responseToken = undefined
                                        }
                                    }
                                }
                            }

                            if (responseToken !== undefined) {
                                assistantReply += responseToken // build up the assistant message
                                displaySentence += responseToken // build up the display sentence

                                // console.log(`Current token: ${responseToken}`)

                                if (responseToken === '\n' || responseToken === '\n\n') {
                                    speak(spokenSentence.trim())
                                    spokenSentence = ''
                                } else {
                                    responseToken = responseToken.replace(/\n/g, '')
                                    spokenSentence += responseToken // build up the spoken sentence

                                    if (responseToken.length === 1 || responseToken.length === 2) {
                                        for (let i = 0; i < sentenceLevelPunctuations.length; ++i) {
                                            let sentenceLevelPunctuation = sentenceLevelPunctuations[i]
                                            if (responseToken.startsWith(sentenceLevelPunctuation)) {
                                                speak(spokenSentence.trim())
                                                spokenSentence = ''
                                                break
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } catch (error) {
                        console.log(`Error occurred while parsing the response: ${error}`)
                        console.log(chunkString)
                    }
                })

                chatHistoryTextArea.innerHTML += `${displaySentence}`
                chatHistoryTextArea.scrollTop = chatHistoryTextArea.scrollHeight
                displaySentence = ''

                // Continue reading the next chunk
                return read()
            })
        }

        // Start reading the stream
        return read()
    })
    .then(() => {
        if (spokenSentence !== '') {
            speak(spokenSentence.trim())
            spokenSentence = ''
        }

        if (dataSources.length > 0) {
            let toolMessage = {
                role: 'tool',
                content: toolContent
            }

            messages.push(toolMessage)
        }

        let assistantMessage = {
            role: 'assistant',
            content: assistantReply
        }

        messages.push(assistantMessage)
    })
}

function getQuickReply() {
    return quickReplies[Math.floor(Math.random() * quickReplies.length)]
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
                        window.startSession()
                    }
                }
            }
        }, 5000)
    }
}

window.onload = () => {
    setInterval(() => { checkHung() }, 5000) // Check session activity every 5 seconds
}

window.startSession = () => {
    const cogSvcRegion = document.getElementById('region').value
    const cogSvcSubKey = document.getElementById('subscriptionKey').value
    if (cogSvcSubKey === '') {
        alert('Please fill in the subscription key of your speech resource.')
        return
    }

    const speechSynthesisConfig = SpeechSDK.SpeechConfig.fromSubscription(cogSvcSubKey, cogSvcRegion)
    speechSynthesisConfig.endpointId = document.getElementById('customVoiceEndpointId').value
    speechSynthesisConfig.speechSynthesisVoiceName = document.getElementById('ttsVoice').value

    const talkingAvatarCharacter = document.getElementById('talkingAvatarCharacter').value
    const talkingAvatarStyle = document.getElementById('talkingAvatarStyle').value
    const avatarConfig = new SpeechSDK.AvatarConfig(talkingAvatarCharacter, talkingAvatarStyle)
    avatarConfig.customized = document.getElementById('customizedAvatar').checked
    avatarSynthesizer = new SpeechSDK.AvatarSynthesizer(speechSynthesisConfig, avatarConfig)
    avatarSynthesizer.avatarEventReceived = function (s, e) {
        var offsetMessage = ", offset from session start: " + e.offset / 10000 + "ms."
        if (e.offset === 0) {
            offsetMessage = ""
        }

        console.log("Event received: " + e.description + offsetMessage)
    }

    const speechRecognitionConfig = SpeechSDK.SpeechConfig.fromEndpoint(new URL(`wss://${cogSvcRegion}.stt.speech.microsoft.com/speech/universal/v2`), cogSvcSubKey)
    speechRecognitionConfig.setProperty(SpeechSDK.PropertyId.SpeechServiceConnection_LanguageIdMode, "Continuous")
    var sttLocales = document.getElementById('sttLocales').value.split(',')
    var autoDetectSourceLanguageConfig = SpeechSDK.AutoDetectSourceLanguageConfig.fromLanguages(sttLocales)
    speechRecognizer = SpeechSDK.SpeechRecognizer.FromConfig(speechRecognitionConfig, autoDetectSourceLanguageConfig, SpeechSDK.AudioConfig.fromDefaultMicrophoneInput())

    const azureOpenAIEndpoint = document.getElementById('azureOpenAIEndpoint').value
    const azureOpenAIApiKey = document.getElementById('azureOpenAIApiKey').value
    const azureOpenAIDeploymentName = document.getElementById('azureOpenAIDeploymentName').value
    if (azureOpenAIEndpoint === '' || azureOpenAIApiKey === '' || azureOpenAIDeploymentName === '') {
        alert('Please fill in the Azure OpenAI endpoint, API key and deployment name.')
        return
    }

    dataSources = []
    if (document.getElementById('enableByod').checked) {
        const azureCogSearchEndpoint = document.getElementById('azureCogSearchEndpoint').value
        const azureCogSearchApiKey = document.getElementById('azureCogSearchApiKey').value
        const azureCogSearchIndexName = document.getElementById('azureCogSearchIndexName').value
        if (azureCogSearchEndpoint === "" || azureCogSearchApiKey === "" || azureCogSearchIndexName === "") {
            alert('Please fill in the Azure Cognitive Search endpoint, API key and index name.')
            return
        } else {
            setDataSources(azureCogSearchEndpoint, azureCogSearchApiKey, azureCogSearchIndexName)
        }
    }

    // Only initialize messages once
    if (!messageInitiated) {
        initMessages()
        messageInitiated = true
    }

    const iceServerUrl = document.getElementById('iceServerUrl').value
    const iceServerUsername = document.getElementById('iceServerUsername').value
    const iceServerCredential = document.getElementById('iceServerCredential').value
    if (iceServerUrl === '' || iceServerUsername === '' || iceServerCredential === '') {
        alert('Please fill in the ICE server URL, username and credential.')
        return
    }

    document.getElementById('startSession').disabled = true
    document.getElementById('configuration').hidden = true

    setupWebRTC(iceServerUrl, iceServerUsername, iceServerCredential)
}

window.stopSession = () => {
    document.getElementById('startSession').disabled = false
    document.getElementById('microphone').disabled = true
    document.getElementById('stopSession').disabled = true
    document.getElementById('configuration').hidden = false

    avatarSynthesizer.close()
    if (speechRecognizer !== undefined) {
        speechRecognizer.stopContinuousRecognitionAsync()
        speechRecognizer.close()
    }

    sessionActive = false
}

window.clearChatHistory = () => {
    document.getElementById('chatHistory').innerHTML = ''
    initMessages()
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

    document.getElementById('microphone').disabled = true
    document.getElementById('audioPlayer').play()
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

window.updataEnableByod = () => {
    if (document.getElementById('enableByod').checked) {
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
                    handleUserQuery(userQuery.trim('\n'))
                    document.getElementById('userMessageBox').value = ''
                }
            }
        })
    } else {
        document.getElementById('userMessageBox').hidden = true
    }
}
