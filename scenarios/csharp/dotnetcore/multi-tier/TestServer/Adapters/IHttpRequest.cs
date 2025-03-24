using System;
using System.IO;

namespace Azure.AI.Test.Common.Servers.Adapters
{
    /// <summary>
    /// Represents an HTTP request
    /// </summary>
    public interface IHttpRequest
    {
        /// <summary>
        /// Gets whether or not this is a web socket upgrade request
        /// </summary>
        bool IsWebSocketUpgrade { get; }

        /// <summary>
        /// The HTTP method of the request. This will be normalized to upper case
        /// </summary>
        string Method { get; }

        /// <summary>
        /// Gets the HTTP protocol version of this request
        /// </summary>
        string Version { get; }

        /// <summary>
        /// Gets the HTTP headers for the request
        /// </summary>
        IHttpRequestHeaders Headers { get; }

        /// <summary>
        /// Gets the full URL for this request
        /// </summary>
        Uri RequestUrl { get; }

        /// <summary>
        /// The path of the request. This will always start with a '/' (even for root paths)
        /// </summary>
        string Path { get; }

        /// <summary>
        /// Gets the parsed query parameters of the request
        /// </summary>
        IHttpQueryParameters Query { get; }

        /// <summary>
        /// Gets the body stream of the request. Will return null if there is no body
        /// </summary>
        Stream Body { get; }
    }
}
