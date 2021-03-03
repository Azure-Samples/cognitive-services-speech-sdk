// <copyright file="FetchTranscription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using System.Threading.Tasks;
    using Connector;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;

    public static class FetchTranscription
    {
        [FunctionName("FetchTranscription")]
        public static async Task Run([ServiceBusTrigger("fetch_transcription_queue", Connection = "AzureServiceBus")]string message, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            log.LogInformation($"C# Timer trigger function executed at: {DateTime.Now}");

            if (string.IsNullOrEmpty(message))
            {
                log.LogInformation($"Found invalid service bus message: {message}. Stopping execution.");
                return;
            }

            var serviceBusMessage = TranscriptionStartedMessage.DeserializeMessage(message);
            await TranscriptionProcessor.ProcessTranscriptionJobAsync(serviceBusMessage, log).ConfigureAwait(false);
        }
    }
}
