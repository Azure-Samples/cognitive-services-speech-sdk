
import { AzureOpenAI } from 'openai'
import { createRequire } from 'module'
import { fileURLToPath } from 'url'
import { dirname } from 'path'
import axios from 'axios'
const require = createRequire(import.meta.url)
const http = require('http')
const express = require('express')
const dotenv = require('dotenv')
const { v4: uuidv4, validate: uuidValidate } = require('uuid')
const speechsdk = require('microsoft-cognitiveservices-speech-sdk')
const bodyParser = require('body-parser')
const { Server } = require('socket.io')
const {DefaultAzureCredential} = require('@azure/identity')

const __filename = fileURLToPath(import.meta.url)
const __dirname = dirname(__filename)

dotenv.config()
const path = require('path')
const app = express()
const server = http.createServer(app)
const io = new Server(server)

app.use(express.json())
app.use(bodyParser.text({ type: '*/*' }))

app.use('/static', express.static(path.join(__dirname, './static/')))
app.set('view engine', 'ejs')
app.set('views', path.join(__dirname, './'))

// Environment variables
// Speech resource (required)
const speech_region = process.env.SPEECH_REGION // e.g. westus2
const speech_key = process.env.SPEECH_KEY
const speech_private_endpoint = process.env.SPEECH_PRIVATE_ENDPOINT // e.g. https://my-speech-service.cognitiveservices.azure.com/ (optional)
const speech_resource_url = process.env.SPEECH_RESOURCE_URL // e.g. /subscriptions/6e83d8b7-00dd-4b0a-9e98-dab9f060418b/resourceGroups/my-rg/providers/Microsoft.CognitiveServices/accounts/my-speech (optional, only used for private endpoint)  # noqa: E501
const user_assigned_managed_identity_client_id = process.env.USER_ASSIGNED_MANAGED_IDENTITY_CLIENT_ID // e.g. the client id of user assigned managed identity associated to your app service (optional, only used for private endpoint and user assigned managed identity)  # noqa: E501
// OpenAI resource (required for chat scenario)
const azure_openai_endpoint = process.env.AZURE_OPENAI_ENDPOINT // e.g. https://my-aoai.openai.azure.com/
const azure_openai_api_key = process.env.AZURE_OPENAI_API_KEY
const azure_openai_deployment_name = process.env.AZURE_OPENAI_DEPLOYMENT_NAME // e.g. my-gpt-35-turbo-deployment
// Customized ICE server (optional, only required for customized ICE server)
const ice_server_url = process.env.ICE_SERVER_URL // The ICE URL, e.g. turn:x.x.x.x:3478
const ice_server_url_remote = process.env.ICE_SERVER_URL_REMOTE // The ICE URL for remote side, e.g. turn:x.x.x.x:3478. This is only required when the ICE address for remote side is different from local side.  # noqa: E501
const ice_server_username = process.env.ICE_SERVER_USERNAME // The ICE username
const ice_server_password = process.env.ICE_SERVER_PASSWORD // The ICE password
// Cognitive search resource (optional, only required for 'on your data' scenario)
const cognitive_search_index_name = process.env.COGNITIVE_SEARCH_INDEX_NAME // e.g. my-search-index
const cognitive_search_api_key = process.env.COGNITIVE_SEARCH_API_KEY
const cognitive_search_endpoint = process.env.COGNITIVE_SEARCH_ENDPOINT // e.g. https://my-cognitive-search.search.windows.net/
// Const variables
const default_tts_voice = 'en-US-JennyMultilingualV2Neural' // Default TTS voice
const enable_token_auth_for_speech = false // Enable token authentication for speech
const enable_websockets = true // Enable websockets between client and server for real-time communication optimization
const enable_quick_reply = false // Enable quick reply for certain chat models which take longer time to respond
const quick_replies = ['Let me take a look.', 'Let me check.', 'One moment, please.'] // Quick reply responses
const oyd_doc_regex = /\[doc(\d+)\]/ // Regex to match the OYD (on-your-data) document reference
const sentence_level_punctuations = ['.', '?', '!', ':', ';', '。', '？', '！', '：', '；'] // Punctuations that indicate the end of a sentence
const repeat_speaking_sentence_after_reconnection = true // Repeat the speaking sentence after reconnection

// Global variables
const client_contexts = {}
let ice_token = null
let speech_token = null
let azure_openai

if (azure_openai_endpoint && azure_openai_api_key) {
    azure_openai = new AzureOpenAI({
        azureEndpoint: azure_openai_endpoint,
        apiVersion: '2024-06-01',
        apiKey: azure_openai_api_key,
    })
}

