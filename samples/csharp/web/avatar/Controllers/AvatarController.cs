//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Avatar.Models;
using Avatar.Services;
using Azure;
using Azure.AI.OpenAI;
using Azure.AI.OpenAI.Chat;
using Azure.Core;
using Microsoft.AspNetCore.Mvc;
using Microsoft.CognitiveServices.Speech;
using Microsoft.Extensions.Options;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using OpenAI.Chat;
using System.ClientModel;
using System.Text;
using System.Web;

namespace Avatar.Controllers
{
    public class HomeController(IOptions<ClientSettings> clientSettings, IClientService clientService, ClientContext clientContext) : Controller
    {
        private readonly ClientSettings _clientSettings = clientSettings.Value;

        private readonly IClientService _clientService = clientService;

        private readonly ClientContext _clientContext = clientContext;

        private static ChatClient? chatClient;

        [HttpGet("")]
        public IActionResult Index()
        {
            var clientId = _clientService.InitializeClient();
            return View("Basic", clientId);
        }

        [HttpGet("basic")]
        public IActionResult BasicView()
        {
            var clientId = _clientService.InitializeClient();
            return View("Basic", clientId);
        }

        [HttpGet("chat")]
        public IActionResult ChatView()
        {
            var clientId = _clientService.InitializeClient();
            if (chatClient == null)
            {
                if (_clientSettings.AzureOpenAIEndpoint != null &&
                    _clientSettings.AzureOpenAIAPIKey != null &&
                    _clientSettings.AzureOpenAIDeploymentName != null)
                {
                    AzureOpenAIClient aoaiClient = new AzureOpenAIClient(
                        new Uri(_clientSettings.AzureOpenAIEndpoint),
                        new ApiKeyCredential(_clientSettings.AzureOpenAIAPIKey));

                    chatClient = aoaiClient.GetChatClient(_clientSettings.AzureOpenAIDeploymentName);
                }
            }

            return View("Chat", clientId);
        }

        [HttpGet("api/getSpeechToken")]
        public IActionResult GetSpeechToken()
        {
            try
            {
                // Retrieve the speech token and other variables
                var speechToken = GlobalVariables.SpeechToken;
                var speechRegion = _clientSettings.SpeechRegion;
                var speechPrivateEndpoint = _clientSettings.SpeechPrivateEndpoint;

                // Create a ContentResult to allow setting response headers
                var contentResult = new ContentResult
                {
                    Content = speechToken,
                    ContentType = "text/plain"
                };

                // Set response headers
                Response.Headers["SpeechRegion"] = speechRegion;
                if (!string.IsNullOrEmpty(speechPrivateEndpoint))
                {
                    Response.Headers["SpeechPrivateEndpoint"] = speechPrivateEndpoint;
                }

                return contentResult;
            }
            catch (Exception ex)
            {
                return BadRequest(ex.Message);
            }
        }

        [HttpGet("api/getIceToken")]
        public IActionResult GetIceToken()
        {
            // Apply customized ICE server if provided
            if (!string.IsNullOrEmpty(_clientSettings.IceServerUrl) &&
                !string.IsNullOrEmpty(_clientSettings.IceServerUsername) &&
                !string.IsNullOrEmpty(_clientSettings.IceServerPassword))
            {
                var customIceToken = new
                {
                    Urls = new[] { _clientSettings.IceServerUrl },
                    Username = _clientSettings.IceServerUsername,
                    Password = _clientSettings.IceServerPassword
                };

                var customIceTokenJson = JsonConvert.SerializeObject(customIceToken);
                return base.Content(customIceTokenJson, "application/json");
            }

            try
            {
                var iceToken = GlobalVariables.IceToken ?? string.Empty;
                return Content(iceToken, "application/json");
            }
            catch (Exception ex)
            {
                return BadRequest(ex.Message);
            }
        }

        [HttpGet("api/getStatus")]
        public IActionResult GetStatus()
        {
            try
            {
                var clientId = new Guid(Request.Headers["ClientId"]!);
                var clientContext = _clientService.GetClientContext(clientId);

                var status = new
                {
                    speechSynthesizerConnected = clientContext.SpeechSynthesizerConnected
                };

                return Ok(JsonConvert.SerializeObject(status));

            }
            catch (Exception ex)
            {
                return BadRequest(new { message = ex.Message });
            }
        }

