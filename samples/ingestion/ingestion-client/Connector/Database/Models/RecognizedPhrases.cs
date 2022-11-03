// <copyright file="RecognizedPhrases.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database.Models
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.DataAnnotations;

    [System.Diagnostics.CodeAnalysis.SuppressMessage("Usage", "CA2227:Collection properties should be read only", Justification = "Used by Entity Framework")]
    public class RecognizedPhrases : DbModelBase
    {
        public RecognizedPhrases(Guid id, string recognitionStatus, int speaker, int channel, string offset, string duration, double silenceBetweenCurrentAndPreviousSegmentInMs, ICollection<NBests> nBests)
        {
            this.Id = id;
            this.RecognitionStatus = recognitionStatus;
            this.Speaker = speaker;
            this.Channel = channel;
            this.Offset = offset;
            this.Duration = duration;
            this.NBests = nBests;
            this.SilenceBetweenCurrentAndPreviousSegmentInMs = silenceBetweenCurrentAndPreviousSegmentInMs;
        }

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

        public double SilenceBetweenCurrentAndPreviousSegmentInMs { get; private set; }

        public ICollection<NBests> NBests { get; set; }
    }
}
