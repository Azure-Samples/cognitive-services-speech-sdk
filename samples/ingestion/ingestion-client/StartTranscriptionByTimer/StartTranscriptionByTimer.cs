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
    using Microsoft.Azure.Functions.Worker;
    using Microsoft.Extensions.Logging;

    /// <summary>
    /// Start Transcription By Timer class.
    /// </summary>
    public class StartTranscriptionByTimer
    {
        private const double MessageReceiveTimeoutInSeconds = 60;

        private static readonly ServiceBusClient ServiceBusClient = new ServiceBusClient(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString);

        private static readonly ServiceBusReceiverOptions ServiceBusReceiverOptions = new ServiceBusReceiverOptions() { PrefetchCount = StartTranscriptionEnvironmentVariables.MessagesPerFunctionExecution };

        private static readonly ServiceBusReceiver ServiceBusReceiver = ServiceBusClient.CreateReceiver(ServiceBusConnectionStringProperties.Parse(StartTranscriptionEnvironmentVariables.StartTranscriptionServiceBusConnectionString).EntityPath, ServiceBusReceiverOptions);

        private readonly ILogger<StartTranscriptionByTimer> logger;

        /// <summary>
        /// Initializes a new instance of the <see cref="StartTranscriptionByTimer"/> class.
        /// </summary>
        /// <param name="logger">The StartTranscriptionByTimer logger</param>
        public StartTranscriptionByTimer(ILogger<StartTranscriptionByTimer> logger)
        {
            this.logger = logger;
        }

        /// <summary>
        /// Run method to start transcription by timer.
        /// </summary>
        /// <param name="timerInfo"></param>
        /// <returns></returns>
        [Function("StartTranscriptionByTimer")]
        public async Task Run([TimerTrigger("%StartTranscriptionFunctionTimeInterval%")] TimerInfo timerInfo)
        {
            ArgumentNullException.ThrowIfNull(this.logger, nameof(this.logger));
            ArgumentNullException.ThrowIfNull(timerInfo, nameof(timerInfo));

            var startDateTime = DateTime.UtcNow;
            this.logger.LogInformation($"C# Isolated Timer trigger function v4 executed at: {startDateTime}. Next occurrence on {timerInfo.ScheduleStatus.Next}.");

            var validServiceBusMessages = new List<ServiceBusReceivedMessage>();
            var transcriptionHelper = new StartTranscriptionHelper(this.logger);

            this.logger.LogInformation("Pulling messages from queue...");
            var messages = await ServiceBusReceiver.ReceiveMessagesAsync(StartTranscriptionEnvironmentVariables.MessagesPerFunctionExecution, TimeSpan.FromSeconds(MessageReceiveTimeoutInSeconds)).ConfigureAwait(false);

            if (messages == null || !messages.Any())
            {
                this.logger.LogInformation($"Got no messages in this iteration.");
                return;
            }

            this.logger.LogInformation($"Got {messages.Count} in this iteration.");
            foreach (var message in messages)
            {
                if (message.LockedUntil > DateTime.UtcNow.AddSeconds(5))
                {
                    try
                    {
                        if (transcriptionHelper.IsValidServiceBusMessage(message))
                        {
                            await ServiceBusReceiver.RenewMessageLockAsync(message).ConfigureAwait(false);
                            validServiceBusMessages.Add(message);
                        }
                        else
                        {
                            await ServiceBusReceiver.CompleteMessageAsync(message).ConfigureAwait(false);
                        }
                    }
                    catch (ServiceBusException ex) when (ex.Reason == ServiceBusFailureReason.MessageLockLost)
                    {
                        this.logger.LogInformation($"Message lock expired for message. Ignore message in this iteration.");
                    }
                }
            }

            if (!validServiceBusMessages.Any())
            {
                this.logger.LogInformation("No valid messages were found in this function execution.");
                return;
            }

            this.logger.LogInformation($"Pulled {validServiceBusMessages.Count} valid messages from queue.");

            await transcriptionHelper.StartTranscriptionsAsync(validServiceBusMessages, ServiceBusReceiver, startDateTime).ConfigureAwait(false);
        }
    }
}
