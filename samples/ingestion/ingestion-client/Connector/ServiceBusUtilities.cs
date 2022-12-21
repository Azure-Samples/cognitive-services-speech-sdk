// <copyright file="ServiceBusUtilities.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;
    using Microsoft.Extensions.Logging;

    public static class ServiceBusUtilities
    {
        public static async Task SendServiceBusMessageAsync(ServiceBusSender queueSender, string messageContent, ILogger log, TimeSpan delay)
        {
            var message = new Azure.Messaging.ServiceBus.ServiceBusMessage(messageContent);
            await SendServiceBusMessageAsync(queueSender, message, log, delay).ConfigureAwait(false);
        }

        public static async Task SendServiceBusMessageAsync(ServiceBusSender queueSender, Azure.Messaging.ServiceBus.ServiceBusMessage message, ILogger log, TimeSpan delay)
        {
            if (queueSender == null)
            {
                throw new ArgumentNullException(nameof(queueSender));
            }

            log.LogInformation($"Sending message with delay of {delay.TotalMinutes} minutes.");
            await queueSender.ScheduleMessageAsync(message, DateTimeOffset.Now.Add(delay)).ConfigureAwait(false);
        }
    }
}