// The default route, which shows the default web page (basic.ejs)
app.get('/', (req, res) => {
    const client_id = initializeClient()
    res.render('basic', { client_id: client_id})
})

// The basic route, which shows the basic web page
app.get('/basic', (req, res) => {
    const client_id = initializeClient()
    res.render('basic', { client_id: client_id})
})

// The chat route, which shows the chat web page
app.get('/chat', (req, res) => {
    const client_id = initializeClient()
    res.render('chat', { client_id: client_id, enable_websockets: enable_websockets })
})

// The API route to get the speech token
app.get('/api/getSpeechToken', (req, res) => {
    res.set('SpeechRegion', speech_region)
    res.set('SpeechKey', speech_key)
    if (speech_private_endpoint) {
        res.set('SpeechPrivateEndpoint', speech_private_endpoint)
    }
    res.status(200).send(speech_token)
})

// The API route to get the ICE token
app.get('/api/getIceToken', (req, res) => {
    if (ice_server_url && ice_server_username && ice_server_password) {
        const custom_ice_token = {
            Urls: [ice_server_url],
            Username: ice_server_username,
            Password: ice_server_password
        }
        return res.status(200).send(custom_ice_token)
    }

    return res.status(200).send(ice_token)

})

// The API route to get the speech token
app.get('/api/getStatus', (req, res) => {
    const client_id = req.headers['clientid']
    if (!client_id || !uuidValidate(client_id)) {
        return res.status(400).send('Invalid or missing ClientId.')
    }

    const client_context = client_contexts[client_id]
    if (!client_context) {
        return res.status(404).send('Client context not found.')
    }

    const status = {
        speechSynthesizerConnected: client_context.speech_synthesizer_connected
    }
    res.status(200).send(status)
})

