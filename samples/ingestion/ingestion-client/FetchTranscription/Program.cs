// <copyright file="Program.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using Connector;
    using Connector.Database;

    using Microsoft.EntityFrameworkCore;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Hosting;

    public static class Program
    {
        public static void Main(string[] args)
        {
            var useSqlDatabase = FetchTranscriptionEnvironmentVariables.UseSqlDatabase;

            var host = new HostBuilder()
                .ConfigureFunctionsWorkerDefaults()
                .ConfigureServices(s =>
                {
                    // This is a unified way to configure logging filter for all functions.
                    s.ConfigureIngestionClientLogging();

                    if (useSqlDatabase)
                    {
                        s.AddDbContext<IngestionClientDbContext>(
                        options => SqlServerDbContextOptionsExtensions.UseSqlServer(options, FetchTranscriptionEnvironmentVariables.DatabaseConnectionString));
                    }
                })
                .Build();

            // apply database migrations once during startup (not with every function execution):
            if (useSqlDatabase)
            {
                using var scope = host.Services.CreateScope();
                var ingestionClientDbContext = scope.ServiceProvider.GetRequiredService<IngestionClientDbContext>();
                ingestionClientDbContext.Database.Migrate();
            }

            host.Run();
        }
    }
}