// <copyright file="TextAnalyticsDocument.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    using System.Collections.Generic;

    public class TextAnalyticsDocument
    {
        public TextAnalyticsDocument(string id, List<TextAnalyticsSentence> sentences, List<TextAnalyticsEntity> entities)
        {
            Id = id;
            Sentences = sentences;
            Entities = entities;
        }

        public string Id { get; set; }

        public List<TextAnalyticsSentence> Sentences { get; private set; }

        public List<TextAnalyticsEntity> Entities { get; private set; }
    }
}
