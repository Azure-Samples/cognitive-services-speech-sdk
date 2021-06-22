// <copyright file="TextAnalyticsRequest.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable
{
    public class TextAnalyticsRequest
    {
        public TextAnalyticsRequest(string language, string id, string text)
        {
            Language = language;
            Id = id;
            Text = text;
        }

        public string Language { get; private set; }

        public string Id { get; private set; }

        public string Text { get; private set; }
    }
}
