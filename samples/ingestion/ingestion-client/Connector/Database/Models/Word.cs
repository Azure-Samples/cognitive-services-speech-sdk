// <copyright file="Word.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database.Models
{
    using System;
    using System.ComponentModel.DataAnnotations;
    using System.ComponentModel.DataAnnotations.Schema;

    public class Word : DbModelBase
    {
        public Word(Guid id, string wordText, string offset, string duration, double confidence)
        {
            this.Id = id;
            this.WordText = wordText;
            this.Offset = offset;
            this.Duration = duration;
            this.Confidence = confidence;
        }

        [Column("ID")]
        [Key]
        public Guid Id { get; set; }

        [Column("Word")]
        [StringLength(MaxWordLength)]
        public string WordText { get; private set; }

        [StringLength(MaxTimeSpanColumnLength)]
        public string Offset { get; private set; }

        [StringLength(MaxTimeSpanColumnLength)]
        public string Duration { get; private set; }

        public double Confidence { get; private set; }
    }
}