// The API route to connect the TTS avatar
app.post('/api/connectAvatar', async (req, res) => {
    try {
        const client_id = req.headers['clientid']
        const isReconnecting = req.headers['reconnect']?.toLowerCase() === 'true'
        // disconnect avatar if already connected
        await disconnectAvatarInternal(client_id, isReconnecting)
        const client_context = client_contexts[client_id]

        // Override default values with client provided values
        client_context.azure_openai_deployment_name = req.headers['aoaideploymentname'] || azure_openai_deployment_name
        client_context.cognitive_search_index_name = req.headers['cognitivesearchindexname'] || cognitive_search_index_name
        client_context.tts_voice = req.headers['ttsvoice'] || default_tts_voice
        client_context.custom_voice_endpoint_id = req.headers['customvoiceendpointid'] || ""
        client_context.personal_voice_speaker_profile_id = req.headers['personalvoicespeakerprofileid'] || ""

        const custom_voice_endpoint_id = client_context['custom_voice_endpoint_id']

        let is_photo_avatar = req.headers['isphotoavatar']?.toLowerCase() === 'true'
        let is_custom_avatar = req.headers['iscustomavatar']?.toLowerCase() === 'true'
        let is_custom_voice = custom_voice_endpoint_id !== undefined && custom_voice_endpoint_id !== null && custom_voice_endpoint_id !== ''
        let endpoint_route = is_custom_avatar || is_custom_voice ? 'voice' : 'tts'

        let speech_config
        if (speech_private_endpoint) {
            const speech_private_endpoint_wss = speech_private_endpoint.replace('https://', 'wss://')
            if (enable_token_auth_for_speech) {
                while (!speech_token) {
                    await new Promise(r => setTimeout(r, 200))
                }
                speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`${speech_private_endpoint_wss}/${endpoint_route}/cognitiveservices/websocket/v1?enableTalkingAvatar=true`))
                speech_config.authorizationToken = speech_token
            } else {
                speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`${speech_private_endpoint_wss}/${endpoint_route}/cognitiveservices/websocket/v1?enableTalkingAvatar=true`), speech_key)
            }
        } else {
            if (enable_token_auth_for_speech) {
                while (!speech_token) {
                    await new Promise(r => setTimeout(r, 200))
                }
                speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`wss://${speech_region}.${endpoint_route}.speech.microsoft.com/cognitiveservices/websocket/v1?enableTalkingAvatar=true`))
                speech_config.authorizationToken = speech_token
            } else {
                speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`wss://${speech_region}.${endpoint_route}.speech.microsoft.com/cognitiveservices/websocket/v1?enableTalkingAvatar=true`), speech_key)

            }
        }

        if (custom_voice_endpoint_id) {
            speech_config.endpointId = custom_voice_endpoint_id
        }

        client_context.speech_synthesizer = new speechsdk.SpeechSynthesizer(speech_config, null)
        const speech_synthesizer = client_context.speech_synthesizer

        let ice_token_obj = ice_token
        // Apply customized ICE server if provided
        if (ice_server_url && ice_server_username && ice_server_password) {
            ice_token_obj = {
                Urls: ice_server_url_remote ? [ice_server_url_remote] : [ice_server_url],
                Username: ice_server_username,
                Password: ice_server_password
            }
        }

        const local_sdp = req.body.toString('utf-8')
        const avatar_character = req.headers['avatarcharacter']
        const avatar_style = req.headers['avatarstyle']
        const background_color = req.headers['backgroundcolor'] || 'FFFFFFFF'
        const background_image_url = req.headers['backgroundimageurl']
        const transparent_background = req.headers['transparentbackground']?.toLowerCase() === 'true'
        const video_crop = req.headers['videocrop']?.toLowerCase() === 'true'

        const avatar_config = {
            synthesis: {
                video: {
                    protocol: {
                        name: "WebRTC",
                        webrtcConfig: {
                            clientDescription: local_sdp,
                            iceServers: [{
                                urls: [ice_token_obj.Urls[0]],
                                username: ice_token_obj.Username,
                                credential: ice_token_obj.Password
                            }]
                        }
                    },
                    format: {
                        crop: {
                            topLeft: { x: video_crop ? 600 : 0, y: 0 },
                            bottomRight: { x: video_crop ? 1320 : 1920, y: 1080 }
                        },
                        bitrate: 1000000,
                        codec: 'H264'
                    },
                    talkingAvatar: {
                        photoAvatarBaseModel: is_photo_avatar ? 'vasa-1' : '',
                        customized: is_custom_avatar,
                        useBuiltInVoice: false,
                        character: avatar_character,
                        style: avatar_style,
                        background: {
                            color: transparent_background ? '#00FF00FF' : background_color,
                            image: { url: background_image_url }
                        },
                        scene: {
                            zoom: 1.0,
                            positionX: 0.0,
                            positionY: 0.0,
                            rotationX: 0.0,
                            rotationY: 0.0,
                            rotationZ: 0.0,
                            amplitude: 1.0
                        }
                    }
                }
            }
        }

        const connection = speechsdk.Connection.fromSynthesizer(speech_synthesizer)
        connection.connected = function (evt) {
            console.log('TTS Avatar service connected.');
        }

        connection.disconnected = function () {
            console.log('TTS Avatar service disconnected.')
            client_context.speech_synthesizer_connection = null
            client_context.speech_synthesizer_connected = false
            if (enable_websockets) {
                io.to(client_id).emit("response", {
                    path: "api.event",
                    eventType: "SPEECH_SYNTHESIZER_DISCONNECTED"
                })

            }
        }

        connection.setMessageProperty('speech.config', 'context', JSON.stringify(avatar_config))
        client_context.speech_synthesizer_connection = connection
        client_context.speech_synthesizer_connected = true

        if (enable_websockets) {
            io.to(client_id).emit("response", {
                path: "api.event",
                eventType: "SPEECH_SYNTHESIZER_CONNECTED"
            })
        }

        speech_synthesizer.speakTextAsync(' ', (result) => {
            if (result.reason === speechsdk.ResultReason.SynthesizingAudioCompleted) {
                console.log("[" + (new Date()).toISOString() + "] Avatar service started. Result ID: " + result.resultId)
                let remoteSdp = result.properties.getProperty("TalkingAvatarService_WebRTC_SDP")
                // console.log("[" + (new Date()).toISOString() + "] Remote SDP: " + remoteSdp)
                res.status(200).send(remoteSdp)
            } else {
                console.log("[" + (new Date()).toISOString() + "] Unable to start avatar service. Result ID: " + result.resultId)
                if (result.reason === speechsdk.ResultReason.Canceled) {
                    let cancellationDetails = speechsdk.CancellationDetails.fromResult(result)
                    if (cancellationDetails.reason === speechsdk.CancellationReason.Error) {
                        console.log(cancellationDetails.errorDetails)
                    }
                    console.log("Unable to start avatar service: " + cancellationDetails.errorDetails)
                }
            }
        })
    } catch (e) {
        res.status(400).send(`Error message: ${e.message}`)
    }

})

