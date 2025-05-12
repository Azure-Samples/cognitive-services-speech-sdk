// program.cs in SpeechSDKRemoteServiceGRPC

using Microsoft.AspNetCore.Server.Kestrel.Core;
using Microsoft.Extensions.Logging.Console;
using SpeechSDKSamples.Remote.Service.Core;
using System.CommandLine;
using System.CommandLine.Parsing;

namespace SpeechSDKSamples.Remote.Service.GRPC.Services
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
                if (string.IsNullOrEmpty(region) || string.IsNullOrEmpty(key) || string.IsNullOrEmpty(endpoint))
                {
                    Console.WriteLine("Region, key, and endpoint must be provided.");
                    return;
                }

                var configService = new BasicConfigurationService
                {
                    SpeechServiceRegion = region,
                    SpeechServiceKey = key,
                    SpeechServiceEndpoint = new Uri(endpoint)
                };

                var app = CreateWebApp(configService);
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

            builder.WebHost.ConfigureKestrel(options =>
            {
                options.ListenAnyIP(port, listenOptions =>
                {
                    listenOptions.Protocols = HttpProtocols.Http2;
                });
            });

            builder.Services.AddGrpc();
            builder.Services.AddSingleton(configService);

            builder.Logging.AddSimpleConsole(options => simpleLoggerOptions.CopyTo(options));
            var app = builder.Build();

            app.MapGrpcService<SpeechSDKService>();
            app.MapGet("/", () => "Communication with gRPC endpoints must be made through a gRPC client. To learn how to create a client, visit: https://go.microsoft.com/fwlink/?linkid=2086909");

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
