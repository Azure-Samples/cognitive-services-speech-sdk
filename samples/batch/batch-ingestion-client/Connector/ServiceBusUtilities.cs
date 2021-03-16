// <copyright file="ServiceBusUtilities.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.Azure.ServiceBus;
    using Microsoft.Extensions.Logging;

    public static class ServiceBusUtilities
    {
        public static async Task SendServiceBusMessageAsync(QueueClient queueClient, string messageContent, ILogger log, TimeSpan delay)
        {
            var message = new Message(Encoding.UTF8.GetBytes(messageContent));
            await SendServiceBusMessageAsync(queueClient, message, log, delay).ConfigureAwait(false);
        }

        public static async Task SendServiceBusMessageAsync(QueueClient queueClient, Message message, ILogger log, TimeSpan delay)
        {
            if (queueClient == null)
            {
                throw new ArgumentNullException(nameof(queueClient));
            }

            log.LogInformation($"Sending message with delay of {delay.TotalMinutes} minutes.");
            await queueClient.ScheduleMessageAsync(message, DateTimeOffset.Now.Add(delay)).ConfigureAwait(false);
        }
    }
}
