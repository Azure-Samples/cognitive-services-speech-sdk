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
        public IngestionClientDbContext(DbContextOptions<IngestionClientDbContext> options)
            : base(options)
        {
        }

        public DbSet<Transcription> Transcriptions { get; set; }

        public DbSet<CombinedRecognizedPhrase> CombinedRecognizedPhrases { get; set; }

        public DbSet<NBest> NBests { get; set; }

        public DbSet<RecognizedPhrase> RecognizedPhrases { get; set; }

        public DbSet<Word> Words { get; set; }
    }
}
