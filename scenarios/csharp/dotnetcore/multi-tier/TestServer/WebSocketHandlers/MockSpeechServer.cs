using Azure.AI.Test.Common.Logging;
using Azure.AI.Test.Common.Servers.USP;
using Azure.AI.Test.Common.Servers.WebSocketHandlers;
using Azure.AI.Test.Common.Servers;
using System.Net.WebSockets;
using Azure.AI.Test.Common.Servers.WebSocketHandlers.Interactive;

namespace Azure.AI.Test.Common.Servers
{
    public class MockSpeechServer : IDisposable
    {
        private const string wsPath = "/ws/callback";

        private InteractiveWebSocketHandler wsHandler;
        private IWebSocketHelper wsHelper;
        private TestServer testServer;
        private string currentRequestId;
        private bool disposed = false;

        private MockSpeechServer()
        {
            wsHandler = new InteractiveWebSocketHandler();

            var testServerConfig = new TestServerConfig();
            testServerConfig.AddWebSocketRoute(wsPath, () => wsHandler);

            testServer = TestServerFactory.CreateTestServer(System.Diagnostics.Debugger.IsAttached ? SystemDebugLogger.Instance : VoidLogger.Instance, testServerConfig);

            var serverUri = testServer.WebSocketEndpoint;
            var callbackUri = new Uri(serverUri, wsPath);
            EndpointUri = callbackUri;

            wsHandler.MessageReceived += async (args) =>
            {
                if (args.UspMessage.Header.IsPath("speech.context"))
                {
                    currentRequestId = args.UspMessage.Header.RequestId;
                }

                var mrTask = MessageReceived?.Invoke(args.UspMessage);
                if (null != mrTask)
                {
                    await mrTask;
                }
            };

            wsHandler.Opened += (args) =>
            {
                currentRequestId = null;
                wsHelper = args.WebSocketHelper;
                return Task.CompletedTask;
            };

        }

        public Uri EndpointUri { get; }

        public static MockSpeechServer Start()
        {
            return new MockSpeechServer();
        }

        public delegate Task MessageReceivedHandler(Message message);

        public event MessageReceivedHandler MessageReceived;

        public Task SendTurnStartMessageAsync(CancellationToken cancellationToken = default)
        {
            var turnStartMessage = new TextMessage();
            turnStartMessage.Header.Path = "turn.start";
            turnStartMessage.Header.RequestId = currentRequestId;
            turnStartMessage.Header.ContentType = "application/json";
            turnStartMessage.Data = "{}";

            return wsHelper.SendAsync(turnStartMessage, cancellationToken);
        }

        public Task SendSpeechStartMessageAsync(TimeSpan offset, CancellationToken cancellationToken = default)
        {
            var speechStartMessage = new TextMessage();
            speechStartMessage.Header.Path = "speech.startDetected";
            speechStartMessage.Header.RequestId = currentRequestId;
            speechStartMessage.Header.ContentType = "application/json";
            speechStartMessage.Data = $"{{\"Offset\":{offset.Ticks}";

            return wsHelper.SendAsync(speechStartMessage, cancellationToken);
        }

        public Task SendRecognizingMessageAsync(string text = "recognizing", TimeSpan offset = default, TimeSpan duration = default, CancellationToken cancellationToken = default)
        {
            var recognizingMessage = new TextMessage();
            recognizingMessage.Header.Path = "speech.hypothesis";
            recognizingMessage.Header.RequestId = currentRequestId;
            recognizingMessage.Header.ContentType = "application/json";
            recognizingMessage.Data = $"{{\"Text\":\"{text}\",\"Offset\":{offset.Ticks},\"Duration\":{duration.Ticks}}}";

            return wsHelper.SendAsync(recognizingMessage, cancellationToken);
        }

        public Task SendRecognizedMessageAsync(string text = "recognized", TimeSpan offset = default, TimeSpan duration = default, CancellationToken cancellationToken = default)
        {
            var recognizedMessage = new TextMessage();
            recognizedMessage.Header.Path = "speech.phrase";
            recognizedMessage.Header.RequestId = currentRequestId;
            recognizedMessage.Header.ContentType = "application/json";
            recognizedMessage.Data = $"{{\"RecognitionStatus\":\"Success\",\"Offset\":{offset.Ticks},\"Duration\":{duration.Ticks},\"DisplayText\":\"{text}\"}}";

            return wsHelper.SendAsync(recognizedMessage, cancellationToken);
        }

        public Task SendSpeechEndMessageAsync(TimeSpan offset = default, CancellationToken cancellationToken = default)
        {
            var speechEndMessage = new TextMessage();
            speechEndMessage.Header.Path = "speech.endDetected";
            speechEndMessage.Header.RequestId = currentRequestId;
            speechEndMessage.Header.ContentType = "application/json";
            speechEndMessage.Data = $"{{\"Offset\":{offset.Ticks}}}";

            return wsHelper.SendAsync(speechEndMessage, cancellationToken);
        }

        public Task SendDictationEndMessageAsync(TimeSpan offset = default, CancellationToken cancellationToken = default)
        {
            var speechEndMessage2 = new TextMessage();
            speechEndMessage2.Header.Path = "speech.phrase";
            speechEndMessage2.Header.RequestId = currentRequestId;
            speechEndMessage2.Header.ContentType = "application/json";
            speechEndMessage2.Data = $"{{\"RecognitionStatus\":\"EndOfDictation\",\"Offset\":{offset.Ticks},\"Duration\":0}}";

            return wsHelper.SendAsync(speechEndMessage2, cancellationToken);
        }

        public Task SendTurnEndMessageAsync(CancellationToken cancellationToken = default)
        {
            var turnEndMessage = new TextMessage();
            turnEndMessage.Header.Path = "turn.end";
            turnEndMessage.Header.RequestId = currentRequestId;
            turnEndMessage.Header.ContentType = "application/json";
            turnEndMessage.Data = "{\"Instrumentation\": {}}";

            return wsHelper.SendAsync(turnEndMessage, cancellationToken);
        }

        public async Task CloseWebSocketAsync(WebSocketCloseStatus status, string statusMessage = "Closing", CancellationToken cancellationToken = default)
        {
            if (disposed)
            {
                return;
            }

            if (wsHelper.WebSocket.State == WebSocketState.Open)
            {
                await wsHelper.WebSocket.CloseAsync(status, statusMessage, cancellationToken);
            }
            else
            {
                return;
            }
        }

        public void Dispose()
        {
            disposed = true;
            if (!disposed)
            {
                ((IDisposable)testServer).Dispose();
            }
        }
    }
}
