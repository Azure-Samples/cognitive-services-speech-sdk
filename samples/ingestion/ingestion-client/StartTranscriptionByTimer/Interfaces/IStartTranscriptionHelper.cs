// <copyright file="IStartTranscriptionHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscriptionByTimer
{
    using System;
    using System.Collections.Generic;
    using System.Threading.Tasks;
    using Azure.Messaging.ServiceBus;

    public interface IStartTranscriptionHelper
    {
        Task StartTranscriptionsAsync(IEnumerable<ServiceBusReceivedMessage> messages, DateTime startDateTime);

        Task StartTranscriptionAsync(ServiceBusReceivedMessage message);

        bool IsValidServiceBusMessage(ServiceBusReceivedMessage message);
    }
}