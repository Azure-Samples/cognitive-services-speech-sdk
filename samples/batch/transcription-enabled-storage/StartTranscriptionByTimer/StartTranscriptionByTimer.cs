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
    using Microsoft.Azure.ServiceBus;
    using Microsoft.Azure.ServiceBus.Core;
    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Logging;

    public static class StartTranscriptionByTimer
    {
        private const double MessageReceiveTimeoutInSeconds = 60;

        private static MessageReceiver MessageReceiverInstance = new MessageReceiver(new ServiceBusConnectionStringBuilder(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString), prefetchCount: StartTranscriptionEnvironmentVariables.MessagesPerFunctionExecution);

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

            var validServiceBusMessages = new List<Message>();
            var transcriptionHelper = new StartTranscriptionHelper(log);

            log.LogInformation("Pulling messages from queue...");

            var messages = await MessageReceiverInstance.ReceiveAsync(StartTranscriptionEnvironmentVariables.MessagesPerFunctionExecution, TimeSpan.FromSeconds(MessageReceiveTimeoutInSeconds)).ConfigureAwait(false);

            if (messages == null || !messages.Any())
            {
                log.LogInformation($"Got no messages in this iteration.");
                return;
            }

            log.LogInformation($"Got {messages.Count} in this iteration.");
            foreach (var message in messages)
            {
                if (message.SystemProperties.LockedUntilUtc > DateTime.UtcNow.AddSeconds(5))
                {
                    try
                    {
                        if (transcriptionHelper.IsValidServiceBusMessage(message))
                        {
                            await MessageReceiverInstance.RenewLockAsync(message.SystemProperties.LockToken).ConfigureAwait(false);
                            validServiceBusMessages.Add(message);
                        }
                        else
                        {
                            await MessageReceiverInstance.CompleteAsync(message.SystemProperties.LockToken).ConfigureAwait(false);
                        }
                    }
                    catch (MessageLockLostException)
                    {
                        log.LogInformation($"Message lock expired for message. Ignore message in this iteration.");
                    }
                }
            }

            if (!validServiceBusMessages.Any())
            {
                log.LogInformation("No valid messages were found in this function execution.");
                return;
            }

            log.LogInformation($"Pulled {validServiceBusMessages.Count} valid messages from queue.");

            await transcriptionHelper.StartTranscriptionsAsync(validServiceBusMessages, MessageReceiverInstance, startDateTime).ConfigureAwait(false);
        }
    }
}
