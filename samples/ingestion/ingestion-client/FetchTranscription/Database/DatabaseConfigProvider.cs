// <copyright file="DatabaseConfigProvider.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

using FetchTranscription.Database;

using Microsoft.Azure.WebJobs.Hosting;

[assembly: WebJobsStartup(typeof(DatabaseInitializationService), "DatabaseInitialize")]

namespace FetchTranscription.Database
{
    using Connector.Database;

    using FetchTranscriptionFunction;

    using Microsoft.Azure.WebJobs.Description;
    using Microsoft.Azure.WebJobs.Host.Config;
    using Microsoft.EntityFrameworkCore;
    using Microsoft.Extensions.DependencyInjection;

    [Extension("DatabaseConfig")]
    public class DatabaseConfigProvider : IExtensionConfigProvider
    {
        private readonly IServiceScopeFactory scopeFactory;

        public DatabaseConfigProvider(IServiceScopeFactory scopeFactory)
        {
            this.scopeFactory = scopeFactory;
        }

        public void Initialize(ExtensionConfigContext context)
        {
            if (FetchTranscriptionEnvironmentVariables.UseSqlDatabase)
            {
                using var scope = this.scopeFactory.CreateScope();
                var databaseContext = scope.ServiceProvider.GetService<IngestionClientDbContext>();
                databaseContext.Database.Migrate();
            }
        }
    }
}