        [HttpPost("api/connectAvatar")]
        public async Task<IActionResult> ConnectAvatar()
        {
            try
            {
                var clientId = new Guid(Request.Headers["ClientId"]!);
                var clientContext = _clientService.GetClientContext(clientId);
                bool isReconnecting = Request.Headers.ContainsKey("Reconnect") &&
                      string.Equals(Request.Headers["Reconnect"], "true", StringComparison.OrdinalIgnoreCase);

                // disconnect avatar if already connected
                await DisconnectAvatarInternal(clientId, isReconnecting);

                // Override default values with client provided values
                clientContext.AzureOpenAIDeploymentName = Request.Headers["AoaiDeploymentName"].FirstOrDefault() ?? _clientSettings.AzureOpenAIDeploymentName;
                clientContext.CognitiveSearchIndexName = Request.Headers["CognitiveSearchIndexName"].FirstOrDefault() ?? _clientSettings.CognitiveSearchIndexName;
                clientContext.TtsVoice = Request.Headers["TtsVoice"].FirstOrDefault() ?? ClientSettings.DefaultTtsVoice;
                clientContext.CustomVoiceEndpointId = Request.Headers["CustomVoiceEndpointId"].FirstOrDefault();
                clientContext.PersonalVoiceSpeakerProfileId = Request.Headers["PersonalVoiceSpeakerProfileId"].FirstOrDefault();

                var customVoiceEndpointId = clientContext.CustomVoiceEndpointId;

                SpeechConfig speechConfig;
                if (!string.IsNullOrEmpty(_clientSettings.SpeechPrivateEndpoint))
                {
                    var speechPrivateEndpointWss = _clientSettings.SpeechPrivateEndpoint.Replace("https://", "wss://");
                    speechConfig = SpeechConfig.FromEndpoint(new Uri($"{speechPrivateEndpointWss}/tts/cognitiveservices/websocket/v1?enableTalkingAvatar=true"), _clientSettings.SpeechKey);
                }
                else
                {
                    string endpointUrl = $"wss://{_clientSettings.SpeechRegion}.tts.speech.microsoft.com/tts/cognitiveservices/websocket/v1?enableTalkingAvatar=true";
                    speechConfig = SpeechConfig.FromEndpoint(new Uri(endpointUrl), _clientSettings.SpeechKey);
                }

                if (!string.IsNullOrEmpty(customVoiceEndpointId))
                {
                    speechConfig.EndpointId = customVoiceEndpointId;
                }

                var speechSynthesizer = new SpeechSynthesizer(speechConfig, null);
                clientContext.SpeechSynthesizer = speechSynthesizer;

                if (ClientSettings.EnableAudioAudit)
                {
                    speechSynthesizer.Synthesizing += (o, e) =>
                    {
                        Console.WriteLine($"Audio chunk received: {e.Result.AudioData.Length} bytes.");
                    };
                }

                if (string.IsNullOrEmpty(GlobalVariables.IceToken))
                {
                    return BadRequest("IceToken is missing or invalid.");
                }

                var iceTokenObj = JsonConvert.DeserializeObject<Dictionary<string, object>>(GlobalVariables.IceToken);

                // Apply customized ICE server if provided
                if (!string.IsNullOrEmpty(_clientSettings.IceServerUrl) && !string.IsNullOrEmpty(_clientSettings.IceServerUsername) && !string.IsNullOrEmpty(_clientSettings.IceServerPassword))
                {
                    iceTokenObj = new Dictionary<string, object>
                    {
                        { "Urls", string.IsNullOrEmpty(_clientSettings.IceServerUrlRemote) ? new JArray(_clientSettings.IceServerUrl) : new JArray(_clientSettings.IceServerUrlRemote) },
                        { "Username", _clientSettings.IceServerUsername },
                        { "Password", _clientSettings.IceServerPassword }
                    };
                }

                string localSdp;
                using (var reader = new StreamReader(Request.Body, Encoding.UTF8))
                {
                    localSdp = await reader.ReadToEndAsync();
                }

                var avatarCharacter = Request.Headers["AvatarCharacter"].FirstOrDefault();
                var avatarStyle = Request.Headers["AvatarStyle"].FirstOrDefault();
                var backgroundColor = Request.Headers["BackgroundColor"].FirstOrDefault() ?? "#FFFFFFFF";
                var backgroundImageUrl = Request.Headers["BackgroundImageUrl"].FirstOrDefault();
                var isCustomAvatar = Request.Headers["IsCustomAvatar"].FirstOrDefault();
                var transparentBackground = Request.Headers["TransparentBackground"].FirstOrDefault() ?? "false";
                var videoCrop = Request.Headers["VideoCrop"].FirstOrDefault() ?? "false";

                // Configure avatar settings
                var urlsArray = iceTokenObj?.TryGetValue("Urls", out var value) == true ? value as JArray : null;

                var firstUrl = urlsArray?.FirstOrDefault()?.ToString();

                var avatarConfig = new
                {
                    synthesis = new
                    {
                        video = new
                        {
                            protocol = new
                            {
                                name = "WebRTC",
                                webrtcConfig = new
                                {
                                    clientDescription = localSdp,
                                    iceServers = new[]
                                    {
                                        new
                                        {
                                            urls = new[] { firstUrl },
                                            username = iceTokenObj!["Username"],
                                            credential = iceTokenObj["Password"]
                                        }
                                    },
                                    auditAudio = ClientSettings.EnableAudioAudit
                                }
                            },
                            format = new
                            {
                                crop = new
                                {
                                    topLeft = new
                                    {
                                        x = videoCrop.ToLower() == "true" ? 600 : 0,
                                        y = 0
                                    },
                                    bottomRight = new
                                    {
                                        x = videoCrop.ToLower() == "true" ? 1320 : 1920,
                                        y = 1080
                                    }
                                },
                                bitrate = 1000000
                            },
                            talkingAvatar = new
                            {
                                customized = (isCustomAvatar?.ToLower() ?? "false") == "true",
                                character = avatarCharacter,
                                style = avatarStyle,
                                background = new
                                {
                                    color = transparentBackground.ToLower() == "true" ? "#00FF00FF" : backgroundColor,
                                    image = new
                                    {
                                        url = backgroundImageUrl
                                    }
                                }
                            }
                        }
                    }
                };

                var connection = Connection.FromSpeechSynthesizer(speechSynthesizer);
                connection.SetMessageProperty("speech.config", "context", JsonConvert.SerializeObject(avatarConfig));
                connection.Connected += (sender, args) =>
                {
                    Console.WriteLine("TTS Avatar service connected.");
                };

                connection.Disconnected += (sender, args) =>
                {
                    Console.WriteLine("TTS Avatar service disconnected.");
                    clientContext.SpeechSynthesizerConnection = null;
                    clientContext.SpeechSynthesizerConnected = false;
                };

                clientContext.SpeechSynthesizerConnection = connection;
                clientContext.SpeechSynthesizerConnected = true;

                var speechSynthesisResult = speechSynthesizer.SpeakTextAsync("").Result;
                Console.WriteLine($"Result ID: {speechSynthesisResult.ResultId}");
                if (speechSynthesisResult.Reason == ResultReason.Canceled)
                {
                    var cancellationDetails = SpeechSynthesisCancellationDetails.FromResult(speechSynthesisResult);
                    throw new Exception(cancellationDetails.ErrorDetails);
                }

                var turnStartMessage = speechSynthesizer.Properties.
                    GetProperty("SpeechSDKInternal-ExtraTurnStartMessage");
                var turnStartMessageJson = JsonConvert.DeserializeObject<JObject>(turnStartMessage);
                var connectionStringToken = turnStartMessageJson?["webrtc"]?["connectionString"];
                var remoteSdp = connectionStringToken?.ToString() ?? string.Empty;
                return Content(remoteSdp, "application/json");
            }
            catch (Exception ex)
            {
                return BadRequest(new { message = ex.Message });
            }
        }

