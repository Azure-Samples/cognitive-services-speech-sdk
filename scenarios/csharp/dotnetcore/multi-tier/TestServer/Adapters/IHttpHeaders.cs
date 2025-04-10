using System.Collections.Generic;
using System.Linq;

namespace Azure.AI.Test.Common.Servers.Adapters
{
    /// <summary>
    /// Represents HTTP headers
    /// </summary>
    public interface IHttpHeaders : IEnumerable<KeyValuePair<string, IEnumerable<string>>>
    {
        /// <summary>
        /// Gets the number of header names
        /// </summary>
        int Count { get; }

        /// <summary>
        /// Gets all the header names
        /// </summary>
        IEnumerable<string> Keys { get; }

        /// <summary>
        /// Gets the values for the specified header
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <returns>The values for the header. If the header is not set, you will get back an empty enumerable</returns>
        IEnumerable<string> GetValues(string name);
    }

    /// <summary>
    /// Represents the HTTP request headers
    /// </summary>
    public interface IHttpRequestHeaders : IHttpHeaders
    {
        /// <summary>
        /// Gets or sets the content type
        /// </summary>
        string ContentType { get; }

        /// <summary>
        /// Gets or sets the content length
        /// </summary>
        long? ContentLength { get; }
    }

    /// <summary>
    /// Represents the HTTP response headers
    /// </summary>
    public interface IHttpResponseHeaders : IHttpHeaders
    {
        /// <summary>
        /// Gets or sets the content type
        /// </summary>
        string ContentType { get; set; }

        /// <summary>
        /// Gets or sets the content length
        /// </summary>
        long? ContentLength { get; set; }

#if NETFRAMEWORK
        /// <summary>
        /// Gets or sets whether or not transfer encoding is used
        /// </summary>
        bool? TransferEncodingChunked { get; set; }
#endif

        /// <summary>
        /// Sets the HTTP header in the response. If there is already a header with that value it will be overwritten.
        /// This will automatically escape header names and values as needed
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <param name="value">The header value</param>
        void SetValue(string name, string value);

        /// <summary>
        /// Adds the HTTP header in the response. If there is already a header with that value, a new header will be appended
        /// with the new value. This will automatically escape header names and values as needed
        /// </summary>
        /// <param name="name">The name of the header</param>
        /// <param name="value">The header value</param>
        void AddValue(string name, string value);
    }

    /// <summary>
    /// Extension methods for HTTP headers
    /// </summary>
    public static class HttpHeadersExtensions
    {
        /// <summary>
        /// Gets the single header value for the specified key, or returns the default value
        /// </summary>
        /// <param name="headers">The HTTP headers to read from</param>
        /// <param name="key">The name of the header</param>
        /// <param name="default">The value to return if the header is not set</param>
        /// <returns>The first value of the header, or null if it is not set</returns>
        public static string GetOrDefault(this IHttpHeaders headers, string key, string @default = null)
        {
            string strValue;

            if (headers != null
                && key != null
                && (strValue = headers.GetValues(key)?.FirstOrDefault(v => !string.IsNullOrWhiteSpace(v))) != null)
            {
                return strValue;
            }
            else
            {
                return @default;
            }
        }
    }
}
