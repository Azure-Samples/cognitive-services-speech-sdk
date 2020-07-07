//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.IO;
using System.Text.Json;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    internal interface IJsonSerializable
    {
        void WriteProperties(Utf8JsonWriter json);
    }

    internal interface IJsonDeserializable
    {
        void ReadProperties(JsonElement json);
    }

    internal static class SerializationExtensions
    {
        public static void Deserialize(this IJsonDeserializable obj, Stream content)
        {
            using (JsonDocument json = JsonDocument.Parse(content, default))
            {
                obj.ReadProperties(json.RootElement);
            }
        }
    }
}
