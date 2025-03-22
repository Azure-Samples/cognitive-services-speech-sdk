using System;
using System.Globalization;
using System.Linq;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Hosting.Server.Features;
using Microsoft.Extensions.DependencyInjection;

namespace Azure.AI.Test.Common.Servers.Adapters.NetCore
{
    /// <summary>
    /// A self hosted HTTP server using Kestrel on .Net core
    /// </summary>
    public class KestrelHttpServer : DisposableBase, IHttpServer
    {
        private IWebHost _host;

        /// <inheritdoc />
        public ushort Start(string endpoint, TestServerConfig config)
        {
            CheckDisposed();

            _host = new WebHostBuilder()
                .UseKestrel()
                .UseUrls(endpoint)
                .ConfigureServices(services =>
                {
                    var webSocketAccept = new AcceptWebSocketHelper(config._logger);
                    foreach (var wsRoute in config._routes)
                    {
                        webSocketAccept.AddRoute(wsRoute.Key, wsRoute.Value);
                    }

                    // Add instances for dependency injection
                    services.AddSingleton(webSocketAccept);
                    services.AddSingleton(config._logger);

                    // enable 'WebAPI' controllers
                    services.AddControllers();
                    services.AddMvcCore(opt =>
                    {
                        opt.RespectBrowserAcceptHeader = true;

                        // disable chunking of text responses
                        opt.OutputFormatters.RemoveType<Microsoft.AspNetCore.Mvc.Formatters.StringOutputFormatter>();
                        opt.OutputFormatters.Insert(1, new NonChunkedStringOutputFormatter());
                    });
                })
                .Configure((context, app) =>
                {
                    app.UseWebSockets(); // This needs to be before any sections that need web socket support

                    app.Use(async (context, next) =>
                    {
                        if (context.WebSockets.IsWebSocketRequest)
                        {
                            var acceptHelper = app.ApplicationServices.GetRequiredService<AcceptWebSocketHelper>();
                            var httpContext = new HttpContextAdapter(context);

                            await acceptHelper.AcceptAsync(httpContext);
                        }
                        else
                        {
                            await next();
                        }
                    });

                    app.UseDeveloperExceptionPage();
                    app.UseRouting();
                    app.UseEndpoints(endpoints =>
                    {
                        endpoints.MapControllers();
                    });
                })
                .Build();

            _host.Start();

            var addressesFeature = _host.ServerFeatures.Get<IServerAddressesFeature>();
            string address = addressesFeature?.Addresses
                ?.FirstOrDefault(a => !string.IsNullOrWhiteSpace(a))
                ?.Split(':', StringSplitOptions.RemoveEmptyEntries)
                ?.LastOrDefault();
            if (ushort.TryParse(address, NumberStyles.Integer, CultureInfo.InvariantCulture, out var port))
            {
                return port;
            }

            Dispose();
            throw new ApplicationException("Failed to determine port the server is listening on");
        }

        /// <inheritdoc />
        protected override void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                _host.StopAsync().Wait();
                _host?.Dispose();
                _host = null;
            }
        }
    }
}
