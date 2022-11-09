// <copyright file="RecognizedPhrase.cs" company="Microsoft Corporation">
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
    public class RecognizedPhrase : DbModelBase
    {
        public RecognizedPhrase(Guid id, string recognitionStatus, int speaker, int channel, string offset, string duration, int silenceBetweenCurrentAndPreviousSegmentInMs)
        {
            this.Id = id;
            this.RecognitionStatus = recognitionStatus;
            this.Speaker = speaker;
            this.Channel = channel;
            this.Offset = offset;
            this.Duration = duration;
            this.SilenceBetweenCurrentAndPreviousSegmentInMs = silenceBetweenCurrentAndPreviousSegmentInMs;
        }

        [Column("ID")]
        [Key]
        public Guid Id { get; set; }

        [StringLength(MaxStateLength)]
        public string RecognitionStatus { get; private set; }

        public int Speaker { get; private set; }

        public int Channel { get; private set; }

        [StringLength(MaxTimeSpanColumnLength)]
        public string Offset { get; private set; }

        [StringLength(MaxTimeSpanColumnLength)]
        public string Duration { get; private set; }

        public int SilenceBetweenCurrentAndPreviousSegmentInMs { get; private set; }

        [ForeignKey("RecognizedPhraseID")]
        public ICollection<NBest> NBests { get; set; }

        public RecognizedPhrase WithNBests(ICollection<NBest> nbests)
        {
            this.NBests = nbests;
            return this;
        }
    }
}
