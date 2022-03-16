// <copyright file="CostEstimation.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using Connector.Enums;

    public static class CostEstimation
    {
        // prices in euro
        private const double STTCostPerHour = 0.844d;

        private const double STTCustomModelCostPerHour = 1.181d;

        private const double TextAnalyticsCostPerHour = 0.72d;

        public static double GetCostEstimation(
            TimeSpan timeSpan,
            int numberOfChannels,
            bool isCustomModel,
            SentimentAnalysisSetting sentimentSetting,
            PiiRedactionSetting entityRedactionSetting)
        {
            double costPerHour = isCustomModel ? STTCustomModelCostPerHour : STTCostPerHour;
            var price = timeSpan.TotalHours * costPerHour;

            if (sentimentSetting != SentimentAnalysisSetting.None)
            {
                price += timeSpan.TotalHours * TextAnalyticsCostPerHour;
            }

            if (entityRedactionSetting != PiiRedactionSetting.None)
            {
                price += timeSpan.TotalHours * TextAnalyticsCostPerHour;
            }

            price *= numberOfChannels;
            return price;
        }
    }
}