        [HttpPost("api/speak")]
        public async Task<IActionResult> Speak()
        {
            try
            {
                // Extract the client ID from the request headers
                var clientIdHeader = Request.Headers["ClientId"];
                if (!Guid.TryParse(clientIdHeader, out Guid clientId))
                {
                    return BadRequest("Invalid ClientId");
                }

                // Read the SSML data from the request body
                string ssml;
                using (var reader = new StreamReader(Request.Body, Encoding.UTF8))
                {
                    ssml = await reader.ReadToEndAsync();
                }

                // Call your method to process the SSML
                string resultId = await SpeakSsml(ssml, clientId).ConfigureAwait(false);
                return Content(resultId, "text/plain");
            }
            catch (Exception ex)
            {
                return BadRequest($"Speak failed. Error message: {ex.Message}");
            }
        }

        [HttpPost("api/stopSpeaking")]
        public async Task<IActionResult> StopSpeaking()
        {
            try
            {
                // Extract the client ID from the request headers
                var clientIdHeader = Request.Headers["ClientId"];
                if (!Guid.TryParse(clientIdHeader, out Guid clientId))
                {
                    return BadRequest("Invalid ClientId");
                }

                var clientContext = _clientService.GetClientContext(clientId);
                await StopSpeakingInternal(clientId, false);

                // Return a success message
                return Ok("Speaking stopped.");
            }
            catch (Exception ex)
            {
                return StatusCode(StatusCodes.Status500InternalServerError, $"Error: {ex.Message}");
            }
        }