app.post('/api/connectSTT', async (req, res) => {
    const client_id = req.headers['clientid']
    disconnectSttInternal(client_id)
    const system_prompt = req.headers['systemprompt']
    const client_context = client_contexts[client_id]

    let speech_config
    if (speech_private_endpoint) {
        const speech_private_endpoint_wss = speech_private_endpoint.replace('https://', 'wss://')
        if (enable_token_auth_for_speech) {
            while (!speech_token) {
                await new Promise(r => setTimeout(r, 200))
            }
            speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`${speech_private_endpoint_wss}/stt/speech/universal/v2`))
            speech_config.authorizationToken = speech_token
        } else {
            speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`${speech_private_endpoint_wss}/stt/speech/universal/v2`), speech_key)
        }
    } else {
        if (enable_token_auth_for_speech) {
            while (!speech_token) {
                await new Promise(r => setTimeout(r, 200))
            }
            speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`wss://${speech_region}.stt.speech.microsoft.com/speech/universal/v2`))
            speech_config.authorizationToken = speech_token
        } else {
            speech_config = speechsdk.SpeechConfig.fromEndpoint(new URL(`wss://${speech_region}.stt.speech.microsoft.com/speech/universal/v2`), speech_key)
        }
    }
    const audio_input_stream = speechsdk.AudioInputStream.createPushStream()
    client_context.audio_input_stream = audio_input_stream

    const audio_config = speechsdk.AudioConfig.fromStreamInput(audio_input_stream)
    const speech_recognizer = new speechsdk.SpeechRecognizer(speech_config, audio_config)
    client_context.speech_recognizer = speech_recognizer

    speech_recognizer.sessionStarted = (s, e) => {
        console.log(`STT session started- session id: ${e.sessionId}`)
    }

    speech_recognizer.sessionStopped = (s, e) => {
        console.log('STT session stopped')
    }

    const speech_recognition_start_time = new Date()

    speech_recognizer.recognized = async (s, e) => {
        if (e.result.reason === speechsdk.ResultReason.RecognizedSpeech) {
            try {
                const user_query = e.result.text.trim()
                if (user_query === '') return

                io.to(client_id).emit("response", {
                    path: "api.chat",
                    chatResponse: `\n\nUser: ${user_query}\n\n`
                })

                const recognition_result_received_time = new Date()
                const speech_finished_offset = (e.result.offset + e.result.duration) / 10000
                const stt_latency = Math.round(recognition_result_received_time.getTime() - speech_recognition_start_time.getTime() - speech_finished_offset)
                console.log(`STT latency: ${stt_latency}ms`)

                io.to(client_id).emit("response", {
                    path: "api.chat",
                    chatResponse: `<STTL>${stt_latency}</STTL>`
                })

                if (!client_context.chat_initiated) {
                    initializeChatContext(system_prompt, client_id)
                    client_context.chat_initiated = true
                }

                let first_response_chunk = true
                for await (const chat_response of handleUserQuery(user_query, client_id)) {
                    if (first_response_chunk) {
                        io.to(client_id).emit('response', {
                            path: 'api.chat',
                            chatResponse: 'Assistant:'
                        })
                        first_response_chunk = false
                    }
                    io.to(client_id).emit('response', {
                        path: 'api.chat',
                        chatResponse: chat_response
                    })
                }

            } catch (err) {
                console.log('Error in handling user query:', err)
            }
        }
    }

    speech_recognizer.Canceled = (s, e) => {
        const cancellation_details = speechsdk.CancellationDetails.fromResult(e.result)
        console.log(`STT connection canceled. Error message: ${cancellation_details.error_details}`)
    }

    speech_recognizer.startContinuousRecognitionAsync(() => {
        console.log('STT continuous recognition started.')
        res.status(200).send()
    },
        (err) => {
            console.error('STT connection failed. Error message:', err)
            res.status(400).send(`STT connection failed. Error message: ${err}`)
        }
    )
})

// The API route to disconnect the STT service
app.post('/api/disconnectSTT', (req, res) => {
    const client_id = req.headers['clientid']
    try {
        disconnectSttInternal(client_id)
        res.status(200).send('STT Disconnected.')
    } catch (e) {
        res.status(400).send(`STT disconnection failed. Error message: ${e}`)
    }
})

// The API route to speak a given SSML
app.post('/api/speak', async (req, res) => {
    const client_id = req.headers['clientid']
    try {
        const ssml = req.body.toString('utf-8')
        const result_id = await speakSsml(ssml, client_id)
        res.status(200).send(result_id)
    } catch (e) {
        res.status(400).send(`Speak failed. Error message: ${e}`)
    }
})

// The API route to stop avatar from speaking
app.post('/api/stopSpeaking', async (req, res) => {
    const client_id = req.headers['clientid']
    await stopSpeakingInternal(client_id, false)
    res.status(200).send('Speaking stopped.')
})

