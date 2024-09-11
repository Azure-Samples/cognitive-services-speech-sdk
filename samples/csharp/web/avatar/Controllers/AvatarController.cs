//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Avatar.Models;
using Avatar.Services;
using Azure;
using Azure.AI.OpenAI;
using Azure.AI.OpenAI.Chat;
using Microsoft.AspNetCore.Mvc;
using Microsoft.CognitiveServices.Speech;
using Microsoft.Extensions.Options;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using OpenAI.Chat;
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
        public ActionResult ChatView()
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
                        new AzureKeyCredential(_clientSettings.AzureOpenAIAPIKey));
                    chatClient = aoaiClient.GetChatClient(_clientSettings.AzureOpenAIDeploymentName);
                }
            }

            return View("Chat", clientId);
        }

        [HttpGet("api/getSpeechToken")]
        public IActionResult GetSpeechToken()
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

        [HttpPost("api/connectAvatar")]
        public async Task<IActionResult> ConnectAvatar()
        {
            try
            {
                var clientId = new Guid(Request.Headers["ClientId"]!);

                var clientContext = _clientService.GetClientContext(clientId);

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
                    speechConfig = SpeechConfig.FromSubscription($"{speechPrivateEndpointWss}/tts/cognitiveservices/websocket/v1?enableTalkingAvatar=true", _clientSettings.SpeechKey);
                }
                else
                {
                    string endpointUrl = $"wss://{_clientSettings.SpeechRegion}.tts.speech.microsoft.com/cognitiveservices/websocket/v1?enableTalkingAvatar=true";
                    speechConfig = SpeechConfig.FromEndpoint(new Uri(endpointUrl), _clientSettings.SpeechKey);
                }

                if (!string.IsNullOrEmpty(customVoiceEndpointId))
                {
                    speechConfig.EndpointId = customVoiceEndpointId;
                }

                var speechSynthesizer = new SpeechSynthesizer(speechConfig);
                clientContext.SpeechSynthesizer = speechSynthesizer;

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
                        { "Urls", string.IsNullOrEmpty(_clientSettings.IceServerUrlRemote) ? [_clientSettings.IceServerUrl] : new[] { _clientSettings.IceServerUrlRemote } },
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
                var urlsArray = iceTokenObj?.TryGetValue("Urls", out var value) == true ? value as string[] : null;

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
                                    }
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
        public async Task<ActionResult> Speak()
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
                string resultId = await SpeakSsml(ssml, clientId);
                return Content(resultId, "text/plain");
            }
            catch (Exception ex)
            {
                return BadRequest($"Speak failed. Error message: {ex.Message}");
            }
        }

        [HttpGet]
        [Route("api/getSpeakingStatus")]
        public ActionResult GetSpeakingStatus()
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

                // Get speaking status
                var speakingStatus = new
                {
                    isSpeaking = clientContext.IsSpeaking,
                    lastSpeakTime = clientContext.LastSpeakTime?.ToString("o") // ISO 8601 format
                };

                // Return the status as JSON
                return Ok(speakingStatus);
            }
            catch (Exception ex)
            {
                return StatusCode(StatusCodes.Status500InternalServerError, $"Error: {ex.Message}");
            }
        }

        [HttpPost("api/stopSpeaking")]
        public async Task<ActionResult> StopSpeaking()
        {
            try
            {
                // Extract the client ID from the request headers
                var clientIdHeader = Request.Headers["ClientId"];
                if (!Guid.TryParse(clientIdHeader, out Guid clientId))
                {
                    return BadRequest("Invalid ClientId");
                }

                // Call the internal method to stop speaking
                await StopSpeakingInternal(clientId);

                // Return a success message
                return Ok("Speaking stopped.");
            }
            catch (Exception ex)
            {
                return StatusCode(StatusCodes.Status500InternalServerError, $"Error: {ex.Message}");
            }
        }

        [HttpPost("api/chat")]
        public async Task<ActionResult> Chat()
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

            return new OkResult();
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
        public IActionResult DisconnectAvatar()
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

                if (clientContext == null)
                {
                    return StatusCode(StatusCodes.Status204NoContent, "Client context not found");
                }

                var speechSynthesizer = clientContext.SpeechSynthesizer as SpeechSynthesizer;
                if (speechSynthesizer != null)
                {
                    var connection = Connection.FromSpeechSynthesizer(speechSynthesizer);
                    connection.Close();
                }

                return Ok("Disconnected avatar");
            }
            catch (Exception ex)
            {
                return BadRequest($"Error: {ex.Message}");
            }
        }

        [HttpGet("api/initializeClient")]
        public ActionResult InitializeClient()
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

        public async Task HandleUserQuery(string userQuery, Guid clientId, HttpResponse httpResponse)
        {
            var clientContext = _clientService.GetClientContext(clientId);
            var azureOpenaiDeploymentName = clientContext.AzureOpenAIDeploymentName;
            var messages = clientContext.Messages;
            var isSpeaking = clientContext.IsSpeaking;
            var httpClient = new HttpClient();

            var chatMessage = new UserChatMessage(userQuery);
            messages.Add(chatMessage);

            // Stop previous speaking if there is any
            if (isSpeaking)
            {
                await StopSpeakingInternal(clientId);
            }

            // For 'on your data' scenario, chat API currently has long (4s+) latency
            // We return some quick reply here before the chat API returns to mitigate.
            if (ClientSettings.EnableQuickReply)
            {
                await SpeakWithQueue(ClientSettings.QuickReplies[new Random().Next(ClientSettings.QuickReplies.Count)], 2000, clientId);
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
            if (_clientSettings.CognitiveSearchEndpoint != null &&
                _clientSettings.CognitiveSearchIndexName != null &&
                _clientSettings.CognitiveSearchAPIKey != null)
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

            var chatUpdates = chatClient.CompleteChatStreaming(messages, chatOptions);

            foreach (var chatUpdate in chatUpdates)
            {
                foreach (var contentPart in chatUpdate.ContentUpdate)
                {
                    var responseToken = contentPart.Text;
                    if (ClientSettings.OydDocRegex.IsMatch(responseToken))
                    {
                        responseToken = ClientSettings.OydDocRegex.Replace(responseToken, string.Empty);
                    }

                    await httpResponse.WriteAsync(responseToken).ConfigureAwait(false);

                    assistantReply.Append(responseToken);
                    if (responseToken == "\n" || responseToken == "\n\n")
                    {
                        await SpeakWithQueue(spokenSentence.ToString().Trim(), 0, clientId);
                        spokenSentence.Clear();
                    }
                    else
                    {
                        responseToken = responseToken.Replace("\n", string.Empty);
                        spokenSentence.Append(responseToken); // build up the spoken sentence
                        if (responseToken.Length == 1 || responseToken.Length == 2)
                        {
                            foreach (var punctuation in ClientSettings.SentenceLevelPunctuations)
                            {
                                if (responseToken.StartsWith(punctuation))
                                {
                                    await SpeakWithQueue(spokenSentence.ToString().Trim(), 0, clientId);
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
                await SpeakWithQueue(spokenSentence.ToString().Trim(), 0, clientId);
            }

            var assistantMessage = new AssistantChatMessage(assistantReply.ToString());
            messages.Add(assistantMessage);
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
        public Task SpeakWithQueue(string text, int endingSilenceMs, Guid clientId)
        {
            var clientContext = _clientService.GetClientContext(clientId);

            var spokenTextQueue = clientContext.SpokenTextQueue;
            var isSpeaking = clientContext.IsSpeaking;

            spokenTextQueue.Enqueue(text);

            if (!isSpeaking)
            {
                clientContext.IsSpeaking = true;

                var ttsVoice = clientContext.TtsVoice;
                var personalVoiceSpeakerProfileId = clientContext.PersonalVoiceSpeakerProfileId;

                _ = Task.Run(async () =>
                {
                    try
                    {
                        while (spokenTextQueue.Count > 0)
                        {
                            var currentText = spokenTextQueue.Dequeue();
                            await SpeakText(currentText, ttsVoice!, personalVoiceSpeakerProfileId!, endingSilenceMs, clientId);
                            clientContext.LastSpeakTime = DateTime.UtcNow;
                        }
                    }
                    finally
                    {
                        clientContext.IsSpeaking = false;
                    }
                });
            }

            return Task.CompletedTask;
        }

        public async Task<string> SpeakText(string text, string voice, string speakerProfileId, int endingSilenceMs, Guid clientId)
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

        public async Task<string> SpeakSsml(string ssml, Guid clientId)
        {
            var clientContext = _clientService.GetClientContext(clientId);

            var speechSynthesizer = clientContext.SpeechSynthesizer as SpeechSynthesizer;
            if (speechSynthesizer == null)
            {
                throw new InvalidOperationException("SpeechSynthesizer is not of type SpeechSynthesizer.");
            }
            var speechSynthesisResult = await Task.Run(() => speechSynthesizer.SpeakSsmlAsync(ssml));

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

        public async Task StopSpeakingInternal(Guid clientId)
        {
            var clientContext = _clientService.GetClientContext(clientId);

            var speechSynthesizer = clientContext.SpeechSynthesizer as SpeechSynthesizer;
            var spokenTextQueue = clientContext.SpokenTextQueue;
            spokenTextQueue.Clear();

            try
            {
                var connection = Connection.FromSpeechSynthesizer(speechSynthesizer);
                await connection.SendMessageAsync("synthesis.control", "{\"action\":\"stop\"}");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}