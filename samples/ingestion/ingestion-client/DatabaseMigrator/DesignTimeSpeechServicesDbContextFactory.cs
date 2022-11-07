// <copyright file="DesignTimeSpeechServicesDbContextFactory.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace DatabaseMigrator
{
    using Connector.Database;

    using Microsoft.EntityFrameworkCore;
    using Microsoft.EntityFrameworkCore.Design;

    public class DesignTimeSpeechServicesDbContextFactory : IDesignTimeDbContextFactory<IngestionClientDbContext>
    {
        public IngestionClientDbContext CreateDbContext(string[] args)
        {
            var optionsBuilder = new DbContextOptionsBuilder<IngestionClientDbContext>();
            optionsBuilder.UseSqlServer("Server=(localdb)\\mssqllocaldb;Database=DesignTimeDb;Trusted_Connection=True;MultipleActiveResultSets=true");
            var options = optionsBuilder.Options;
            options.Freeze();
            var context = new IngestionClientDbContext(optionsBuilder.Options);
            return context;
        }
    }
}