// <copyright file="StartTranscriptionByServiceBus.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscription
{
    using System;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;
    using Microsoft.Azure.Functions.Worker;
    using Microsoft.Extensions.Logging;
    using StartTranscriptionByTimer;

    /// <summary>
    /// Start Transcription by Service Bus class.
    /// </summary>
    public class StartTranscriptionByServiceBus
    {
        private readonly ILogger<StartTranscriptionByServiceBus> logger;

        /// <summary>
        /// Initializes a new instance of the <see cref="StartTranscriptionByServiceBus"/> class.
        /// </summary>
        /// <param name="logger">The StartTranscriptionByServiceBus Logger</param>
        public StartTranscriptionByServiceBus(ILogger<StartTranscriptionByServiceBus> logger)
        {
            this.logger = logger;
        }

        /// <summary>
        /// Starts the transcription process by processing a message from a Service Bus queue.
        /// </summary>
        /// <param name="message">The message to read from the queue</param>
        /// <returns>A <see cref="Task"/> representing the result of the asynchronous operation.</returns>
        [Function("StartTranscriptionByServiceBus")]
        public async Task Run([ServiceBusTrigger("start_transcription_queue", Connection = "AzureServiceBus")]ServiceBusReceivedMessage message)
        {
            ArgumentNullException.ThrowIfNull(this.logger, nameof(this.logger));
            ArgumentNullException.ThrowIfNull(message, nameof(message));

            this.logger.LogInformation($"C# Isolated ServiceBus queue trigger function processed message: {message.Subject}");
            this.logger.LogInformation($"Received message: SequenceNumber:{message.SequenceNumber} Body:{message.Body}");

            var transcriptionHelper = new StartTranscriptionHelper(this.logger);

            if (message == null || !transcriptionHelper.IsValidServiceBusMessage(message))
            {
                this.logger.LogInformation($"Service bus message is invalid.");
                return;
            }

            await transcriptionHelper.StartTranscriptionAsync(message).ConfigureAwait(false);
        }
    }
}
