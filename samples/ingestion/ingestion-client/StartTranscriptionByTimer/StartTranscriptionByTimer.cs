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
    using Azure.Messaging.ServiceBus;
    using Connector.Enums;

    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.Logging;

    public class StartTranscriptionByTimer
    {
        private const double MessageReceiveTimeoutInSeconds = 60;

        private readonly ServiceBusReceiver startTranscriptionServiceBusReceiver;

        private readonly ServiceBusSender startTranscriptionServiceBusSender;

        private readonly ServiceBusSender fetchTranscriptionServiceBusSender;

        /// <summary>
        /// Initializes a new instance of the <see cref="StartTranscriptionByTimer"/> class.
        /// </summary>
        /// <param name="serviceBusClientFactory">serviceBusClientFactory from Dependency Injection</param>
        public StartTranscriptionByTimer(IAzureClientFactory<ServiceBusClient> serviceBusClientFactory)
        {
            serviceBusClientFactory = serviceBusClientFactory ?? throw new ArgumentNullException(nameof(serviceBusClientFactory));
            var startTranscriptionServiceBusClient = serviceBusClientFactory.CreateClient(ServiceBusClientName.StartTranscriptionServiceBusClient.ToString());

            var startTranscriptionQueueName = ServiceBusConnectionStringProperties.Parse(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString).EntityPath;
            var receiverOptions = new ServiceBusReceiverOptions()
            {
                PrefetchCount = StartTranscriptionEnvironmentVariables.MessagesPerFunctionExecution
            };
            this.startTranscriptionServiceBusReceiver = startTranscriptionServiceBusClient.CreateReceiver(startTranscriptionQueueName, receiverOptions);
            this.startTranscriptionServiceBusSender = startTranscriptionServiceBusClient.CreateSender(startTranscriptionQueueName);

            var fetchTranscriptionServiceBusClient = serviceBusClientFactory.CreateClient(ServiceBusClientName.FetchTranscriptionServiceBusClient.ToString());
            var fetchTranscriptionQueueName = ServiceBusConnectionStringProperties.Parse(StartTranscriptionEnvironmentVariables.FetchTranscriptionServiceBusConnectionString).EntityPath;
            this.fetchTranscriptionServiceBusSender = startTranscriptionServiceBusClient.CreateSender(fetchTranscriptionQueueName);
        }

        [FunctionName("StartTranscriptionByTimer")]
        public async Task Run([TimerTrigger("%StartTranscriptionFunctionTimeInterval%")] TimerInfo timerInfo, ILogger log)
        {
            if (log == null)
            {
                throw new ArgumentNullException(nameof(log));
            }

            if (timerInfo == null)
            {
                throw new ArgumentNullException(nameof(timerInfo));
            }

            var startDateTime = DateTime.UtcNow;
            log.LogInformation($"C# Timer trigger function v3 executed at: {startDateTime}. Next occurrence on {timerInfo.Schedule.GetNextOccurrence(startDateTime)}.");

            var validServiceBusMessages = new List<ServiceBusReceivedMessage>();
            var transcriptionHelper = new StartTranscriptionHelper(
                log,
                this.startTranscriptionServiceBusSender,
                this.startTranscriptionServiceBusReceiver,
                this.fetchTranscriptionServiceBusSender);

            log.LogInformation("Pulling messages from queue...");
            var messages = await this.startTranscriptionServiceBusReceiver.ReceiveMessagesAsync(StartTranscriptionEnvironmentVariables.MessagesPerFunctionExecution, TimeSpan.FromSeconds(MessageReceiveTimeoutInSeconds)).ConfigureAwait(false);

            if (messages == null || !messages.Any())
            {
                log.LogInformation($"Got no messages in this iteration.");
                return;
            }

            log.LogInformation($"Got {messages.Count} in this iteration.");
            foreach (var message in messages)
            {
                if (message.LockedUntil > DateTime.UtcNow.AddSeconds(5))
                {
                    try
                    {
                        if (transcriptionHelper.IsValidServiceBusMessage(message))
                        {
                            await this.startTranscriptionServiceBusReceiver.RenewMessageLockAsync(message).ConfigureAwait(false);
                            validServiceBusMessages.Add(message);
                        }
                        else
                        {
                            await this.startTranscriptionServiceBusReceiver.CompleteMessageAsync(message).ConfigureAwait(false);
                        }
                    }
                    catch (ServiceBusException ex) when (ex.Reason == ServiceBusFailureReason.MessageLockLost)
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

            await transcriptionHelper.StartTranscriptionsAsync(validServiceBusMessages, startDateTime).ConfigureAwait(false);
        }
    }
}
