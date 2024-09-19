// <copyright file="Program.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System.IO;
    using System.Threading;

    using Azure.Storage;
    using Azure.Storage.Blobs;

    using Connector;
    using Connector.Database;
    using Connector.Enums;

    using Microsoft.EntityFrameworkCore;
    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.Configuration;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Hosting;
    using Microsoft.Extensions.Options;

    public static class Program
    {
        public static void Main(string[] args)
        {
            var host = new HostBuilder()
                .ConfigureFunctionsWorkerDefaults()
                .ConfigureAppConfiguration((context, config) =>
                {
                    config.SetBasePath(Directory.GetCurrentDirectory())
                     .AddJsonFile("local.settings.json", optional: true, reloadOnChange: true)
                     .AddEnvironmentVariables();
                })
                .ConfigureServices((context, services) =>
                {
                    var configuration = context.Configuration;
                    var config = new AppConfig();
                    configuration.Bind(config);

                    var blobServiceClient = new BlobServiceClient(config.AzureWebJobsStorage);
                    var storageCredential = new StorageSharedKeyCredential(
                    AzureStorageConnectionExtensions.GetValueFromConnectionString("AccountName", config.AzureWebJobsStorage),
                    AzureStorageConnectionExtensions.GetValueFromConnectionString("AccountKey", config.AzureWebJobsStorage));

                    // This is a unified way to configure logging filter for all functions.
                    services.ConfigureIngestionClientLogging();

                    if (config.UseSqlDatabase)
                    {
                        services.AddDbContext<IngestionClientDbContext>(
                        options => SqlServerDbContextOptionsExtensions.UseSqlServer(options, config.DatabaseConnectionString));
                    }

                    services.AddSingleton(blobServiceClient);
                    services.AddSingleton(storageCredential);
                    services.AddTransient<IStorageConnector, StorageConnector>();

                    services.AddAzureClients(clientBuilder =>
                    {
                        clientBuilder.AddServiceBusClient(config.StartTranscriptionServiceBusConnectionString)
                            .WithName(ServiceBusClientName.StartTranscriptionServiceBusClient.ToString());
                        clientBuilder.AddServiceBusClient(config.FetchTranscriptionServiceBusConnectionString)
                            .WithName(ServiceBusClientName.FetchTranscriptionServiceBusClient.ToString());

                        if (!string.IsNullOrWhiteSpace(config.CompletedServiceBusConnectionString))
                        {
                            clientBuilder.AddServiceBusClient(config.CompletedServiceBusConnectionString)
                                .WithName(ServiceBusClientName.CompletedTranscriptionServiceBusClient.ToString());
                        }
                    });

                    services.AddHttpClient(nameof(BatchClient), httpClient =>
                    {
                        // timeouts are managed by BatchClient directly:
                        httpClient.Timeout = Timeout.InfiniteTimeSpan;
                        httpClient.DefaultRequestHeaders.UserAgent.ParseAdd($"Ingestion Client ({config.Version})");
                    });

                    services.AddSingleton<BatchClient>();

                    services.Configure<AppConfig>(configuration);
                })
                .Build();

            var config = host.Services.GetService<IOptions<AppConfig>>().Value;

            // apply database migrations once during startup (not with every function execution):
            if (config.UseSqlDatabase)
            {
                using var scope = host.Services.CreateScope();
                var ingestionClientDbContext = scope.ServiceProvider.GetRequiredService<IngestionClientDbContext>();
                ingestionClientDbContext.Database.Migrate();
            }

            host.Run();
        }
    }
}