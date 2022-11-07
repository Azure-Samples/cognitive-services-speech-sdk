// <copyright file="Transcription.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database.Models
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.DataAnnotations;
    using System.ComponentModel.DataAnnotations.Schema;

    [System.Diagnostics.CodeAnalysis.SuppressMessage("Usage", "CA2227:Collection properties should be read only", Justification = "Used by Entity Framework")]
    public class Transcription : DbModelBase
    {
        public Transcription(Guid id, string locale, string name, string source, DateTime timestamp, string duration, double durationInSeconds, int numberOfChannels, float approximateCost)
        {
            this.Id = id;
            this.Locale = locale;
            this.Name = name;
            this.Source = source;
            this.Timestamp = timestamp;
            this.Duration = duration;
            this.DurationInSeconds = durationInSeconds;
            this.NumberOfChannels = numberOfChannels;
            this.ApproximateCost = approximateCost;
        }

        [Column("ID")]
        [Key]
        public Guid Id { get; set; }

        [StringLength(MaxLocaleLength)]
        public string Locale { get; private set; }

        [StringLength(MaxDefaultStringLength)]
        public string Name { get; private set; }

        [StringLength(MaxDefaultStringLength)]
        public string Source { get; private set; }

        public DateTime Timestamp { get; private set; }

        [StringLength(MaxTimeSpanColumnLength)]
        public string Duration { get; private set; }

        public double DurationInSeconds { get; private set; }

        public int NumberOfChannels { get; private set; }

        public float ApproximateCost { get; private set; }

        [ForeignKey("TranscriptionID")]
        public ICollection<CombinedRecognizedPhrase> CombinedRecognizedPhrases { get; set; }

        public Transcription WithCombinedRecognizedPhrases(ICollection<CombinedRecognizedPhrase> combinedRecognizedPhrases)
        {
            this.CombinedRecognizedPhrases = combinedRecognizedPhrases;
            return this;
        }
    }
}
