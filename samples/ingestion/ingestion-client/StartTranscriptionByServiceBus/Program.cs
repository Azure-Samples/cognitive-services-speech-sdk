// <copyright file="Program.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscription
{
    using Azure.Storage;
    using Azure.Storage.Blobs;

    using Connector;
    using Connector.Enums;

    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Hosting;

    using StartTranscriptionByTimer;

    /// <summary>
    /// Represents the entry point of the application.
    /// </summary>
    public static class Program
    {
        /// <summary>
        /// Main entry point of the function app.
        /// </summary>
        /// <param name="args"></param>
        public static void Main(string[] args)
        {
            var storageConnectionString = StartTranscriptionEnvironmentVariables.AzureWebJobsStorage;
            var blobServiceClient = new BlobServiceClient(storageConnectionString);

            var storageCredential = new StorageSharedKeyCredential(
                AzureStorageConnectionExtensions.GetValueFromConnectionString("AccountName", storageConnectionString),
                AzureStorageConnectionExtensions.GetValueFromConnectionString("AccountKey", storageConnectionString));

            var host = new HostBuilder()
                .ConfigureFunctionsWorkerDefaults()
                .ConfigureServices(s =>
                {
                    // This is a unified way to configure logging filter for all functions.
                    s.ConfigureIngestionClientLogging();
                    s.AddSingleton(blobServiceClient);
                    s.AddSingleton(storageCredential);
                    s.AddTransient<IStorageConnector, StorageConnector>();

                    s.AddAzureClients(clientBuilder =>
                    {
                        clientBuilder.AddServiceBusClient(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString)
                            .WithName(ServiceBusClientName.StartTranscriptionServiceBusClient.ToString());
                        clientBuilder.AddServiceBusClient(StartTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString)
                            .WithName(ServiceBusClientName.FetchTranscriptionServiceBusClient.ToString());
                    });
                })
                .Build();

            host.Run();
        }
    }
}