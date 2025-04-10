using System.Net.WebSockets;
using System.Threading;
using System.Threading.Tasks;

namespace Azure.AI.Test.Common.Servers.Adapters
{
    /// <summary>
    /// Runs the web socket. This should not complete until you are done with the web socket otherwise the
    /// web socket connection will be aborted.
    /// </summary>
    /// <param name="webSocket">The open web socket connection</param>
    /// <param name="token">The cancellation token to use</param>
    /// <returns>Task to run the web socket. The task should not complete until you are done with the web socket</returns>
    public delegate Task RunWebSocketAsync(WebSocket webSocket, CancellationToken token);

    /// <summary>
    /// Represents the HTTP context
    /// </summary>
    public interface IHttpContext
    {
        /// <summary>
        /// Gets the incoming request
        /// </summary>
        IHttpRequest Request { get; }

        /// <summary>
        /// Gets the response to send back
        /// </summary>
        IHttpResponseWithBody Response { get; }

        /// <summary>
        /// Accepts a web socket upgrade request
        /// </summary>
        /// <param name="subProtocol">The web socket sub protocol to accept. Set this to null if you don't need one</param>
        /// <param name="runWebSocket">The function to run when the web socket has been accepted. The first parameter
        /// is the sub protocol accepted. The second parameter is the cancellation token that will be set if the web socket
        /// is to be aborted. The returned task should not complete until you are done with the web socket</param>
        Task AcceptWebSocketAsync(string subProtocol, RunWebSocketAsync runWebSocket, CancellationToken token);
    }
}
