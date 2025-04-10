//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Concurrent;
using System.Linq;
using System.Linq.Expressions;
using System.Net;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;
using Azure.AI.Test.Common.Logging;
using Azure.AI.Test.Common.Servers.Adapters;

#if NETFRAMEWORK
using System.Web.Http;
#elif NET
using Microsoft.AspNetCore.Components;
#endif

namespace Azure.AI.Test.Common.Servers
{
    /// <summary>
    /// Helper used to accept web Socket upgrade when hosted on an OWIN compliant platform. This will automatically
    /// enumerate all non abstract types that implement IWebSocketHandler, and have a default constructor. The path
    /// generated will use the prefix in the type name e.g. EchoWebSocketHandler will map to ws/echo
    /// </summary>
    public class AcceptWebSocketHelper
    {
        private static readonly ConcurrentDictionary<string, Func<IWebSocketHandler>> WEB_SOCKET_HANDLERS = GenerateWebSocketRouteMappings();
        private Logging.ILogger _logger;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="logger"></param>
        public AcceptWebSocketHelper(Logging.ILogger logger = null)
        {
            _logger = logger ?? VoidLogger.Instance;
        }

        /// <summary>
        /// Adds a custom route using the specified handler. If a route already exists, it will be overwritten
        /// </summary>
        /// <param name="route">The path to handler</param>
        /// <param name="createHandler">The function that creates the handler to use</param>
        public void AddRoute(string route, Func<IWebSocketHandler> createHandler)
        {
            if (string.IsNullOrWhiteSpace(route))
            {
                throw new ArgumentException(nameof(route) + " cannot be null, empty or consist only of white space");
            }

            if (!route.StartsWith("/"))
            {
                route = "/" + route;
            }

            WEB_SOCKET_HANDLERS.AddOrUpdate(route, createHandler, (_, prevCreator) => createHandler);
        }

        /// <summary>
        /// Checks whether or not we can accept this web socket request. This will check if this is an upgrade request,
        /// and whether or not we have a route defined for the request path
        /// </summary>
        /// <param name="context">The http request</param>
        /// <returns>True if we can accept this web socket upgrade request, false otherwise</returns>
        public bool CanAccept(IHttpRequest request)
        {
            string path = request?.Path ?? string.Empty;

            return request != null
                && "GET" == request?.Method
                && request.IsWebSocketUpgrade
                && WEB_SOCKET_HANDLERS.ContainsKey(path);
        }

        /// <summary>
        /// Accepts the web socket upgrade request. This will return HTTP 101 to the client, upgrade the connection
        /// to a web socket, and then call the handler to process the web socket messages
        /// </summary>
        /// <param name="context">The HTTP context</param>
        /// <param name="token">The cancellation token to use. Requesting cancellation on this will abort the web socket</param>
        /// <returns>Asynchronous task to accept upgrade request</returns>
        public async Task AcceptAsync(IHttpContext context, CancellationToken token = default)
        {
            if (context == null)
            {
                throw new ArgumentNullException(nameof(context));
            }

            ProcessRequestResult processResult;
            IWebSocketHandler handler = null;

            if (WEB_SOCKET_HANDLERS.TryGetValue(context.Request?.Path ?? string.Empty, out var creator))
            {
                handler = creator();
                handler.Logger = new Logger(_logger.LogWriter, _logger.Level, handler?.GetType().Name);

                _logger.LogTrace("Processing web socket upgrade request");
                processResult = await handler.ProcessRequestAsync(context.Request, token)
                    .ConfigureAwait(false);
            }
            else
            {
                processResult = new ProcessRequestResult(HttpStatusCode.NotFound, null);
            }

            if (!processResult.ShouldUpgrade)
            {
                await HandleNoUpgrade(context.Response, processResult, token)
                    .ConfigureAwait(false);
                return;
            }

            _logger.LogTrace("Accepting web socket request");

            await context.AcceptWebSocketAsync(
                processResult.SubProtocol,
                (ws, tkn) => RunAsync(handler, ws, tkn),
                token
            ).ConfigureAwait(false);
        }