        [HttpPost("api/chat")]
        public async Task<IActionResult> Chat()
        {
            // Retrieve and parse the ClientId from headers
            var clientIdHeaderValues = Request.Headers["ClientId"];
            var clientId = clientIdHeaderValues.FirstOrDefault();
            if (string.IsNullOrEmpty(clientId) || !Guid.TryParse(clientId, out var clientGuid))
            {
                // Handle missing or invalid ClientId
                return BadRequest("Invalid or missing ClientId.");
            }

            // Retrieve the SystemPrompt from headers (optional)
            var systemPromptHeaderValues = Request.Headers["SystemPrompt"];
            string systemPrompt = systemPromptHeaderValues.FirstOrDefault() ?? string.Empty;

            // Read the user query from the request body
            string userQuery;
            using (var reader = new StreamReader(Request.Body, Encoding.UTF8))
            {
                userQuery = await reader.ReadToEndAsync();
            }

            var clientContext = _clientService.GetClientContext(clientGuid);

            if (!clientContext.ChatInitiated)
            {
                InitializeChatContext(systemPrompt, clientGuid);
                clientContext.ChatInitiated = true;
            }

            await HandleUserQuery(userQuery, clientGuid, Response);

            return new EmptyResult();
        }

        // The API route to continue speaking the unfinished sentences
        [HttpPost("api/chat/continueSpeaking")]
        public IActionResult ContinueSpeaking()
        {
            try
            {
                // Extract the client ID from the request headers
                var clientIdHeader = Request.Headers["ClientId"];
                if (!Guid.TryParse(clientIdHeader, out Guid clientId))
                {
                    return BadRequest("Invalid ClientId");
                }

                // Retrieve the client context
                var clientContext = _clientService.GetClientContext(clientId);
                var spokenTextQueue = clientContext.SpokenTextQueue;
                var speakingText = clientContext.SpeakingText;

                if (spokenTextQueue == null)
                {
                    return StatusCode(500, "Internal error: spoken_text_queue is missing");
                }

                if (!string.IsNullOrEmpty(speakingText) && ClientSettings.RepeatSpeakingSentenceAfterReconnection)
                {
                    spokenTextQueue.AddFirst(speakingText);
                }

                if (spokenTextQueue.Count > 0)
                {
                    SpeakWithQueue(null, 0, clientId, null);
                }

                return Ok("Request sent.");
            }
            catch (Exception ex)
            {
                return StatusCode(StatusCodes.Status500InternalServerError, $"Error: {ex.Message}");
            }
        }

        [HttpPost("api/chat/clearHistory")]
        public IActionResult ClearChatHistory()
        {
            try
            {
                // Extract the client ID from the request headers
                var clientIdHeader = Request.Headers["ClientId"];
                if (!Guid.TryParse(clientIdHeader, out Guid clientId))
                {
                    return BadRequest("Invalid ClientId");
                }

                // Retrieve the client context and clear chat history
                var clientContext = _clientService.GetClientContext(clientId);
                var systemPrompt = Request.Headers["SystemPrompt"].FirstOrDefault() ?? string.Empty;
                InitializeChatContext(systemPrompt, clientId);
                clientContext.ChatInitiated = true;

                return Ok("Chat history cleared.");
            }
            catch (Exception ex)
            {
                return StatusCode(StatusCodes.Status500InternalServerError, $"Error: {ex.Message}");
            }
        }

