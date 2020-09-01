// <copyright file="StartTranscriptionByTimer.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;
    using Connector;
    using Microsoft.Azure.ServiceBus;
    using Microsoft.Azure.ServiceBus.Core;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;

    public static class StartTranscriptionByTimer
    {
        private const int MaxMessagesPerRequest = 500;

        [FunctionName("StartTranscriptionByTimer")]
        public static async Task Run([TimerTrigger("0 */2 * * * *")] TimerInfo myTimer, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            if (myTimer == null)
            {
                throw new ArgumentNullException(nameof(myTimer));
            }

            log.LogInformation($"C# Timer trigger function v3 executed at: {DateTime.UtcNow}. Next occurrence on {myTimer.Schedule.GetNextOccurrence(DateTime.UtcNow)}.");

            if (!string.IsNullOrEmpty(StartTranscriptionEnvironmentVariables.SecondaryLocale) && !StartTranscriptionEnvironmentVariables.SecondaryLocale.Equals("None", StringComparison.OrdinalIgnoreCase))
            {
                log.LogWarning($"Language identification is not supported for timer-based transcription creation.");
            }

            var receiver = new MessageReceiver(StartTranscriptionEnvironmentVariables.AzureServiceBus, "start_transcription_queue");
            var totalServiceBusMessages = new List<Message>();

            var serviceBusMessagesRemaining = true;
            log.LogInformation("Pulling messages from queue...");
            do
            {
                var messages = await receiver.ReceiveAsync(MaxMessagesPerRequest, TimeSpan.FromSeconds(20)).ConfigureAwait(false);

                if (messages == null)
                {
                    log.LogInformation("No remaining messages found in queue.");
                    break;
                }

                foreach (var message in messages)
                {
                    await receiver.CompleteAsync(message.SystemProperties.LockToken).ConfigureAwait(false);
                    totalServiceBusMessages.Add(message);
                }

                if (messages.Count < MaxMessagesPerRequest)
                {
                    serviceBusMessagesRemaining = false;
                }
            }
            while (serviceBusMessagesRemaining);

            log.LogInformation($"Received {totalServiceBusMessages.Count} messages in total from queue.");

            var transcriptionHelper = new StartTranscriptionHelper(log);
            await transcriptionHelper.StartTranscriptionsAsync(totalServiceBusMessages).ConfigureAwait(false);
        }
    }
}
