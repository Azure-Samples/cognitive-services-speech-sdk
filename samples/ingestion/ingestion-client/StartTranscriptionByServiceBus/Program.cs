// <copyright file="Program.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace StartTranscription
{
    using System.Threading.Tasks;
    using Connector;
    using Microsoft.Extensions.Hosting;

    /// <summary>
    /// Represents the entry point of the application.
    /// </summary>
    public static class Program
    {
        /// <summary>
        /// Main entry point of the function app.
        /// </summary>
        /// <param name="args"></param>
        /// <returns>A <see cref="Task"/> representing the result of the asynchronous operation.</returns>
        public static async Task Main(string[] args)
        {
            var host = new HostBuilder()
                .ConfigureFunctionsWorkerDefaults()
                .ConfigureServices(s =>
                {
                    // This is a unified way to configure logging filter for all functions.
                    s.ConfigureIngestionClientLogging();
                })
                .Build();

            await host.RunAsync();
        }
    }
}