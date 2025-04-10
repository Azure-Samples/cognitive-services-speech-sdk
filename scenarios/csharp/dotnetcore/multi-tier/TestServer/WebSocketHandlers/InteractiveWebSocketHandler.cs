//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.ComponentModel;
using System.Linq.Expressions;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Azure.AI.Test.Common.Logging;
using Azure.AI.Test.Common.Servers.USP;
using Azure.AI.Test.Common.Servers.WebSocketHandlers.Interactive;

namespace Azure.AI.Test.Common.Servers.WebSocketHandlers
{
    /// <summary>
    /// A web socket handler that raises events for all received messages
    /// </summary>
    public class InteractiveWebSocketHandler : WebSocketHandlerBase
    {
        private readonly int _receiveBufferSize;
        private readonly Encoding _encoding;

        public InteractiveWebSocketHandler(int receiveBufferSizeInBytes = 4096, Encoding textEncoding = null)
        {
            if (receiveBufferSizeInBytes <= 0)
            {
                throw new ArgumentOutOfRangeException(nameof(receiveBufferSizeInBytes), "Receive buffer size must be > 0 bytes");
            }

            _receiveBufferSize = receiveBufferSizeInBytes;
            _encoding = textEncoding ?? Encoding.UTF8;
        }

        public delegate Task OpenWebSocketEventHandler(OpenWebSocketEventArgs e);
        public event OpenWebSocketEventHandler Opened;
        
        public delegate Task CloseWebSocketEventHandler(CloseWebSocketEventArgs e);
        public event CloseWebSocketEventHandler Closed;

        public delegate Task MessageReceivedEventHandler(WebSocketMessageEventArgs e);
        public event MessageReceivedEventHandler MessageReceived;

        /// <inheritdoc />
        public override async Task RunAsync(WebSocket ws, CancellationToken token)
        {
            var connectionId = Guid.NewGuid().ToString();
            var helper = new WebSocketHelper(ws, _encoding);
            int bufferOffset = 0;

            try
            {
                var openTask = Opened?.Invoke(new OpenWebSocketEventArgs(connectionId, SubProtocol, helper));
                if (null != openTask)
                {
                    await openTask;
                }
            }
            catch (Exception ex)
            {
                Logger.LogError(ex, $"Error while raising {nameof(Opened)} event");
            }

            var buffer = new byte[_receiveBufferSize];
            try
            {
                while (ws.State == WebSocketState.Open)
                {
                    if (buffer.Length - bufferOffset <= 0)
                    {
                        throw new System.IO.InvalidDataException("Receive buffer is too small");
                    }

                    var receiveBuffer = new ArraySegment<byte>(buffer, bufferOffset, buffer.Length - bufferOffset);
                    var result = await ws.ReceiveAsync(receiveBuffer, token);

                    if (result.EndOfMessage)
                    {
                        receiveBuffer = new ArraySegment<byte>(buffer, 0, receiveBuffer.Offset + result.Count);
                        bufferOffset = 0;
                    }
                    else
                    {
                        bufferOffset += result.Count;
                        continue;
                    }

                    if (result.MessageType == WebSocketMessageType.Close)
                    {
                        Logger.LogTrace($"Received closed {result.CloseStatus}: {result.CloseStatusDescription}");

                        try
                        {
                            var closedTask = Closed?.Invoke(new CloseWebSocketEventArgs(connectionId, result.CloseStatus.Value, result.CloseStatusDescription));
                            if (null != closedTask)
                            {
                                await closedTask;
                            }
                        }
                        catch (Exception ex)
                        {
                            Logger.LogError(ex, $"Error while raising {nameof(Closed)} event");
                        }

                        if (ws.State == WebSocketState.CloseReceived)
                        {
                            await ws.CloseAsync(result.CloseStatus.Value, result.CloseStatusDescription, token);
                        }

                        // exit
                        return;
                    }
                    else
                    {
                        Logger.LogTrace($"Received {result.MessageType} {result.Count} bytes {result.EndOfMessage}");

                        WebSocketMessageEventArgs eventArg;

                        try
                        {
                            if (string.Equals(Message.USP_WEB_SOCKET_PROTOCOL, SubProtocol, StringComparison.OrdinalIgnoreCase))
                            {
                                Message uspMessage = result.MessageType == WebSocketMessageType.Text
                                    ? (Message)new TextMessage(receiveBuffer)
                                    : (Message)new BinaryMessage(receiveBuffer);

                                eventArg = new WebSocketMessageEventArgs(connectionId, uspMessage, _encoding, helper);
                            }
                            else
                            {
                                eventArg = new WebSocketMessageEventArgs(
                                    connectionId, result.MessageType != WebSocketMessageType.Text, receiveBuffer, result.EndOfMessage, _encoding, helper);
                            }

                            var mrTask = MessageReceived?.Invoke(eventArg);
                            if(null != mrTask)
                            {
                                await mrTask;
                            }
                        }
                        catch (Exception ex)
                        {
                            Logger.LogError(ex, $"Failed while parsing the received message, or raising the {nameof(MessageReceived)} event");
                        }
                    }
                }

            }
            catch (OperationCanceledException)
            {
                // ignore
            }
            catch (Exception ex)
            {
                Logger.LogError(ex, "Error while processing web socket messages");
            }
            finally
            {
                try
                {
                    if (ws.State == WebSocketState.Open || ws.State == WebSocketState.CloseReceived)
                    {
                        await ws.CloseAsync(WebSocketCloseStatus.InternalServerError, "Internal server error", CancellationToken.None);
                    }
                }
                catch (Exception ex)
                {
                    Logger.LogError(ex, "Error while closing the web socket");
                }
            }
        }

