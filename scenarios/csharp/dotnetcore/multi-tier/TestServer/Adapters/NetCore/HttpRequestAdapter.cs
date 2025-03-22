using System;
using System.IO;
using System.Linq;
using Azure.AI.Test.Common.Utils;
using Microsoft.AspNetCore.Http;

namespace Azure.AI.Test.Common.Servers.Adapters.NetCore
{
    /// <summary>
    /// Adapter for HTTP requests
    /// </summary>
    public class HttpRequestAdapter : IHttpRequest
    {
        private HttpRequest _request;
        private Lazy<Uri> _url;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="context">The HTTP context to wrap</param>
        public HttpRequestAdapter(HttpContext context)
        {
            _request = context.Request ?? throw new ArgumentNullException("context.Request");
            _url = new Lazy<Uri>(() => GetRequestUrl(context.Connection, context.Request));
            Headers = new HttpHeadersAdapter(_request);
            Query = new HttpQueryParamsAdapter(_request.Query);
        }

        /// <inheritdoc />
        public bool IsWebSocketUpgrade => _request.HttpContext.WebSockets.IsWebSocketRequest;

        /// <inheritdoc />
        public string Method => _request.Method?.ToUpperInvariant();

        /// <inheritdoc />
        public string Version => _request.Protocol?.Split('/').LastOrDefault() ?? string.Empty;

        /// <inheritdoc />
        public IHttpRequestHeaders Headers { get; }

        /// <inheritdoc />
        public Uri RequestUrl => _url.Value;

        /// <inheritdoc />
        public string Path => (_request.PathBase + _request.Path).ToUriComponent().EnsureStartsWith("/");

        /// <inheritdoc />
        public IHttpQueryParameters Query { get; }

        /// <inheritdoc />
        public Stream Body => _request.Body;

        private static Uri GetRequestUrl(ConnectionInfo connection, HttpRequest request)
        {
            // for some odd reason, HttpRequest.Host does not seem to contain the port number even
            // when not using a default port. Get this instead from Connection
            var builder = new UriBuilder();
            builder.Scheme = request.Scheme;
            builder.Host = request.Host.HasValue ? request.Host.Host : string.Empty;
            builder.Port = request.Host.Port ?? connection?.LocalPort ?? 0;
            builder.Path = request.PathBase.HasValue || request.Path.HasValue
                ? (request.PathBase + request.Path).ToString()
                : "/";
            builder.Query = request.QueryString.ToString();

            return builder.Uri;
        }
    }
}
