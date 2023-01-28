// <copyright file="Transcription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public class Transcription
    {
        public Transcription(string self, string displayName, string description, string locale, TranscriptionProperties properties, DateTime createdDateTime, DateTime lastActionDateTime, string status)
        {
            Self = self;
            DisplayName = displayName;
            Description = description;
            Locale = locale;
            Properties = properties;
            CreatedDateTime = createdDateTime;
            LastActionDateTime = lastActionDateTime;
            Status = status;
        }

        public string Self { get; set; }

        public string DisplayName { get; }

        public string Description { get; set; }

        public string Locale { get; set; }

        public TranscriptionProperties Properties { get; set; }

        public DateTime CreatedDateTime { get; set; }

        public DateTime LastActionDateTime { get; set; }

        public string Status { get; set; }
    }
}
