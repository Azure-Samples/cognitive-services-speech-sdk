using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Azure.AI.Test.Common.Servers.Adapters
{
    /// <summary>
    /// Represents an HTTP response
    /// </summary>
    public interface IHttpResponse
    {
        /// <summary>
        /// Gets or sets the HTTP status code for the response
        /// </summary>
        int StatusCode { get; set; }

        /// <summary>
        /// Gets or sets the HTTP reason for the response
        /// </summary>
        string ReasonPhrase { get; set; }

        /// <summary>
        /// Gets the response headers
        /// </summary>
        IHttpResponseHeaders Headers { get; }
    }

    /// <summary>
    /// Represents an HTTP response with a body
    /// </summary>
    public interface IHttpResponseWithBody : IHttpResponse
    {
        /// <summary>
        /// Gets the body stream
        /// </summary>
        Stream Body { get; }

        /// <summary>
        /// Writes data to the response stream. You should always set all the required headers before you call this method
        /// </summary>
        /// <param name="data">The data to write</param>
        /// <param name="offset">The offset in the array</param>
        /// <param name="count">The number of bytes to write</param>
        /// <param name="token">The cancellation token to use</param>
        /// <returns>The number of bytes written</returns>
        Task WriteAsync(byte[] data, int offset, int count, CancellationToken token);
    }
}
