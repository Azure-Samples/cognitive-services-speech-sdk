// <copyright file="TextAnalyticsDocument.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    using System.Collections.Generic;

    public class TextAnalyticsDocument
    {
        public TextAnalyticsDocument(string id, string redactedText, TextAnalyticsConfidenceScores confidenceScores, IEnumerable<TextAnalyticsEntity> entities)
        {
            this.Id = id;
            this.RedactedText = redactedText;
            this.ConfidenceScores = confidenceScores;
            this.Entities = entities;
        }

        public string Id { get; set; }

        public string RedactedText { get; set; }

        public TextAnalyticsConfidenceScores ConfidenceScores { get; private set; }

        public IEnumerable<TextAnalyticsEntity> Entities { get; private set; }
    }
}
