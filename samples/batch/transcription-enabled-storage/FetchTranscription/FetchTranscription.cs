// <copyright file="FetchTranscription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscriptionFunction
{
    using System;
    using Connector;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;

    public static class FetchTranscription
    {
        [FunctionName("FetchTranscription")]
        public static void Run([ServiceBusTrigger("fetch_transcription_queue", Connection = "AzureServiceBus")]string myQueueItem, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            log.LogInformation($"C# Timer trigger function executed at: {DateTime.Now}");

            ProcessMessage(myQueueItem, log);
        }

        public static bool ProcessMessage(string message, ILogger log)
        {
            if (!string.IsNullOrEmpty(message))
            {
                var serviceBusMessage = PostTranscriptionServiceBusMessage.DeserializeMessage(message);
                bool result = TranscriptionProcessor.GetTranscripts(serviceBusMessage, log).Result;
                return result;
            }

            return false;
        }
    }
}
