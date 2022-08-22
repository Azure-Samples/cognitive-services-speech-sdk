// <copyright file="AnalyzeConversationsTask.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Converters;

    public class AnalyzeConversationsTask
    {
        [JsonProperty("taskName")]
        public string TaskName { get; set; }

        [JsonProperty("kind")]
        [JsonConverter(typeof(StringEnumConverter))]
        public AnalyzeConversationsTaskKind Kind { get; set; }

        [JsonProperty("parameters")]
#pragma warning disable CA2227 // Collection properties should be read only
        public IDictionary<string, object> Parameters { get; set; }
#pragma warning restore CA2227 // Collection properties should be read only
    }
}