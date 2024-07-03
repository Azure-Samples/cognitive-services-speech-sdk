// <copyright file="FetchTranscription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System;
    using System.Threading.Tasks;
    using Connector;

    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;

    public class FetchTranscription
    {
        private readonly IServiceProvider serviceProvider;
        private readonly IStorageConnector storageConnector;

        public FetchTranscription(IServiceProvider serviceProvider, IStorageConnector storageConnector)
        {
            this.serviceProvider = serviceProvider;
            this.storageConnector = storageConnector;
        }

        [FunctionName("FetchTranscription")]
        public async Task Run([ServiceBusTrigger("fetch_transcription_queue", Connection = "AzureServiceBus")]string message, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            log.LogInformation($"C# Service bus triggered function executed at: {DateTime.Now}");

            if (string.IsNullOrEmpty(message))
            {
                log.LogInformation($"Found invalid service bus message: {message}. Stopping execution.");
                return;
            }

            var serviceBusMessage = TranscriptionStartedMessage.DeserializeMessage(message);

            var transcriptionProcessor = new TranscriptionProcessor(this.serviceProvider, this.storageConnector);

            await transcriptionProcessor.ProcessTranscriptionJobAsync(serviceBusMessage, this.serviceProvider,  log).ConfigureAwait(false);
        }
    }
}
