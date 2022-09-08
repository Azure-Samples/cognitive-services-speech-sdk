// <copyright file="AudioTiming.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using Newtonsoft.Json;

    public class AudioTiming
    {
        /// <summary>
        /// Gets or sets offset from start of speech audio, in ticks.
        /// </summary>
        [JsonProperty(PropertyName = "offset")]
        public long Offset
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets duration of word articulation, in ticks.
        /// </summary>
        [JsonProperty(PropertyName = "duration")]
        public long Duration
        {
            get;
            set;
        }
    }
}