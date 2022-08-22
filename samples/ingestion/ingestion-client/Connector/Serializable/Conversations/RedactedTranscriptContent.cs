// <copyright file="RedactedTranscriptContent.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class RedactedTranscriptContent
    {
        /// <summary>
        /// Gets or sets PII Redacted Itn version of the transcript input.
        /// </summary>
        [JsonProperty(PropertyName = "itn")]
        public string Itn
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or Sets PII Redacted lexical version of the transcript input.
        /// </summary>
        [JsonProperty(PropertyName = "lexical")]
        public string Lexical
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or sets PII redacted maskedItn version of the transcript input.
        /// </summary>
        [JsonProperty(PropertyName = "maskedItn")]
        public string MaskedItn
        {
            get;
            set;
        }

        /// <summary>
        /// Gets or Sets PII redacted display version of the transcript input.
        /// </summary>
        [JsonProperty(PropertyName = "text")]
        public string Text
        {
            get;
            set;
        }

        /// <summary>
        /// Gets  Redacted audio timing information for speech input.
        /// </summary>
        [JsonProperty(PropertyName = "redactedAudioTimings")]
        public IList<AudioTiming> RedactedAudioTimings
        {
            get;
            internal set;
        }
    }
}