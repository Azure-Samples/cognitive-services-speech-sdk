using System;

namespace Azure.AI.Test.Common.Servers.Adapters
{
    /// <summary>
    /// Represents a HTTP server that runs the test server code. You start the test server by calling
    /// <see cref="Start(string, TestServerConfig)"/>. You stop the test server by calling <see cref="IDisposable.Dispose"/>
    /// </summary>
    public interface IHttpServer : IDisposable
    {
        /// <summary>
        /// Starts the HTTP server. This should return as soon as the server is ready to accept requests
        /// </summary>
        /// <param name="endpoint">The scheme, host, and port to listen on</param>
        /// <param name="config">The test server configuration to use</param>
        /// <returns>The port the server is running on</returns>
        ushort Start(string endpoint, TestServerConfig config);
    }
}
