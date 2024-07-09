// <copyright file="Program.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace FetchTranscription
{
    using System.Threading.Tasks;

    using Connector;
    using Connector.Database;

    using Microsoft.EntityFrameworkCore;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Hosting;

    public static class Program
    {
        public static async Task Main(string[] args)
        {
            var host = new HostBuilder()
                .ConfigureFunctionsWorkerDefaults()
                .ConfigureServices(s =>
                {
                    // This is a unified way to configure logging filter for all functions.
                    s.ConfigureIngestionClientLogging();

                    if (FetchTranscriptionEnvironmentVariables.UseSqlDatabase)
                    {
                        s.AddDbContext<IngestionClientDbContext>(
                        options => SqlServerDbContextOptionsExtensions.UseSqlServer(options, FetchTranscriptionEnvironmentVariables.DatabaseConnectionString));
                    }
                })
                .Build();

            await host.RunAsync();
        }
    }
}