// The API route to update the avatar scene
app.post('/api/updateScene', async (req, res) => {
    try {
        const client_id = req.headers['clientid']
        const scene_request = req.body
        const scene_config = {
            avatarScene: {
                zoom: scene_request.zoom,
                positionX: scene_request.positionX,
                positionY: scene_request.positionY,
                rotationX: scene_request.rotationX,
                rotationY: scene_request.rotationY,
                rotationZ: scene_request.rotationZ,
                amplitude: scene_request.amplitude
            }
        }
        const client_context = client_contexts[client_id]
        const avatar_connection = client_context.speech_synthesizer_connection
        if (avatar_connection) {
            await avatar_connection.sendMessageAsync('synthesis.control', JSON.stringify(scene_config))
            res.status(200).send('Scene updated.')
        } else {
            res.status(400).send('Connection not available.')
        }
    } catch (error) {
        res.status(500).send(`Error: ${error.message}`)
    }
})

// The API route for chat
// It receives the user query and return the chat response.
// It returns response in stream, which yields the chat response in chunks.
app.post('/api/chat', async (req, res) => {
    const client_id = req.headers['clientid']
    const client_context = client_contexts[client_id]
    const chat_initiated = client_context.chat_initiated
    if (!chat_initiated) {
        initializeChatContext(req.headers['systemprompt'], client_id)
        client_context.chat_initiated = true
    }
    const user_query = req.body.toString('utf-8')

    for await (const chunk of handleUserQuery(user_query, client_id)) {
        res.write(chunk)
    }
    res.end()
})

// The API route to continue speaking the unfinished sentences
app.post('/api/chat/continueSpeaking', (req, res) => {
    const client_id = req.headers['clientid']
    const client_context = client_contexts[client_id]
    const spoken_text_queue = client_context.spoken_text_queue
    const speaking_text = client_context.speaking_text
    if (speaking_text && repeat_speaking_sentence_after_reconnection) {
        client_context.spoken_text_queue.unshift(speaking_text)
    }

    if (spoken_text_queue.length > 0) {
        client_context.can_speak_next = true
        speakWithQueue('', 0, client_id)
    }
    res.status(200).send('Request sent.')
})

// The API route to clear the chat history
app.post('/api/chat/clearHistory', (req, res) => {
    const client_id = req.headers['clientid']
    const client_context = client_contexts[client_id]
    initializeChatContext(req.headers['systemprompt'], client_id)
    client_context.chat_initiated = true
    res.status(200).send('Chat history cleared.')
})

// The API route to disconnect the TTS avatar
app.post('/api/disconnectAvatar', async (req, res) => {
    const client_id = req.headers['clientid']
    try {
        await disconnectAvatarInternal(client_id, false)
        res.status(200).send('Disconnected avatar.')
    } catch (err) {
        res.status(400).send(err.stack || err.message)
    }
})

// The API route to release the client context, to be invoked when the client is closed
app.post('/api/releaseClient', async (req, res) => {
    const client_id = req.headers['clientid']
    if (client_id) {
        try {
            await disconnectAvatarInternal(client_id, false)
            disconnectSttInternal(client_id)
            await new Promise(resolve => setTimeout(resolve, 2000))
            delete client_contexts[client_id]
            console.log(`Client context released for client ${client_id}`)
            res.status(200).send('Client context released.')
        } catch (e) {
            console.log(`Client context release failed. Error message: ${e}`)
            res.status(400).send(`Client context release failed. Error message: ${e}`)
        }
    }
})


io.on("connection", (socket) => {
    const client_id = socket.handshake.query.clientId
    if (!client_id) {
        console.log('Missing clientId in connection query.')
        return
    }
    socket.join(client_id)
    console.log(`WebSocket connected for client ${client_id}.`)

    socket.on('message', async (message) => {
        const client_id = message.clientId
        const path = message.path
        const client_context = client_contexts[client_id]
        if (client_context) {
            if (path === 'api.audio') {
                const audio_chunk = message.audioChunk
                const audio_chunk_binary = Buffer.from(audio_chunk, 'base64')
                const audio_input_stream = client_context.audio_input_stream
                if (audio_input_stream) {
                    audio_input_stream.write(audio_chunk_binary)
                }
            } else if (path === 'api.chat') {
                if (!client_context.chat_initiated) {
                    initializeChatContext(message.system_prompt, client_id)
                    client_context.chat_initiated = true
                }
                const user_query = message.userQuery
                let first_response_chunk = true
                for await (const chat_response of handleUserQuery(user_query, client_id)) {
                    if (first_response_chunk) {
                        io.to(client_id).emit('response', {
                            path: 'api.chat',
                            chatResponse: 'Assistant:'
                        })
                        first_response_chunk = false
                    }
                    io.to(client_id).emit('response', {
                        path: 'api.chat',
                        chatResponse: chat_response
                    })
                }
            } else if (path === 'api.stopSpeaking') {
                await stopSpeakingInternal(client_id, false)
            }
        }
    })
})

