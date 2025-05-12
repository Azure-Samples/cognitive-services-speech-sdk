//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using Azure.AI.Test.Common.Logging;

namespace Azure.AI.Test.Common.Servers
{
    /// <summary>
    /// The test server configuration
    /// </summary>
    public class TestServerConfig
    {
        internal int _port = 0;
        internal Logging.ILogger _logger = VoidLogger.Instance;
        internal Dictionary<string, Func<IWebSocketHandler>> _routes = new Dictionary<string, Func<IWebSocketHandler>>();

        /// <summary>
        /// Sets the port to use
        /// </summary>
        /// <param name="port">The port to use or 0 to automatically use the next available free port</param>
        /// <returns>This config instance for chaining</returns>
        public TestServerConfig SetPort(int port)
        {
            if (port < 0 || port > 65535)
            {
                throw new ArgumentOutOfRangeException(nameof(port), "Port is not valid. Must be 0 <= port <= 65535");
            }

            _port = port;
            return this;
        }


        /// <summary>
        /// Sets the logger to use for the test server
        /// </summary>
        /// <param name="logger">The logger to use. Set to null to disable logging</param>
        /// <returns>This config instance for chaining</returns>
        public TestServerConfig SetLogger(Logging.ILogger logger)
        {
            _logger = logger ?? VoidLogger.Instance;
            return this;
        }

        /// <summary>
        /// Adds a custom web socket handler. The path to your handler will be "ws/custom"
        /// </summary>
        /// <param name="creator">The function to create the web socket handler</param>
        /// <returns>The config instance for chaining</returns>
        public TestServerConfig AddWebSocketRoute(Func<IWebSocketHandler> creator)
            => AddWebSocketRoute("ws/custom", creator);

        /// <summary>
        /// Adds a custom web socket handler
        /// </summary>
        /// <param name="route">The path to use for your custom web socket handler</param>
        /// <param name="creator">The function to create the web socket handler</param>
        /// <returns>The config instance for chaining</returns>
        public TestServerConfig AddWebSocketRoute(string route, Func<IWebSocketHandler> creator)
        {
            if (string.IsNullOrWhiteSpace(route))
            {
                throw new ArgumentException(nameof(route) + " cannot be null, empty or consist only of white space");
            }
            else if (creator == null)
            {
                throw new ArgumentNullException(nameof(creator));
            }

            _routes.Add(route, creator);
            return this;
        }
    }
}