        private async Task HandleNoUpgrade(IHttpResponseWithBody response, ProcessRequestResult processResult, CancellationToken token)
        {
            _logger.LogTrace($"NOT accepting web socket upgrade request. Will return {processResult.ResponseStatusCode}");

            response.StatusCode = processResult.ResponseStatusCode;
            if (processResult.ResponseBody != null)
            {
                response.Headers.ContentType = processResult.ResponseContentType;
                response.Headers.ContentLength = processResult.ResponseBody.Length;

                await response.WriteAsync(processResult.ResponseBody, 0, processResult.ResponseBody.Length, token)
                    .ConfigureAwait(false);
            }
        }

        private async Task RunAsync(IWebSocketHandler handler, System.Net.WebSockets.WebSocket webSocket, CancellationToken token)
        {
            _logger.LogTrace("Upgraded to web socket. Start listening...");

            try
            {
                await handler.RunAsync(webSocket, token)
                    .ConfigureAwait(false);

                // wait for client to acknowledge the close
                DateTimeOffset maxWait = DateTimeOffset.UtcNow +
                    (System.Diagnostics.Debugger.IsAttached
                        ? TimeSpan.FromMinutes(15)
                        : TimeSpan.FromSeconds(5));

                while (webSocket.State < System.Net.WebSockets.WebSocketState.Closed
                    && DateTimeOffset.UtcNow < maxWait)
                {
                    await Task.Delay(100, token);
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exited with unhandled exception");
            }
            finally
            {
                _logger.LogTrace("Web socket handler completed");
            }
        }

        #region Automatically find web socket handlers, and configure routes

        private static ConcurrentDictionary<string, Func<IWebSocketHandler>> GenerateWebSocketRouteMappings()
        {
            var handlerInterface = typeof(IWebSocketHandler);

            var webSocketHandlerTypes = typeof(AcceptWebSocketHelper).Assembly
                .GetTypes()
                .Where(t => handlerInterface.IsAssignableFrom(t) && !t.IsAbstract)
                .Where(t =>
                {
                    // check for parameterless constructor
                    var constructor = t.GetConstructor(BindingFlags.Public | BindingFlags.Instance, null, Array.Empty<Type>(), null);
                    return constructor != null;
                });

            var dict = new ConcurrentDictionary<string, Func<IWebSocketHandler>>(StringComparer.OrdinalIgnoreCase);

            // add mappings based on the name of the type first
            foreach (var type in webSocketHandlerTypes)
            {
                string route;

                var routeAttrib = type.GetCustomAttribute<RouteAttribute>();
                if (!string.IsNullOrWhiteSpace(routeAttrib?.Template))
                {
                    route = "/" + routeAttrib.Template;
                }
                else
                {
                    string name = type.Name;
                    int postfix = name.IndexOf("WebSocketHandler");
                    if (postfix > 0)
                    {
                        name = name.Substring(0, postfix);
                    }

                    route = "/ws/" + name.ToLowerInvariant();
                }

                // capturing loop variables in lambdas is the epitome of evil. Generate a new creator function instead
                dict[route] = CreateCreator(type);
            }

            return dict;
        }

        private static Func<IWebSocketHandler> CreateCreator(Type t)
        {
            var exParam = Expression.Constant(t, typeof(Type));

            var activatorMethod = typeof(Activator).GetMethod(nameof(Activator.CreateInstance), new[] { typeof(Type) });
            var callMethod = Expression.Call(activatorMethod, exParam);

            var convertCall = Expression.Convert(callMethod, typeof(IWebSocketHandler));

            var expr = Expression.Lambda<Func<IWebSocketHandler>>(convertCall, Array.Empty<ParameterExpression>());
            return expr.Compile();
        }

        #endregion
    }
}
