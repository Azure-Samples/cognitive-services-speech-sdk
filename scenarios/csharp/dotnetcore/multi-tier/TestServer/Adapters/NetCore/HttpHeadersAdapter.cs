using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Azure.AI.Test.Common.Utils;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Primitives;

namespace Azure.AI.Test.Common.Servers.Adapters.NetCore
{
    /// <summary>
    /// HTTP headers adapters for the .Net Core implementation
    /// </summary>
    public class HttpHeadersAdapter : IHttpRequestHeaders, IHttpResponseHeaders
    {
        private IHeaderDictionary _headers;
        private Action<string> _setContentType;
        private Func<string> _getContentType;
        private Action<long?> _setContentLength;
        private Func<long?> _getContentLength;

        /// <summary>
        /// Creates a new instance for request headers
        /// </summary>
        /// <param name="request">The HTTP request to wrap</param>
        public HttpHeadersAdapter(HttpRequest request)
        {
            _headers = request?.Headers ?? throw new ArgumentNullException("request.Headers");
            _getContentType = () => request.ContentType;
            _getContentLength = () => request.ContentLength;
        }

        /// <summary>
        /// Creates a new instance for response headers
        /// </summary>
        /// <param name="response">The HTTP response to wrap</param>
        public HttpHeadersAdapter(HttpResponse response)
        {
            _headers = response.Headers ?? throw new ArgumentNullException("response.Headers");
            _setContentType = ct => response.ContentType = ct;
            _getContentType = () => response.ContentType;
            _setContentLength = l => response.ContentLength = l;
            _getContentLength = () => response.ContentLength;
        }

        /// <inheritdoc />
        public string ContentType
        {
            get => _getContentType?.Invoke();
            set => _setContentType?.Invoke(value);
        }

        /// <inheritdoc />
        public long? ContentLength
        {
            get => _getContentLength?.Invoke();
            set => _setContentLength?.Invoke(value);
        }

        /// <inheritdoc />
        public int Count => _headers.Count;

        /// <inheritdoc />
        public IEnumerable<string> Keys => _headers.Keys;

        /// <inheritdoc />
        public IEnumerable<string> GetValues(string name)
            => _headers.GetOrDefault(name, StringValues.Empty);

        /// <inheritdoc />
        public void AddValue(string name, string value)
            => _headers.Add(name, value);

        /// <inheritdoc />
        public void SetValue(string name, string value)
            => _headers[name] = new StringValues(value);

        /// <inheritdoc />
        public IEnumerator<KeyValuePair<string, IEnumerable<string>>> GetEnumerator()
            => _headers.Select(kvp => new KeyValuePair<string, IEnumerable<string>>(kvp.Key, kvp.Value)).GetEnumerator();

        /// <inheritdoc />
        IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();
    }
}
