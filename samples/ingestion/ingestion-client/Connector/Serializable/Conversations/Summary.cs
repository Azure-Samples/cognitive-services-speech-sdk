// <copyright file="Summary.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using Newtonsoft.Json;

    public enum SummaryAspect
    {
        Unknown = 0,
        Issue,
        Resolution,
    }

    public class Summary
    {
        [JsonProperty(PropertyName = "aspect")]
        public SummaryAspect Aspect { get; set; }

        [JsonProperty(PropertyName = "text")]
        public string Text { get; set; }
    }
}
