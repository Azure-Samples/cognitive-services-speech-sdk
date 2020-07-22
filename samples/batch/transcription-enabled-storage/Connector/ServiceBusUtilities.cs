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
        public static async Task SendServiceBusMessageAsync(string serviceBusConnectionString, string messageContent, ILogger log, double delayInSeconds = 0d)
        {
            var message = new Message(Encoding.UTF8.GetBytes(messageContent));
            log.LogInformation($"Sending message: {messageContent}");

            await SendServiceBusMessageAsync(serviceBusConnectionString, message, log, delayInSeconds).ConfigureAwait(false);
        }

        public static async Task SendServiceBusMessageAsync(string serviceBusConnectionString, Message message, ILogger log, double delayInSeconds = 0d)
        {
            var queueClient = new QueueClient(new ServiceBusConnectionStringBuilder(serviceBusConnectionString));
            if (delayInSeconds == 0d)
            {
                await queueClient.SendAsync(message).ConfigureAwait(false);
            }
            else
            {
                log.LogInformation($"Delaying message for {(int)delayInSeconds} seconds.");
                await queueClient.ScheduleMessageAsync(message, DateTimeOffset.Now.AddSeconds(delayInSeconds)).ConfigureAwait(false);
            }
        }
    }
}
