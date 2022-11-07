// <copyright file="NBest.cs" company="Microsoft Corporation">
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
    public class NBest : DbModelBase
    {
        public NBest(Guid id, double confidence, string lexical, string itn, string maskedItn, string display, double sentimentNegative, double sentimentNeutral, double sentimentPositive)
        {
            this.Id = id;
            this.Confidence = confidence;
            this.Lexical = lexical;
            this.Itn = itn;
            this.MaskedItn = maskedItn;
            this.Display = display;
            this.SentimentNegative = sentimentNegative;
            this.SentimentNeutral = sentimentNeutral;
            this.SentimentPositive = sentimentPositive;
        }

        [Column("ID")]
        [Key]
        public Guid Id { get; set; }

        public double Confidence { get; private set; }

        public string Lexical { get; private set; }

        public string Itn { get; private set; }

        public string MaskedItn { get; private set; }

        public string Display { get; private set; }

        public double SentimentNegative { get; private set; }

        public double SentimentNeutral { get; private set; }

        public double SentimentPositive { get; private set; }

        [ForeignKey("NBestID")]
        public ICollection<Word> Words { get; set; }

        public NBest WithWords(ICollection<Word> words)
        {
            this.Words = words;
            return this;
        }
    }
}
