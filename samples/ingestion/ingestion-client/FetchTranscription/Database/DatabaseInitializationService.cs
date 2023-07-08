// <copyright file="DatabaseInitializationService.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

using FetchTranscription.Database;

using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Hosting;

[assembly: WebJobsStartup(typeof(DatabaseInitializationService), "DatabaseInitialize")]

namespace FetchTranscription.Database
{
    public class DatabaseInitializationService : IWebJobsStartup
    {
        public void Configure(IWebJobsBuilder builder)
        {
            builder.AddExtension<DatabaseConfigProvider>();
        }
    }
}
