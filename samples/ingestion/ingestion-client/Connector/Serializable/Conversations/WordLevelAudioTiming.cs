// <copyright file="WordLevelAudioTiming.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using Newtonsoft.Json;

    public class WordLevelAudioTiming : AudioTiming
    {
        [JsonProperty("word")]
        public string Word { get; set; }
    }
}