
using Azure.AI.Test.Common;
using Azure.AI.Test.Common.Logging;
using Azure.AI.Test.Common.Servers.Adapters;
using Azure.AI.Test.Common.Servers.Adapters.NetCore;
using Microsoft.AspNetCore.Mvc;

namespace System.Web.Http
{
    /// <summary>
    /// Interop for .Net framework WebAPI, and ASP.Net Core API controllers
    /// </summary>
    [ApiController]
    public class ApiControllerBase : ControllerBase
    {
        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="logger">The logger instance to use</param>
        public ApiControllerBase(Azure.AI.Test.Common.Logging.ILogger logger)
        {
            Logger = logger ?? VoidLogger.Instance;
        }

        /// <summary>
        /// Gets the logger instance to use
        /// </summary>
        public Azure.AI.Test.Common.Logging.ILogger Logger { get; }

        /// <summary>
        /// Gets the HTTP request adapter
        /// </summary>
        public IHttpRequest RequestAdapter => new HttpRequestAdapter(HttpContext);

        /// <summary>
        /// Gets the HTTP response adapter
        /// </summary>
        public IHttpResponse ResponseAdapter => new HttpResponseAdapter(HttpContext?.Response);
    }
}
