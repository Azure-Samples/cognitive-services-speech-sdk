// <copyright file="IngestionClientDbContext.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database
{
    using Connector.Database.Models;
    using Microsoft.EntityFrameworkCore;

    public class IngestionClientDbContext : DbContext
    {
        public virtual DbSet<Transcriptions> Transcriptions { get; set; }

        public virtual DbSet<CombinedRecognizedPhrases> CombinedRecognizedPhrases { get; set; }

        public virtual DbSet<NBests> NBests { get; set; }

        public virtual DbSet<RecognizedPhrases> RecognizedPhrases { get; set; }

        public virtual DbSet<Words> Words { get; set; }
    }
}
