//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Microsoft.CognitiveServices.Speech;
using System.Text.Json;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    internal class RemoteTranscriptionModel : IJsonDeserializable
    {
        public string UserId { get; set; }
        public string ResultId { get; set; }
        public ResultReason reason { get; set; }
        public string Text { get; set; }
        public long DurationInTicks { get; set; }
        public long OffsetInTicks { get; set; }

        public void ReadProperties(JsonElement json)
        {
            foreach (JsonProperty prop in json.EnumerateObject())
            {
                switch (prop.Name)
                {
                    case UserIdPropertyName:
                        UserId = prop.Value.ToString();
                        break;
                    case ResultIdPropertyName:
                        ResultId = prop.Value.ToString();
                        break;
                    case ReasonPropertyName:
                        Enum.TryParse(prop.Value.ToString(), out ResultReason local_reason);
                        reason = local_reason;
                        break;
                    case TextPropertyName:
                        Text = prop.Value.ToString();
                        break;
                    case DurationPropertyName:
                        DurationInTicks = prop.Value.GetInt64();
                        break;
                    case OffsetInTicksPropertyName:
                        OffsetInTicks = prop.Value.GetInt64();
                        break;
                }
            }
        }

        private const string UserIdPropertyName = "userId";
        private const string ResultIdPropertyName = "resultId";
        private const string ReasonPropertyName = "reason";
        private const string TextPropertyName = "text";
        private const string DurationPropertyName = "durationInTicks";
        private const string OffsetInTicksPropertyName = "offsetInTicks";
    }
}