        [HttpPost("api/disconnectAvatar")]
        public async Task<IActionResult> DisconnectAvatar()
        {
            try
            {
                // Extract the client ID from the request headers
                var clientIdHeader = Request.Headers["ClientId"];
                if (!Guid.TryParse(clientIdHeader, out Guid clientId))
                {
                    return BadRequest("Invalid ClientId");
                }

                await DisconnectAvatarInternal(clientId, false);
                return Ok("Disconnected avatar");
            }
            catch (Exception ex)
            {
                return BadRequest($"Error: {ex.Message}");
            }
        }

        [HttpGet("api/initializeClient")]
        public IActionResult InitializeClient()
        {
            try
            {
                var clientId = _clientService.InitializeClient();
                return Ok(new { ClientId = clientId });
            }
            catch (Exception ex)
            {
                return StatusCode(StatusCodes.Status500InternalServerError, $"Error: {ex.Message}");
            }
        }

        [HttpPost("api/releaseClient")]
        public async Task<IActionResult> ReleaseClient()
        {
            // Extract the client ID from the request body
            var clientIdString = string.Empty;
            using (var reader = new StreamReader(Request.Body, Encoding.UTF8))
            {
                clientIdString = JObject.Parse(await reader.ReadToEndAsync()).Value<string>("clientId");
            }

            if (!Guid.TryParse(clientIdString, out Guid clientId))
            {
                return BadRequest("Invalid ClientId");
            }

            try
            {
                await DisconnectAvatarInternal(clientId, false);
                await Task.Delay(2000); // Wait some time for the connection to close
                _clientService.RemoveClient(clientId);
                Console.WriteLine($"Client context released for client id {clientId}.");
                return Ok($"Client context released for client id {clientId}.");
            }
            catch (Exception ex)
            {
                return BadRequest($"Client context release failed for client id {clientId}. Error message: {ex.Message}");
            }
        }

