// <copyright file="ServiceBusMessage.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    public class ServiceBusMessage
    {
        public string EventType { get; set; }

        public Data Data { get; set; }

        public int RetryCount { get; set; }
    }
}
