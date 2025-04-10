using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;

namespace Azure.AI.Test.Common.Servers.Adapters.NetCore
{
    /// <summary>
    /// Adapter for .Net Core WebAPI HTTP contexts
    /// </summary>
    public class HttpContextAdapter : IHttpContext
    {
        HttpContext _context;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="context">The HTTP context to wrap</param>
        public HttpContextAdapter(HttpContext context)
        {
            _context = context ?? throw new ArgumentNullException(nameof(context));
            Request = new HttpRequestAdapter(context);
            Response = new HttpResponseAdapter(context.Response);
        }

        /// <inheritdoc />
        public IHttpRequest Request { get; }

        /// <inheritdoc />
        public IHttpResponseWithBody Response { get; }

        /// <inheritdoc />
        public async Task AcceptWebSocketAsync(string subProtocol, RunWebSocketAsync runWebSocket, CancellationToken token)
        {
            var webSocket = await _context.WebSockets.AcceptWebSocketAsync(subProtocol)
                .ConfigureAwait(false);

            await runWebSocket(webSocket, token)
                .ConfigureAwait(false);
        }
    }
}
