// <copyright file="FetchTranscription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;

    using Connector;
    using Connector.Database;

    using Microsoft.Azure.Functions.Worker;
    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Logging;
    using Microsoft.Extensions.Options;

    /// <summary>
    /// Fetch Transcription class.
    /// </summary>
    public class FetchTranscription
    {
        private readonly IServiceProvider serviceProvider;
        private readonly IStorageConnector storageConnector;
        private readonly IAzureClientFactory<ServiceBusClient> serviceBusClientFactory;
        private readonly ILogger<FetchTranscription> logger;
        private readonly BatchClient batchClient;
        private readonly AppConfig appConfig;

        /// <summary>
        /// Initializes a new instance of the <see cref="FetchTranscription"/> class.
        /// </summary>
        /// <param name="serviceProvider">The service provider.</param>
        /// <param name="logger">The FetchTranscription logger.</param>
        /// <param name="storageConnector">Storage Connector dependency</param>
        /// <param name="serviceBusClientFactory">Azure client factory for service bus clients</param>
        /// <param name="batchClient">The client to call the Azure Speech-To-Text batch API</param>
        /// <param name="appConfig">Environment configuration</param>
        public FetchTranscription(
            IServiceProvider serviceProvider,
            ILogger<FetchTranscription> logger,
            IStorageConnector storageConnector,
            IAzureClientFactory<ServiceBusClient> serviceBusClientFactory,
            BatchClient batchClient,
            IOptions<AppConfig> appConfig)
        {
            this.serviceProvider = serviceProvider;
            this.logger = logger;
            this.storageConnector = storageConnector;
            this.serviceBusClientFactory = serviceBusClientFactory;
            this.batchClient = batchClient;
            this.appConfig = appConfig?.Value;
        }

        /// <summary>
        /// Triggered by a Service Bus message to fetch the transcription.
        /// </summary>
        /// <param name="message">The message on the queue</param>
        [Function("FetchTranscription")]
        public async Task Run([ServiceBusTrigger("fetch_transcription_queue", Connection = "AzureServiceBus")]string message)
        {
            ArgumentNullException.ThrowIfNull(this.logger, nameof(this.logger));

            this.logger.LogInformation($"C# Isolated Service bus triggered function executed at: {DateTime.Now}");

            if (string.IsNullOrEmpty(message))
            {
                this.logger.LogInformation($"Found invalid service bus message: {message}. Stopping execution.");
                return;
            }

            var serviceBusMessage = TranscriptionStartedMessage.DeserializeMessage(message);

            var databaseContext = this.appConfig.UseSqlDatabase ? this.serviceProvider.GetRequiredService<IngestionClientDbContext>() : null;

            var transcriptionProcessor = new TranscriptionProcessor(
                this.storageConnector,
                this.serviceBusClientFactory,
                databaseContext,
                this.batchClient,
                Options.Create(this.appConfig));

            await transcriptionProcessor.ProcessTranscriptionJobAsync(serviceBusMessage, this.serviceProvider,  this.logger).ConfigureAwait(false);
        }
    }
}
