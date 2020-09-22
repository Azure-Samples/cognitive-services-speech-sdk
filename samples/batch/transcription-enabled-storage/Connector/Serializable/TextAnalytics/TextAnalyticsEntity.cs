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
        public TextAnalyticsEntity(string text, string type, string subType, int offset, int length, float score)
        {
            Text = text;
            TypeString = type;
            SubType = subType;
            Offset = offset;
            Length = length;
            Score = score;
        }

        public string Text { get; private set; }

        public string SubType { get; private set; }

        public int Offset { get; private set; }

        public int Length { get; private set; }

        public float Score { get; private set; }

        public EntityType Type
        {
            get
            {
                if (Enum.TryParse(TypeString, out EntityType outputType))
                {
                    return outputType;
                }

                return EntityType.Unknown;
            }
        }

        private string TypeString { get; set; }
    }
}
