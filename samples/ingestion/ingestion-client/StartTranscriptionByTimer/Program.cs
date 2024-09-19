// <copyright file="Program.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System.IO;
    using System.Threading;

    using Azure.Storage;
    using Azure.Storage.Blobs;

    using Connector;
    using Connector.Enums;

    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.Configuration;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Hosting;

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
                    services.AddSingleton(blobServiceClient);
                    services.AddSingleton(storageCredential);
                    services.AddTransient<IStorageConnector, StorageConnector>();
                    services.AddTransient<IStartTranscriptionHelper, StartTranscriptionHelper>();

                    services.AddAzureClients(clientBuilder =>
                    {
                        clientBuilder.AddServiceBusClient(config.StartTranscriptionServiceBusConnectionString)
                            .WithName(ServiceBusClientName.StartTranscriptionServiceBusClient.ToString());
                        clientBuilder.AddServiceBusClient(config.FetchTranscriptionServiceBusConnectionString)
                            .WithName(ServiceBusClientName.FetchTranscriptionServiceBusClient.ToString());
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

            host.Run();
        }
    }
}