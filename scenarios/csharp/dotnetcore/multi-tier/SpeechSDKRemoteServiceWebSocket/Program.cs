// program.cs in SpeechSDKRemoteServiceWebSocket

using Microsoft.Extensions.Logging.Console;
using SpeechSDKSamples.Remote.Service.Core;
using SpeechSDKSamples.Remote.Service.WebSocket.Services.Controllers;
using System.CommandLine;
using System.CommandLine.Parsing;

namespace SpeechSDKSamples.Remote.Service.WebSocket.Services
{
    public class Program
    {
        public static async Task Main(string[] args)
        {
            var regionOption = new Option<string>("--region", "The Azure region for the Speech service");
            var keyOption = new Option<string>("--key", "The Azure key for the Speech service");
            var endpointOption = new Option<string>("--endpoint", () => "http://localhost:5000", "The endpoint for the Speech service");

            var rootCommand = new RootCommand
            {
                regionOption,
                keyOption,
                endpointOption
            };

            rootCommand.SetHandler(async (region, key, endpoint) =>
            {
                var configService = new BasicConfigurationService
                {
                    SpeechServiceRegion = region,
                    SpeechServiceKey = key,
                    SpeechServiceEndpoint = new Uri(endpoint)
                };

                var app = CreateWebApp(configService, 5001);
                await app.RunAsync();

            }, regionOption, keyOption, endpointOption);

            await rootCommand.InvokeAsync(args);
        }

        public static WebApplication CreateWebApp(IConfigurationService configService, int port = 8080)
        {
            var simpleLoggerOptions = new SimpleConsoleFormatterOptions
            {
                ColorBehavior = LoggerColorBehavior.Disabled,
                SingleLine = true,
                UseUtcTimestamp = true,
                TimestampFormat = string.IsNullOrEmpty(Environment.GetEnvironmentVariable("USE_TIMESTAMP")) ? null : "MM/dd/yyyy HH:mm:ss, ",
                IncludeScopes = false
            };

            var builder = WebApplication.CreateBuilder();
            builder.WebHost.UseKestrel();

            builder.WebHost.ConfigureKestrel(options =>
            {
                options.ListenAnyIP(port);
            });

            builder.Services.AddSingleton(configService);
            builder.Services.AddTransient<SpeechSDKRemoteService>();

            builder.Logging.AddSimpleConsole(options => simpleLoggerOptions.CopyTo(options));
            builder.Logging.SetMinimumLevel(LogLevel.Debug);

            var app = builder.Build();

            app.UseRouting();
            app.UseWebSockets();
            app.UseEndpoints(endpoints =>
            {
                endpoints.Map("/ws/SpeechSDKRemoteService", async context =>
                {
                    if (context.WebSockets.IsWebSocketRequest)
                    {
                        var webSocketHandler = context.RequestServices.GetRequiredService<SpeechSDKRemoteService>();
                        var webSocket = await context.WebSockets.AcceptWebSocketAsync();
                        await webSocketHandler.HandleWebSocketConnectionAsync(webSocket);
                    }
                    else
                    {
                        context.Response.StatusCode = 400;
                    }
                });
            });

            return app;
        }
    }

    public static class Extensions
    {
        public static void CopyTo(this SimpleConsoleFormatterOptions sourceOptions, SimpleConsoleFormatterOptions destOptions)
        {
            destOptions.ColorBehavior = sourceOptions.ColorBehavior;
            destOptions.SingleLine = sourceOptions.SingleLine;
            destOptions.UseUtcTimestamp = sourceOptions.UseUtcTimestamp;
            destOptions.TimestampFormat = sourceOptions.TimestampFormat;
            destOptions.IncludeScopes = sourceOptions.IncludeScopes;
        }
    }
}
