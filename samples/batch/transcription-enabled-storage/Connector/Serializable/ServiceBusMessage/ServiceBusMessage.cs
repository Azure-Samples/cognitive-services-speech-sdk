// <copyright file="ServiceBusMessage.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public class ServiceBusMessage
    {
        public string Topic { get; set; }

        public string Subject { get; set; }

        public string EventType { get; set; }

        public DateTime EventTime { get; set; }

        public string Id { get; set; }

        public Data Data { get; set; }

        public string DataVersion { get; set; }

        public string MetadataVersion { get; set; }
    }
}