// Initialize the client by creating a client id and an initial context
function initializeClient() {
    const client_id = uuidv4()
    client_contexts[client_id] = {
        audio_input_stream: null,
        speech_recognizer: null,
        azure_openai_deployment_name: azure_openai_deployment_name,
        cognitive_search_index_name: cognitive_search_index_name,
        tts_voice: default_tts_voice,
        custom_voice_endpoint_id: null,
        personal_voice_speaker_profile_id: null,
        speech_synthesizer: null,
        speech_synthesizer_connection: null,
        speech_synthesizer_connected: false,
        speech_token: null,
        ice_token: null,
        chat_initiated: false,
        messages: [],
        data_sources: [],
        is_speaking: false,
        speaking_text: null,
        spoken_text_queue: [],
        speaking_thread: null,
        last_speak_time: null,
        can_speak_next: true
    };
    return client_id
}

// Refresh the ICE token every 24 hours
async function refreshIceToken() {
        while (true) {
            let ice_token_response = null
            if (speech_private_endpoint) {
                if (enable_token_auth_for_speech) {
                    while (!speech_token) {
                        await new Promise(resolve => setTimeout(resolve, 2000))
                    }
                    ice_token_response = await axios.get(`${speech_private_endpoint}/tts/cognitiveservices/avatar/relay/token/v1`, {headers: {Authorization: `Bearer ${speech_token}`}})
                } else {
                    ice_token_response = await axios.get(`${speech_private_endpoint}/tts/cognitiveservices/avatar/relay/token/v1`, {headers: {'Ocp-Apim-Subscription-Key': speech_key}})
                }
            } else {
                if (enable_token_auth_for_speech) {
                    while (!speech_token) {
                        await new Promise(resolve => setTimeout(resolve, 2000))
                    }
                    ice_token_response = await axios.get(`https://${speech_region}.tts.speech.microsoft.com/cognitiveservices/avatar/relay/token/v1`, {headers: {Authorization: `Bearer ${speech_token}`}})
                } else {
                    ice_token_response = await axios.get(`https://${speech_region}.tts.speech.microsoft.com/cognitiveservices/avatar/relay/token/v1`, {headers: {'Ocp-Apim-Subscription-Key': speech_key}})
                }
            }
    
            if (ice_token_response.status === 200) {
                ice_token = ice_token_response.data
            } else {
                throw new Error(`Failed to get ICE token. Status code: ${response.status}`)
            }
        }
}

// Refresh the speech token every 9 minutes
async function refreshSpeechToken() {
        while (true) {
            if  (speech_private_endpoint) {
                const credential = new DefaultAzureCredential({managedIdentityClientId: user_assigned_managed_identity_client_id})
                const token = await credential.getToken('https://cognitiveservices.azure.com/.default')
                speech_token = `add#${speech_resource_url}#${token.token}`
            } else {
                const response = await axios.post(`https://${speech_region}.api.cognitive.microsoft.com/sts/v1.0/issueToken`, null, {headers: {'Ocp-Apim-Subscription-Key': speech_key}})
                speech_token = response.data
            }
        }
}

// Initialize the chat context, e.g. chat history (messages), data sources, etc. For chat scenario.
function initializeChatContext(system_prompt, client_id) {
    const client_context = client_contexts[client_id]
    const cognitive_search_index_name = client_context.cognitive_search_index_name
    const messages = client_context.messages
    let data_sources = client_context.data_sources

    // Initialize data sources for 'on your data' scenario
    data_sources = []
    if (cognitive_search_endpoint && cognitive_search_api_key && cognitive_search_index_name) {
        data_source = {
            type: 'azure_search',
            parameters: {
                endpoint: cognitive_search_endpoint,
                index_name: cognitive_search_index_name,
                authentication: {
                    type: 'api_key',
                    key: cognitive_search_api_key
                },
                semantic_configuration: '',
                query_type: 'simple',
                fields_mapping: {
                    content_fields_separator: '\n',
                    content_fields: ['content'],
                    filepath_field: null,
                    title_field: 'title',
                    url_field: null
                },
                in_scope: true,
                role_information: system_prompt
            }
        }
        data_sources.push(data_source)

        // Initialize messages
        messages.clear()
        if (data_sources.length === 0) {
            const system_message = {
                role: 'system',
                content: system_prompt
            }
            messages.push(system_message)
        }
    }
}

