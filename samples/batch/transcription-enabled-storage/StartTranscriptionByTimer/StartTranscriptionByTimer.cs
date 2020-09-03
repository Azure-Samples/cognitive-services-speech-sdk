// <copyright file="StartTranscriptionByTimer.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Connector;
    using Microsoft.Azure.ServiceBus;
    using Microsoft.Azure.ServiceBus.Core;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;

    public static class StartTranscriptionByTimer
    {
        private const int MaxMessagesPerRequest = 500;

        private const double MessageReceiveTimeoutInSeconds = 60;

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

            var startDateTime = DateTime.UtcNow;
            log.LogInformation($"C# Timer trigger function v3 executed at: {startDateTime}. Next occurrence on {myTimer.Schedule.GetNextOccurrence(startDateTime)}.");

            if (!string.IsNullOrEmpty(StartTranscriptionEnvironmentVariables.SecondaryLocale) && !StartTranscriptionEnvironmentVariables.SecondaryLocale.Equals("None", StringComparison.OrdinalIgnoreCase))
            {
                log.LogWarning($"Language identification is not supported for timer-based transcription creation.");
            }

            var receiver = new MessageReceiver(StartTranscriptionEnvironmentVariables.AzureServiceBus, "start_transcription_queue");
            var validServiceBusMessages = new List<Message>();
            var transcriptionHelper = new StartTranscriptionHelper(log);

            var receiveFromServiceBus = true;
            log.LogInformation("Pulling messages from queue...");
            do
            {
                var messages = await receiver.ReceiveAsync(MaxMessagesPerRequest, TimeSpan.FromSeconds(30)).ConfigureAwait(false);

                if (messages == null)
                {
                    log.LogInformation("No remaining messages found in queue.");
                    break;
                }

                foreach (var message in messages)
                {
                    await receiver.CompleteAsync(message.SystemProperties.LockToken).ConfigureAwait(false);
                    if (transcriptionHelper.IsValidServiceBusMessage(message))
                    {
                        validServiceBusMessages.Add(message);
                    }
                }

                if (startDateTime.AddSeconds(MessageReceiveTimeoutInSeconds) > DateTime.UtcNow)
                {
                    log.LogInformation("Receiving messages from queue timed out.");
                    receiveFromServiceBus = false;
                }
                else if (messages.Count < MaxMessagesPerRequest)
                {
                    log.LogInformation("Received all remaining messages from the queue.");
                    receiveFromServiceBus = false;
                }
            }
            while (receiveFromServiceBus);

            if (!validServiceBusMessages.Any())
            {
                log.LogInformation("No valid messages were found in this function execution.");
                return;
            }

            log.LogInformation($"Received {validServiceBusMessages.Count} valid messages in total from queue.");

            await transcriptionHelper.StartTranscriptionsAsync(validServiceBusMessages).ConfigureAwait(false);
        }
    }
}
