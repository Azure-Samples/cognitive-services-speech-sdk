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

        public DbSet<Transcriptions> Transcriptions { get; set; }

        public DbSet<CombinedRecognizedPhrases> CombinedRecognizedPhrases { get; set; }

        public DbSet<NBests> NBests { get; set; }

        public DbSet<RecognizedPhrases> RecognizedPhrases { get; set; }

        public DbSet<Words> Words { get; set; }
    }
}
