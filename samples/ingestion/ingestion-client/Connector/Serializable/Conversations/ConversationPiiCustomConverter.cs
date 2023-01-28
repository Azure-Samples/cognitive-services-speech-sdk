// <copyright file="ConversationPiiCustomConverter.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System;
    using Newtonsoft.Json;
    using Newtonsoft.Json.Linq;

    public class ConversationPiiCustomConverter : JsonConverter
    {
        public override bool CanWrite => false;

        public override bool CanConvert(Type objectType)
        {
            return typeof(ConversationPiiResultItem).IsAssignableFrom(objectType);
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            var jo = JObject.Load(reader);
            var response = JsonConvert.DeserializeObject<ConversationPiiResultItem>(jo.ToString());
            var values = response.Id.Split("__");

            if (values != null && values.Length == 3)
            {
                response.Id = values[0];
                response.Channel = values[2];
                response.Offset = values[1];
            }

            return response;
        }

        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
            throw new NotImplementedException();
        }
    }
}