        private class WebSocketHelper : IWebSocketHelper
        {
            private readonly Encoding _encoding;

            public WebSocketHelper(WebSocket ws, Encoding encoding)
            {
                WebSocket = ws ?? throw new ArgumentNullException(nameof(ws));
                _encoding = encoding ?? throw new ArgumentNullException(nameof(encoding));
            }

            public WebSocket WebSocket { get; }

            public Task SendAsync(string text, CancellationToken token)
            {
                var bytes = _encoding.GetBytes(text ?? throw new ArgumentNullException(nameof(text)));
                var segment = new ArraySegment<byte>(bytes);

                return WebSocket.SendAsync(segment, WebSocketMessageType.Text, true, token);
            }

            public Task SendAsync(byte[] data, CancellationToken token)
            {
                return WebSocket.SendAsync(
                    new ArraySegment<byte>(data ?? throw new ArgumentNullException(nameof(data))),
                    WebSocketMessageType.Binary,
                    true,
                    token);
            }

            public Task SendAsync(Message message, CancellationToken token)
            {
                if (message == null) throw new ArgumentNullException(nameof(message));

                return WebSocket.SendAsync(
                    new ArraySegment<byte>(message.Serialize()),
                    message.IsBinary ? WebSocketMessageType.Binary : WebSocketMessageType.Text,
                    true,
                    token);
            }
        }
    }
}

namespace Azure.AI.Test.Common.Servers.WebSocketHandlers.Interactive
{
    public interface IWebSocketHelper
    {
        WebSocket WebSocket { get; }

        Task SendAsync(string text, CancellationToken token);

        Task SendAsync(byte[] data, CancellationToken token);

        Task SendAsync(Message message, CancellationToken token);
    }

    public abstract class WebSocketEventArgs : EventArgs
    {
        public WebSocketEventArgs(string connectionId)
        {
            if (string.IsNullOrWhiteSpace(connectionId))
            {
                throw new ArgumentException("Connection identifier cannot be null, empty, nor consist only of white space", nameof(connectionId));
            }

            ConnectionId = connectionId;
        }

        /// <summary>
        /// A unique identifier for the web socket session. You can use this to distinguish between events from different web socket
        /// </summary>
        public string ConnectionId { get; }
    }

    public class OpenWebSocketEventArgs : WebSocketEventArgs
    {
        public OpenWebSocketEventArgs(string connectionId, string protocol, IWebSocketHelper webSocketHelper) : base(connectionId)
        {
            Protocol = protocol ?? string.Empty;
            this.WebSocketHelper = webSocketHelper;
        }

        public string Protocol { get; }

        public IWebSocketHelper WebSocketHelper { get; }
    }

    public class CloseWebSocketEventArgs : WebSocketEventArgs
    {
        public CloseWebSocketEventArgs(string connectionId, WebSocketCloseStatus status, string description) : base(connectionId)
        {
            Status = status;
            Description = description;
        }

        public WebSocketCloseStatus Status { get; }

        public string Description { get; }
    }

    public class WebSocketMessageEventArgs : WebSocketEventArgs
    {
        private readonly Encoding _encoding;

        public WebSocketMessageEventArgs(string connectionId, bool isBinary, byte[] data, bool isEndOfMessage, Encoding encoding, IWebSocketHelper helper)
            : this(connectionId, isBinary, new ArraySegment<byte>(data ?? throw new ArgumentNullException(nameof(data))), isEndOfMessage, encoding, helper)
        { }

        public WebSocketMessageEventArgs(string connectionId, bool isBinary, ArraySegment<byte> data, bool isEndOfMessage, Encoding encoding, IWebSocketHelper helper)
            : base(connectionId)
        {
            IsBinary = isBinary;
            RawData = new byte[data.Count];
            Array.Copy(data.Array, data.Offset, RawData, 0, data.Count);
            IsEndOfMessage = isEndOfMessage;
            _encoding = encoding ?? throw new ArgumentNullException(nameof(encoding));
            Response = helper;

            if (isEndOfMessage && !isBinary)
            {
                Text = _encoding.GetString(RawData);
            }
        }

        public WebSocketMessageEventArgs(string connectionId, Message message, Encoding encoding, IWebSocketHelper helper)
            : base(connectionId)
        {
            UspMessage = message ?? throw new ArgumentNullException(nameof(message));

            IsBinary = UspMessage.IsBinary;
            RawData = message.Serialize();
            IsEndOfMessage = true;
            _encoding = encoding ?? throw new ArgumentNullException(nameof(encoding));
            Response = helper;
        }

        public bool IsBinary { get; }

        public byte[] RawData { get; }

        public bool IsEndOfMessage { get; }

        public string Text { get; }

        public Message UspMessage { get; }

        public IWebSocketHelper Response { get; }
    }
}
