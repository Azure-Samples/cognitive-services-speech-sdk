// <copyright file="Tasks.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class Tasks
    {
        [JsonProperty("completed")]
        public long Completed { get; set; }

        [JsonProperty("failed")]
        public long Failed { get; set; }

        [JsonProperty("inProgress")]
        public long InProgress { get; set; }

        [JsonProperty("total")]
        public long Total { get; set; }

        [JsonProperty("items")]
        public IList<Item> Items { get; internal set; }
    }
}