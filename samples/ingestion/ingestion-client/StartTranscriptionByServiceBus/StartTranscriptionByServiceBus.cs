// <copyright file="StartTranscriptionByServiceBus.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscription
{
    using System;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;
    using Connector.Enums;

    using Microsoft.Azure.WebJobs;
    using Microsoft.Extensions.Azure;
    using Microsoft.Extensions.Logging;
    using StartTranscriptionByTimer;

    public class StartTranscriptionByServiceBus
    {
        private readonly ServiceBusSender startTranscriptionServiceBusSender;

        private readonly ServiceBusReceiver startTranscriptionServiceBusReceiver;

        private readonly ServiceBusSender fetchTranscriptionServiceBusSender;

        /// <summary>
        /// Initializes a new instance of the <see cref="StartTranscriptionByServiceBus"/> class.
        /// </summary>
        /// <param name="serviceBusClientFactory">serviceBusClientFactory from Dependency Injection</param>
        public StartTranscriptionByServiceBus(IAzureClientFactory<ServiceBusClient> serviceBusClientFactory)
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
            this.fetchTranscriptionServiceBusSender = fetchTranscriptionServiceBusClient.CreateSender(fetchTranscriptionQueueName);
        }

        [FunctionName("StartTranscriptionByServiceBus")]
        public async Task Run([ServiceBusTrigger("start_transcription_queue", Connection = "AzureServiceBus")]ServiceBusReceivedMessage message, ILogger log)
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

            var transcriptionHelper = new StartTranscriptionHelper(
                log,
                this.startTranscriptionServiceBusSender,
                this.startTranscriptionServiceBusReceiver,
                this.fetchTranscriptionServiceBusSender);

            if (message == null || !transcriptionHelper.IsValidServiceBusMessage(message))
            {
                log.LogInformation($"Service bus message is invalid.");
                return;
            }

            await transcriptionHelper.StartTranscriptionAsync(message).ConfigureAwait(false);
        }
    }
}
