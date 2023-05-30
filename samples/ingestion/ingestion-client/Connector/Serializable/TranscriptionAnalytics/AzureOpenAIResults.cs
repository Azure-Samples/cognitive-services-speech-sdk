// <copyright file="AzureOpenAIResults.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;

    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    public class AzureOpenAIResults
    {
        [JsonProperty("batchCompletionResults")]
        public IEnumerable<JObject> BatchCompletionResults { get; set; }
    }
}
