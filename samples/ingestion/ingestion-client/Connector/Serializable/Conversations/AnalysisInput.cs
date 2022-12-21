// <copyright file="AnalysisInput.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class AnalysisInput
    {
        public AnalysisInput(IList<Conversation> conversations)
        {
            this.Conversations = conversations;
        }

        [JsonProperty("conversations")]
        public IList<Conversation> Conversations { get; internal set; }
    }
}
