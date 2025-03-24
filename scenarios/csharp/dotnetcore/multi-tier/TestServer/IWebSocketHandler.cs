//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Azure.AI.Test.Common.Logging;
using Azure.AI.Test.Common.Servers.Adapters;

namespace Azure.AI.Test.Common.Servers
{
    /// <summary>
    /// The result of processing the web socket upgrade request
    /// </summary>
    public readonly struct ProcessRequestResult
    {
        public ProcessRequestResult(string subProtocol)
        {
            ShouldUpgrade = true;
            SubProtocol = subProtocol;
            ResponseStatusCode = 0;
            ResponseBody = null;
            ResponseContentType = null;
        }

        public ProcessRequestResult(HttpStatusCode code, string body, string contentType = null)
        {
            ShouldUpgrade = false;
            SubProtocol = null;
            ResponseStatusCode = (int)code;
            ResponseBody = Encoding.UTF8.GetBytes(body);
            ResponseContentType = string.IsNullOrWhiteSpace(contentType) ? "text/plain" : contentType;
        }

        /// <summary>
        /// True if we should upgrade the request to a web socket. False otherwise
        /// </summary>
        public bool ShouldUpgrade { get; }

        /// <summary>
        /// The web socket sub protocol to accept. Set this to null if you don't need to accept a particular one
        /// </summary>
        public string SubProtocol { get; }

        /// <summary>
        /// The status code to send back in the response to the client. This value is only
        /// used if <see cref="ShouldUpgrade"/> if false
        /// </summary>
        public int ResponseStatusCode { get; }

        /// <summary>
        /// The body of the response back to the client. This value is only used if
        /// <see cref="ShouldUpgrade"/> if false
        /// </summary>
        public byte[] ResponseBody { get; }

        /// <summary>
        /// The status code to send back in the response to the client. This value is only
        /// used if <see cref="ShouldUpgrade"/> if false
        /// </summary>
        public string ResponseContentType { get; }
    }

    /// <summary>
    /// Interface that a web socket handler should implement
    /// </summary>
    public interface IWebSocketHandler
    {
        /// <summary>
        /// Sets the logger that the handler will use
        /// </summary>
        Logging.ILogger Logger { set; }

        /// <summary>
        /// Processes the incoming HTTP upgrade request to extract needed values from it (e.g. query parameters).
        /// This can also be used to return specific responses to clients
        /// </summary>
        /// <param name="request">The HTTP request to process</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>The result of processing request</returns>
        Task<ProcessRequestResult> ProcessRequestAsync(IHttpRequest request, CancellationToken token);

        /// <summary>
        /// Runs the code that processes the web socket
        /// </summary>
        /// <remarks>You must not return from this until you are done with the web socket. Otherwise your web
        /// socket request will be terminated</remarks>
        /// <param name="webSocket">The web socket to use</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>The asynchronous task that completes when the web socket is done</returns>
        Task RunAsync(WebSocket webSocket, CancellationToken token);
    }
}
