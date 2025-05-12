//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Net;
using System.Net.WebSockets;
using System.Threading;
using System.Threading.Tasks;
using Azure.AI.Test.Common.Logging;
using Azure.AI.Test.Common.Servers.Adapters;

namespace Azure.AI.Test.Common.Servers.WebSocketHandlers
{
    /// <summary>
    /// Base class for web socket handlers
    /// </summary>
    public abstract class WebSocketHandlerBase : IWebSocketHandler
    {
        private Logging.ILogger _logger;

        /// <summary>
        /// Gets the delegate that can be used to create a web socket handler instance
        /// </summary>
        /// <param name="handlerType">The type that implements <see cref="IWebSocketHandler"/></param>
        /// <returns>The delegate used to create the web socket handler instance</returns>
        public static Func<IWebSocketHandler> CreateWebSocketHandlerDelegate(Type handlerType)
        {
            if (handlerType == null)
            {
                throw new ArgumentNullException(nameof(handlerType));
            }
            else if (!typeof(IWebSocketHandler).IsAssignableFrom(handlerType))
            {
                throw new ArgumentException($"'{handlerType.AssemblyQualifiedName}' does not implement '{typeof(IWebSocketHandler).AssemblyQualifiedName}'");
            }
            else if (handlerType.GetConstructor(new Type[0]) == null)
            {
                throw new ArgumentException($"'{handlerType.AssemblyQualifiedName}' does not have a public parameterless constructor");
            }

            return () => (IWebSocketHandler)Activator.CreateInstance(handlerType);
        }

        /// <inheritdoc />
        public Logging.ILogger Logger
        {
            get => _logger ?? VoidLogger.Instance;
            set => _logger = value;
        }

        /// <summary>
        /// The web socket protocol in use for this web socket connection (e.g. USP). Can be null if
        /// no protocol was specified.
        /// </summary>
        public string SubProtocol { get; protected set; } = null;

        /// <inheritdoc />
        public virtual async Task<ProcessRequestResult> ProcessRequestAsync(IHttpRequest request, CancellationToken token)
        {
            try
            {
                SubProtocol = request?.Headers?.GetOrDefault("Sec-WebSocket-Protocol")?.Trim();

                await InternalProcessRequestAsync(request, token)
                    .ConfigureAwait(false);

                return new ProcessRequestResult(SubProtocol);
            }
            catch (ArgumentException ex)
            {
                return new ProcessRequestResult(HttpStatusCode.BadRequest, ex.Message);
            }
            catch (Exception ex)
            {
                return new ProcessRequestResult(HttpStatusCode.InternalServerError, ex.ToString());
            }
        }

        /// <inheritdoc />
        public abstract Task RunAsync(WebSocket webSocket, CancellationToken token);

        /// <summary>
        /// Override this in your child methods
        /// </summary>
        /// <param name="request">The OWIN HTTP upgrade request</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>Asynchronous task</returns>
        protected virtual Task InternalProcessRequestAsync(IHttpRequest request, CancellationToken token)
        {
            return Task.CompletedTask;
        }
    }
}
