//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Collections.Generic;
using System.Text.Json;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    enum RemoteTaskStatusModel
    {
        Uploading,
        Processing,
        Completed,
        Failed,
        Deleted
    }

    internal class RemoteConversationTranscriptionModel : IJsonDeserializable
    {
        internal string Id { get; set; }

        internal List<RemoteTranscriptionModel> Transcriptions { get; set; }

        internal RemoteTaskStatusModel ProcessingStatus { get; set; }

        internal RemoteConversationTranscriptionModel()
        {
            Transcriptions = new List<RemoteTranscriptionModel>();
        }

        public void ReadProperties(JsonElement json)
        {
            foreach (JsonProperty prop in json.EnumerateObject())
            {
                if (prop.Value.ValueKind != JsonValueKind.Null && prop.Value.ValueKind != JsonValueKind.Undefined)
                {
                    switch (prop.Name)
                    {
                        case IdPropertyName:
                            Id = prop.Value.ToString();
                            break;

                        case TranscriptionsPropertyName:
                            var trans = prop.Value.EnumerateArray();
                            foreach (JsonElement t in trans)
                            {
                                RemoteTranscriptionModel model = new RemoteTranscriptionModel();
                                model.ReadProperties(t);
                                Transcriptions.Add(model);
                            }
                            break;

                        case ProcessingStatusPropertyName:
                            ProcessingStatus = (RemoteTaskStatusModel)prop.Value.GetUInt32();
                            break;
                    }
                }
            }
        }

        private const string IdPropertyName = "id";
        private const string TranscriptionsPropertyName = "transcriptions";
        private const string ProcessingStatusPropertyName = "processingStatus";
    }
}
