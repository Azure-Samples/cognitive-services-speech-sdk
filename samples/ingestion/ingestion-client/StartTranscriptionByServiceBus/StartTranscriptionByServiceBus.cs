// <copyright file="StartTranscriptionByServiceBus.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscription
{
    using System;
    using System.Text;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;
    using StartTranscriptionByTimer;

    public static class StartTranscriptionByServiceBus
    {
        [FunctionName("StartTranscriptionByServiceBus")]
        public static async Task Run([ServiceBusTrigger("start_transcription_queue", Connection = "AzureServiceBus")]ServiceBusReceivedMessage message, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            if (message == null)
            {
                throw new ArgumentNullException(nameof(message));
            }

            log.LogInformation($"C# ServiceBus queue trigger function processed message: {message.Subject}");
            log.LogInformation($"Received message: SequenceNumber:{message.SequenceNumber} Body:{message.Body}");

            var transcriptionHelper = new StartTranscriptionHelper(log);

            if (message == null || !transcriptionHelper.IsValidServiceBusMessage(message))
            {
                log.LogInformation($"Service bus message is invalid.");
                return;
            }

            await transcriptionHelper.StartTranscriptionAsync(message).ConfigureAwait(false);
        }
    }
}