// Handle the user query and return the assistant reply. For chat scenario.
// The function is a generator, which yields the assistant reply in chunks.
async function* handleUserQuery(user_query, client_id) {
    const client_context = client_contexts[client_id]
    const azure_openai_deployment_name = client_context.azure_openai_deployment_name
    const messages = client_context.messages
    const data_sources = client_context.data_sources

    const chat_message = {
        role: 'user',
        content: user_query
    }

    messages.push(chat_message)

    // For 'on your data' scenario, chat API currently has long (4s+) latency
    // We return some quick reply here before the chat API returns to mitigate.
    if (data_sources.length > 0 && enable_quick_reply) {
        const reply = quick_replies[Math.floor(Math.random() * quick_replies.length)]
        speakWithQueue(reply, 2000)
    }

    let assistant_reply = ''
    let tool_content = ''
    let spoken_sentence = ''

    const aoai_start_time = new Date()
    const response = await azure_openai.chat.completions.create({
        model: azure_openai_deployment_name,
        messages: messages,
        stream: true,
        ...(data_sources.length > 0 && { dataSources: data_sources })
    })
    let is_first_chunk = true
    let is_first_sentence = true
    for await (const chunk of response) {
        if (chunk.choices.length > 0) {
            let response_token = chunk.choices[0].delta.content
            if (response_token) {
                // Log response_token here if need debug
                if (is_first_chunk) {
                    const first_token_latency_ms = new Date().getTime() - aoai_start_time.getTime()
                    console.log(`AOAI first token latency: ${first_token_latency_ms}ms`)
                    yield (`<FTL>${first_token_latency_ms}</FTL>`);
                    is_first_chunk = false
                }
                if (oyd_doc_regex.test(response_token)) {
                    response_token = response_token.replace(oyd_doc_regex, '').trim()
                }
                yield (response_token)
                assistant_reply += response_token //build up the assistant message

                if (response_token === '\n' || response_token == '\n\n') {
                    if (is_first_sentence) {
                        const first_sentence_latency_ms = new Date().getTime() - aoai_start_time.getTime()
                        console.log(`AIAO first sentence latency: ${first_sentence_latency_ms}ms`)
                        yield (`<FSL>${first_sentence_latency_ms}</FSL>`)
                        is_first_sentence = false
                    }
                    speakWithQueue(spoken_sentence.trim(), 0, client_id)
                    spoken_sentence = ''
                } else {
                    response_token = response_token.replace('\n', '')
                    spoken_sentence += response_token // build up the spoken sentence

                    if (response_token.length === 1 || response_token.length === 2) {
                        for (const punctuation of sentence_level_punctuations) {
                            if (response_token.startsWith(punctuation)) {
                                if (is_first_sentence) {
                                    const first_sentence_latency_ms = new Date().getTime() - aoai_start_time.getTime()
                                    console.log(`AOAI first sentence latency: ${first_sentence_latency_ms}ms`)
                                    yield (`<FSL>${first_sentence_latency_ms}</FSL>`);
                                    is_first_sentence = false
                                }
                                speakWithQueue(spoken_sentence.trim(), 0, client_id)
                                spoken_sentence = ''
                                break
                            }
                        }
                    }
                }
            }
        }
    }

    if (spoken_sentence !== '') {
        speakWithQueue(spoken_sentence.trim(), 0, client_id)
        spoken_sentence = ''
    }

    if (data_sources.length > 0) {
        const tool_message = {
            role: 'tool',
            content: tool_content
        }
        messages.push(tool_message)
    }

    const assistant_message = {
        role: 'assistant',
        content: assistant_reply
    }
    messages.push(assistant_message)
}

// Speak the given text. If there is already a speaking in progress, add the text to the queue. For chat scenario.
function speakWithQueue(text, ending_silence_ms, client_id) {
    const client_context = client_contexts[client_id]
    const is_speaking = client_context.is_speaking
    if (text) {
        client_context.spoken_text_queue.push(text)
    }
    if (!is_speaking) {
        async function speakThread() {
            const tts_voice = client_context.tts_voice
            const personal_voice_speaker_profile_id = client_context.personal_voice_speaker_profile_id
            client_context.is_speaking = true
            while (client_context.spoken_text_queue.length > 0) {
                if (!client_context.can_speak_next) {
                    await new Promise(resolve => setTimeout(resolve, 100))
                    continue
                }

                const current_text = client_context.spoken_text_queue.shift()
                client_context.speaking_text = current_text
                client_context.can_speak_next = false
                try {
                    await speakText(current_text, tts_voice, personal_voice_speaker_profile_id, ending_silence_ms, client_id)
                } catch (e) {
                    console.log(`Error in speaking text: ${e}`)
                    break
                }

                client_context.last_speak_time = new Date()
            }
            client_context.is_speaking = false
            client_context.speaking_text = null
            console.log('Speaking thread stopped')
        }

        setImmediate(speakThread);
        client_context.speaking_thread = speakThread

    }
}

