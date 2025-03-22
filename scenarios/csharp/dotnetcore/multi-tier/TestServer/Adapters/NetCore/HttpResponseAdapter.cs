using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Http.Features;

namespace Azure.AI.Test.Common.Servers.Adapters.NetCore
{
    /// <summary>
    /// Adapter for HTTP responses
    /// </summary>
    public class HttpResponseAdapter : IHttpResponseWithBody
    {
        private HttpResponse _response;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="response">The HTTP response to wrap</param>
        public HttpResponseAdapter(HttpResponse response)
        {
            _response = response ?? throw new ArgumentNullException(nameof(response));
            Headers = new HttpHeadersAdapter(_response);
        }

        /// <inheritdoc />
        public int StatusCode
        {
            get => _response.StatusCode;
            set => _response.StatusCode = value;
        }

        /// <inheritdoc />
        /// <remarks>This will not work on HTTP/2</remarks>
        public string ReasonPhrase
        {
            get => _response.HttpContext.Features.Get<IHttpResponseFeature>()?.ReasonPhrase;
            set => _response.HttpContext.Features.Get<IHttpResponseFeature>().ReasonPhrase = value;
        }

        /// <inheritdoc />
        public IHttpResponseHeaders Headers { get; }

        /// <inheritdoc />
        public Stream Body => _response.Body;

        /// <inheritdoc />
        public Task WriteAsync(byte[] data, int offset, int count, CancellationToken token)
            => _response.Body.WriteAsync(data, offset, count, token);
    }
}
