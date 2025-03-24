//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.ComponentModel;
using System.Globalization;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using Azure.AI.Test.Common.Servers.Adapters;

namespace Azure.AI.Test.Common.Servers
{
    /// <summary>
    /// A helper class to start a test server for testing. The server remains active until you dispose of the instance
    /// </summary>
    public class TestServer : DisposableBase
    {
        private IHttpServer _server;

        /// <summary>
        /// Creates a new instance. You can use the <see cref="HttpEndpoint"/> or <see cref="WebSocketEndpoint"/>
        /// properties to get the URI to use to connect to the test server. The server is running as soon as the
        /// constructor returns
        /// </summary>
        /// <param name="config">(Optional) The configuration to use for the test server</param>
        public TestServer(TestServerConfig config = null)
        {
            config = config ?? new TestServerConfig();

#if NETFRAMEWORK
            _server = new Adapters.NetFramework.OwinServerAdapter();
#elif NET
            _server = new Adapters.NetCore.KestrelHttpServer();
#else
#error If you are building for .Net Framework, please specify the NETFRAMEWORK preprocessor directive. For .Net Core please specify NET preprocessor directive. Otherwise please implement the interfaces in the Azure.AI.Test.Common.Servers.Adapters namespace
#endif

            ushort port = _server.Start($"http://127.0.0.1:{config._port.ToString(CultureInfo.InvariantCulture)}/", config);

            string hostPort = "localhost:" + port.ToString(CultureInfo.InvariantCulture) + "/";
            HttpEndpoint = new Uri("http://" + hostPort, UriKind.Absolute);
            WebSocketEndpoint = new Uri("ws://" + hostPort, UriKind.Absolute);
        }

        /// <summary>
        /// Gets the URI to the base HTTP endpoint
        /// </summary>
        public Uri HttpEndpoint { get; }

        /// <summary>
        /// Gets the URI to the base web socket endpoint
        /// </summary>
        public Uri WebSocketEndpoint { get; }

        /// <summary>
        /// Gets the HTTP endpoint that returns the current UTC time. This supports only GET requests,
        /// and the response will be a "plain/text" string with the following format:
        /// Current time is 2023-08-30T06:29:33.2993220Z
        /// </summary>
        /// <returns>The URI to use</returns>
        public Uri HttpTime() => GetUri(HttpEndpoint, "/http/current_time");

        /// <summary>
        /// Gets the HTTP endpoint that echoes back the request. All request headers will be included in the response
        /// but their names will be prefixed with "X-Request-". If a body was sent in the request, it will be echoed
        /// back as well in the response
        /// </summary>
        /// <returns>The URI to use</returns>
        public Uri HttpEcho() => GetUri(HttpEndpoint, "/http/echo");

        /// <summary>
        /// Gets the HTTP endpoint that returns a specified status code. You can optionally provide a body to
        /// return, as well as the content type
        /// </summary>
        /// <param name="status">The status code to return</param>
        /// <param name="content">(Optional) The body to return. If set to null, no body will be returned in the response</param>
        /// <param name="contentType">(Optional) The content type of the body. This is ignored if the content is null</param>
        /// <returns>The URI to use</returns>
        public Uri HttpStatus(HttpStatusCode status, string content = null, string contentType = null)
            => GetUri(HttpEndpoint, "/http/status",
                "status", (int)status,
                "content", content,
                "contentType", contentType);

        /// <summary>
        /// Gets the HTTP endpoint that "streams" a large amount of data to the client.
        /// </summary>
        /// <returns>The URI to use</returns>
        public Uri HttpStream() => GetUri(HttpEndpoint, "/http/stream");

        /// <summary>
        /// Gets the HTTP endpoint that "streams" a large amount of data to the client with chunking.
        /// </summary>
        /// <returns>The URI to use</returns>
        public Uri HttpChunked() => GetUri(HttpEndpoint, "/http/chunked");

        /// <summary>
        /// Gets the web socket echo endpoint URI. This endpoint will echo back every received message
        /// </summary>
        /// <returns>The URI to use</returns>
        public Uri WSEchoEndpoint() => GetUri(WebSocketEndpoint, "/ws/echo");

        /// <summary>
        /// Gets the web socket close endpoint URI. This will close the web socket connection after the
        /// specified number of messages are received. If closeAfter is greater than 0, the endpoint will
        /// return a text message with a count starting from 0
        /// </summary>
        /// <param name="status">The web socket status to return when closing</param>
        /// <param name="closeReason">The reason to return when closing</param>
        /// <param name="closeAfter">(Optional) After how many messages to close the web socket</param>
        /// <returns>The URI to use</returns>
        public Uri WSCloseEndpoint(WebSocketCloseStatus status, string closeReason, int closeAfter = 0)
            => GetUri(WebSocketEndpoint, "/ws/close",
                "status", (int)status,
                "reason", closeReason,
                "after", closeAfter);

        /// <summary>
        /// Gets the web socket proxy endpoint URI. This will add the test service as a middleman allowing
        /// you greater flexibility. For example, you can send special Templeton command messages to force
        /// a connection to close with an status and reason that you want, introduce delays, alter service
        /// responses, and so on
        /// </summary>
        /// <param name="upstreamUri">The actual upstream endpoint that the proxy should connect to</param>
        /// <param name="subProtocol">(Optional) The sub protocol to set on the web socket upgrade request to the upstream service</param>
        /// <param name="proxyHost">(Optional) The proxy host to use when connecting to the upstream endpoint</param>
        /// <param name="proxyPort">(Optional) The proxy port to use when connecting to the upstream endpoint</param>
        /// <returns>The URI to use</returns>
        public Uri WSProxyEndpoint(Uri upstreamUri, string proxyHost = null, int proxyPort = 0, string subProtocol = null)
            => GetUri(WebSocketEndpoint, "/ws/proxy",
                "x-proxy-uri", upstreamUri,
                "x-proxy-sub-protocol", subProtocol,
                "x-proxy-proxy", string.IsNullOrWhiteSpace(proxyHost) ? null : $"{proxyHost}:{proxyPort}");

        /// <inheritdoc />
        protected override void Dispose(bool disposeManaged)
        {
            if (!disposeManaged)
            {
                return;
            }

            _server?.Dispose();
        }

        private static Uri GetUri(Uri baseUri, string subPath, params object[] query)
        {
            if (query != null && query.Length % 2 == 1)
            {
                throw new ArgumentException("Query parameters must have even length", nameof(query));
            }

            var builder = new StringBuilder();

            for (int i = 0; i < query.Length; i += 2)
            {
                string key = query[i] as string;
                if (key == null)
                {
                    throw new ArgumentException($"Key at index {i} is not a string");
                }

                object value = query[i + 1];
                if (value == null)
                {
                    continue;
                }

                if (builder.Length > 0) builder.Append('&');
                builder.Append(Uri.EscapeDataString(key));

                string strValue;
                var converter = TypeDescriptor.GetConverter(value.GetType());

                if (converter == null)
                {
                    throw new ArgumentException($"Value at index {i + 1} could not be converted to a string");
                }
                else
                {
                    strValue = converter.ConvertToInvariantString(value);
                }

                if (strValue?.Length > 0)
                {
                    builder.Append('=');
                    builder.Append(Uri.EscapeDataString(strValue));
                }
            }

            var uri = new UriBuilder(baseUri);
            uri.Path = subPath ?? string.Empty;
            uri.Query = builder.ToString();

            return uri.Uri;
        }
    }
}