        private async Task HandleUserQuery(string userQuery, Guid clientId, HttpResponse httpResponse)
        {
            var clientContext = _clientService.GetClientContext(clientId);
            var azureOpenaiDeploymentName = clientContext.AzureOpenAIDeploymentName;
            var messages = clientContext.Messages;

            var chatMessage = new UserChatMessage(userQuery);
            messages.Add(chatMessage);

            // For 'on your data' scenario, chat API currently has long (4s+) latency
            // We return some quick reply here before the chat API returns to mitigate.
            if (ClientSettings.EnableQuickReply)
            {
                await SpeakWithQueue(ClientSettings.QuickReplies[new Random().Next(ClientSettings.QuickReplies.Count)], 2000, clientId, httpResponse);
            }

            // Process the responseContent as needed
            var assistantReply = new StringBuilder();
            var spokenSentence = new StringBuilder();

            if (chatClient == null)
            {
                // Skip if the chat client is not ready
                return;
            }

            var chatOptions = new ChatCompletionOptions();
            if (!string.IsNullOrEmpty(_clientSettings.CognitiveSearchEndpoint) &&
                !string.IsNullOrEmpty(_clientSettings.CognitiveSearchIndexName) &&
                !string.IsNullOrEmpty(_clientSettings.CognitiveSearchAPIKey))
            {
#pragma warning disable AOAI001 // Type is for evaluation purposes only and is subject to change or removal in future updates. Suppress this diagnostic to proceed.
                chatOptions.AddDataSource(new AzureSearchChatDataSource()
                {
                    Endpoint = new Uri(_clientSettings.CognitiveSearchEndpoint),
                    IndexName = _clientSettings.CognitiveSearchIndexName,
                    Authentication = DataSourceAuthentication.FromApiKey(_clientSettings.CognitiveSearchAPIKey)
                });
#pragma warning restore AOAI001 // Type is for evaluation purposes only and is subject to change or removal in future updates. Suppress this diagnostic to proceed.
            }

            var aoaiStartTime = DateTime.Now;
            var chatUpdates = chatClient.CompleteChatStreaming(messages, chatOptions);

            var isFirstChunk = true;
            var isFirstSentence = true;
            foreach (var chatUpdate in chatUpdates)
            {
                foreach (var contentPart in chatUpdate.ContentUpdate)
                {
                    var responseToken = contentPart.Text;
                    if (isFirstChunk)
                    {
                        var aoaiFirstTokenLatency = (int)(DateTime.Now.Subtract(aoaiStartTime).TotalMilliseconds + 0.5);
                        Console.WriteLine($"AOAI first token latency: {aoaiFirstTokenLatency}ms");
                        await httpResponse.WriteAsync($"<FTL>{aoaiFirstTokenLatency}</FTL>");
                        isFirstChunk = false;
                    }

                    if (ClientSettings.OydDocRegex.IsMatch(responseToken))
                    {
                        responseToken = ClientSettings.OydDocRegex.Replace(responseToken, string.Empty);
                    }

                    if (!ClientSettings.EnableDisplayTextAlignmentWithSpeech)
                    {
                        await httpResponse.WriteAsync(responseToken).ConfigureAwait(false);
                    }

                    assistantReply.Append(responseToken);
                    spokenSentence.Append(responseToken); // build up the spoken sentence
                    if (responseToken == "\n" || responseToken == "\n\n")
                    {
                        if (isFirstSentence)
                        {
                            var aoaiFirstSentenceLatency = (int)(DateTime.Now.Subtract(aoaiStartTime).TotalMilliseconds + 0.5);
                            Console.WriteLine($"AOAI first sentence latency: {aoaiFirstSentenceLatency}ms");
                            await httpResponse.WriteAsync($"<FSL>{aoaiFirstSentenceLatency}</FSL>");
                            isFirstSentence = false;
                        }

                        await SpeakWithQueue(spokenSentence.ToString(), 0, clientId, httpResponse);
                        spokenSentence.Clear();
                    }
                    else
                    {
                        responseToken = responseToken.Replace("\n", string.Empty);
                        if (responseToken.Length == 1 || responseToken.Length == 2)
                        {
                            foreach (var punctuation in ClientSettings.SentenceLevelPunctuations)
                            {
                                if (responseToken.StartsWith(punctuation))
                                {
                                    if (isFirstSentence)
                                    {
                                        var aoaiFirstSentenceLatency = (int)(DateTime.Now.Subtract(aoaiStartTime).TotalMilliseconds + 0.5);
                                        Console.WriteLine($"AOAI first sentence latency: {aoaiFirstSentenceLatency}ms");
                                        await httpResponse.WriteAsync($"<FSL>{aoaiFirstSentenceLatency}</FSL>");
                                        isFirstSentence = false;
                                    }

                                    await SpeakWithQueue(spokenSentence.ToString(), 0, clientId, httpResponse);
                                    spokenSentence.Clear();
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if (spokenSentence.Length > 0)
            {
                await SpeakWithQueue(spokenSentence.ToString(), 0, clientId, httpResponse);
            }

            var assistantMessage = new AssistantChatMessage(assistantReply.ToString());
            messages.Add(assistantMessage);

            if (ClientSettings.EnableDisplayTextAlignmentWithSpeech)
            {
                while (clientContext.SpokenTextQueue.Count > 0)
                {
                    await Task.Delay(200);
                }

                await Task.Delay(200);
            }
        }

        public void InitializeChatContext(string systemPrompt, Guid clientId)
        {
            var clientContext = _clientService.GetClientContext(clientId);
            var messages = clientContext.Messages;

            // Initialize messages
            messages.Clear();
            var systemMessage = new SystemChatMessage(systemPrompt);
            messages.Add(systemMessage);
        }

        // Speak the given text. If there is already a speaking in progress, add the text to the queue. For chat scenario.
        private Task SpeakWithQueue(string text, int endingSilenceMs, Guid clientId, HttpResponse httpResponse)
        {
            var clientContext = _clientService.GetClientContext(clientId);

            var spokenTextQueue = clientContext.SpokenTextQueue;
            var isSpeaking = clientContext.IsSpeaking;

            spokenTextQueue.AddLast(text);

            if (!isSpeaking)
            {
                clientContext.IsSpeaking = true;

                var ttsVoice = clientContext.TtsVoice;
                var personalVoiceSpeakerProfileId = clientContext.PersonalVoiceSpeakerProfileId;

                _ = Task.Run(async () =>
                {
                    try
                    {
                        while (spokenTextQueue?.Count > 0)
                        {
                            var currentText = spokenTextQueue?.First?.Value;
                            clientContext.SpeakingText = currentText;
                            spokenTextQueue?.RemoveFirst();
                            if (ClientSettings.EnableDisplayTextAlignmentWithSpeech)
                            {
                                httpResponse.WriteAsync(currentText);
                            }

                            await SpeakText(currentText, ttsVoice!, personalVoiceSpeakerProfileId!, endingSilenceMs, clientId);
                            clientContext.LastSpeakTime = DateTime.UtcNow;
                        }
                    }
                    finally
                    {
                        clientContext.IsSpeaking = false;
                        clientContext.SpeakingText = null;
                    }
                });
            }

            return Task.CompletedTask;
        }

        private async Task<string> SpeakText(string text, string voice, string speakerProfileId, int endingSilenceMs, Guid clientId)
        {
            var escapedText = HttpUtility.HtmlEncode(text);
            string ssml;

            if (endingSilenceMs > 0)
            {
                ssml = $@"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>
                        <voice name='{voice}'>
                            <mstts:ttsembedding speakerProfileId='{speakerProfileId}'>
                                <mstts:leadingsilence-exact value='0'/>
                                {escapedText}
                                <break time='{endingSilenceMs}ms' />
                            </mstts:ttsembedding>
                        </voice>
                      </speak>";
            }
            else
            {
                ssml = $@"<speak version='1.0' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts' xml:lang='en-US'>
                        <voice name='{voice}'>
                            <mstts:ttsembedding speakerProfileId='{speakerProfileId}'>
                                <mstts:leadingsilence-exact value='0'/>
                                {escapedText}
                            </mstts:ttsembedding>
                        </voice>
                      </speak>";
            }

            return await SpeakSsml(ssml, clientId);
        }

        private async Task<string> SpeakSsml(string ssml, Guid clientId)
        {
            var clientContext = _clientService.GetClientContext(clientId);

            var speechSynthesizer = clientContext.SpeechSynthesizer as SpeechSynthesizer;
            if (speechSynthesizer == null)
            {
                throw new InvalidOperationException("SpeechSynthesizer is not of type SpeechSynthesizer.");
            }

            var speechSynthesisResult = await speechSynthesizer.SpeakSsmlAsync(ssml).ConfigureAwait(false);

            if (speechSynthesisResult.Reason == ResultReason.Canceled)
            {
                var cancellationDetails = SpeechSynthesisCancellationDetails.FromResult(speechSynthesisResult);
                Console.WriteLine($"Speech synthesis canceled: {cancellationDetails.Reason}");

                if (cancellationDetails.Reason == CancellationReason.Error)
                {
                    Console.WriteLine($"Result ID: {speechSynthesisResult.ResultId}. Error details: {cancellationDetails.ErrorDetails}");
                    throw new Exception(cancellationDetails.ErrorDetails);
                }
            }

            return speechSynthesisResult.ResultId;
        }

        private async Task StopSpeakingInternal(Guid clientId, bool skipClearingSpokenTextQueue)
        {
            var clientContext = _clientService.GetClientContext(clientId);
            clientContext.IsSpeaking = false;
            if (!skipClearingSpokenTextQueue)
            {
                var spokenTextQueue = clientContext.SpokenTextQueue;
                spokenTextQueue.Clear();
            }

            try
            {
                var connection = clientContext.SpeechSynthesizerConnection as Connection;
                if (connection != null)
                {
                    await connection.SendMessageAsync("synthesis.control", "{\"action\":\"stop\"}");
                    Console.WriteLine("Stop speaking message sent.");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }

        private async Task DisconnectAvatarInternal(Guid clientId, bool isReconnecting)
        {
            // Retrieve the client context
            var clientContext = _clientService.GetClientContext(clientId);

            if (clientContext == null)
            {
                throw new Exception("Client context not found");
            }

            await StopSpeakingInternal(clientId, isReconnecting);
            await Task.Delay(2000); // Wait for the last speech to finish

            var connection = clientContext.SpeechSynthesizerConnection as Connection;
            if (connection != null)
            {
                connection.Close();
            }
        }
    }
}