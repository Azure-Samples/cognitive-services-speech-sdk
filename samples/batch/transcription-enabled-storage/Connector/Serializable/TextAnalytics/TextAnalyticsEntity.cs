// <copyright file="TextAnalyticsEntity.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    using System;
    using Connector.Enums;

    public class TextAnalyticsEntity
    {
        public TextAnalyticsEntity(string text, string category, string subCategory, int offset, int length, float score)
        {
            Text = text;
            CategoryString = category;
            SubCategory = subCategory;
            Offset = offset;
            Length = length;
            Score = score;
        }

        public string Text { get; private set; }

        public string SubCategory { get; private set; }

        public int Offset { get; private set; }

        public int Length { get; private set; }

        public float Score { get; private set; }

        public EntityCategory Category
        {
            get
            {
                if (Enum.TryParse(CategoryString, out EntityCategory outputType))
                {
                    return outputType;
                }

                return EntityCategory.Unknown;
            }
        }

        private string CategoryString { get; set; }
    }
}
