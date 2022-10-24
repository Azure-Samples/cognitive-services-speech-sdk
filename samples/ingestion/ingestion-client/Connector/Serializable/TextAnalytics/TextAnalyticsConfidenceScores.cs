// <copyright file="TextAnalyticsConfidenceScores.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    public class TextAnalyticsConfidenceScores
    {
        public TextAnalyticsConfidenceScores(float positive, float neutral, float negative)
        {
            this.Positive = positive;
            this.Neutral = neutral;
            this.Negative = negative;
        }

        public float Positive { get; private set; }

        public float Neutral { get; private set; }

        public float Negative { get; private set; }
    }
}
