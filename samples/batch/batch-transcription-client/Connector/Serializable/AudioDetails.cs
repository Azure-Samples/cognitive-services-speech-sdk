// <copyright file="AudioDetails.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public class AudioDetails
    {
        public AudioDetails(string fileName, DateTime createdTime, TimeSpan audioLength, int channels, double estimatedCost, string locale)
        {
            FileName = fileName;
            CreatedTime = createdTime;
            AudioLength = audioLength;
            Channels = channels;
            EstimatedCost = estimatedCost;
            Locale = locale;
        }

        public string FileName { get; set; }

        public DateTime CreatedTime { get; set; }

        public TimeSpan AudioLength { get; set; }

        public int Channels { get; set; }

        public double EstimatedCost { get; set; }

        public string Locale { get; set; }
    }
}