function htmlEscape(str) {
    return str
        .replace(/&/g, '&amp;')
        .replace(/</g, '&lt;')
        .replace(/>/g, '&gt;')
        .replace(/"/g, '&quot;')
        .replace(/'/g, '&#x27;');
}

// Speak the given text.
async function speakText(text, voice, speaker_profile_id, ending_silence_ms, client_id) {
    let ssml = `<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>
                 <voice name='${voice}'>
                     <mstts:ttsembedding speakerProfileId='${speaker_profile_id}'>
                         <mstts:leadingsilence-exact value='0'/>
                         ${htmlEscape(text)}
                     </mstts:ttsembedding>
                 </voice>
               </speak>`
    if (ending_silence_ms > 0) {
        ssml = `<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>
                     <voice name='${voice}'>
                         <mstts:ttsembedding speakerProfileId='${speaker_profile_id}'>
                             <mstts:leadingsilence-exact value='0'/>
                             ${htmlEscape(text)}
                             <break time='${ending_silence_ms}ms' />
                         </mstts:ttsembedding>
                     </voice>
                   </speak>`
    }
    await speakSsml(ssml, client_id)
}

// Speak the given ssml with speech sdk
async function speakSsml(ssml, client_id) {
    const speech_synthesizer = client_contexts[client_id].speech_synthesizer

    return new Promise((resolve, reject) => {
            speech_synthesizer.speakSsmlAsync(
                ssml,
                result => {
                    if (result.reason === speechsdk.ResultReason.SynthesizingAudioCompleted) {
                        console.log('The current text playback is complete.')
                        client_contexts[client_id].can_speak_next = true
                        resolve(result.resultId)
                    } else if (result.reason === speechsdk.ResultReason.Canceled) {
                        const cancellation_details = speechsdk.CancellationDetails.fromResult(result)
                        console.log(`Speech synthesis canceled: ${cancellation_details.reason}`)
                        if (cancellation_details.reason === speechsdk.CancellationReason.Error) {
                            console.log(`Result ID: ${result.resultId}. Error details: ${cancellation_details.errorDetails}`)
                        }
                    }
                },
                error => reject(error)
            )
    })
}

// Stop speaking internal function
async function stopSpeakingInternal(client_id, skipClearingSpokenTextQueue) {
    const client_context = client_contexts[client_id]
    client_context.is_speaking = false
    if (!skipClearingSpokenTextQueue) {
        client_context.spoken_text_queue.length = 0
    }
    const avatar_connection = client_context.speech_synthesizer_connection
    if (avatar_connection) {
        await avatar_connection.sendMessageAsync('synthesis.control', '{"action":"stop"}')
    }

}

// Disconnect avatar internal function
async function disconnectAvatarInternal(client_id, isReconnecting) {
    const client_context = client_contexts[client_id]
    const speech_synthesizer = client_context.speech_synthesizer
    await stopSpeakingInternal(client_id, isReconnecting)
    await new Promise(resolve => setTimeout(resolve, 2000))
    const avatar_connection = client_context.speech_synthesizer_connection
    if (avatar_connection) {
        avatar_connection.close()
        io.to(client_id).emit("response", {
            path: "api.event",
            eventType: "SPEECH_SYNTHESIZER_DISCONNECTED"
        })
    }

    if (speech_synthesizer) {
        speech_synthesizer.close()
        client_contexts[client_id].speech_synthesizer = null

    }

}

// Disconnect STT internal function
function disconnectSttInternal(client_id) {
    const client_context = client_contexts[client_id]
    const speech_recognizer = client_context.speech_recognizer
    const audio_input_stream = client_context.audio_input_stream
    if (speech_recognizer) {
        speech_recognizer.stopContinuousRecognitionAsync(() => {
            const connection = speechsdk.Connection.fromRecognizer(speech_recognizer)
            connection.close()
            client_context.speech_recognizer = null
        })
    }

    if (audio_input_stream) {
        audio_input_stream.close()
        client_context.audio_input_stream = null
    }
}

setInterval(refreshIceToken, 1000 * 60 * 60 * 24)
refreshIceToken()

setInterval(refreshSpeechToken, 1000 * 60 * 9)
refreshSpeechToken()

const PORT = 5000
server.listen(PORT, () => {
    console.log(`Server running at http://localhost:${PORT}`)
})

