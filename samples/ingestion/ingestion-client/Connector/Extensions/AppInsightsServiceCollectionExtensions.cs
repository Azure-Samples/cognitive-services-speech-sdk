// <copyright file="AppInsightsServiceCollectionExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Linq;

    using Microsoft.Azure.Functions.Worker;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Logging;

    /// <summary>
    /// Extension methods for configuring Application Insights.
    /// </summary>
    public static class AppInsightsServiceCollectionExtensions
    {
        /// <summary>
        /// Configures the logger for the Ingestion Client and removes the default logging filter added by the Application Insights SDK.
        /// </summary>
        /// <param name="services">The Service Collection</param>
        /// <returns>The Service Collection</returns>
        public static IServiceCollection ConfigureIngestionClientLogging(this IServiceCollection services)
        {
            services.AddApplicationInsightsTelemetryWorkerService();
            services.ConfigureFunctionsApplicationInsights();

            services.Configure<LoggerFilterOptions>(options =>
            {
                // The Application Insights SDK adds a default logging filter that instructs ILogger to capture only Warning and more severe logs. Application Insights requires an explicit override.
                // Log levels can also be configured using appsettings.json. For more information, see https://learn.microsoft.com/azure/azure-monitor/app/worker-service#ilogger-logs
                LoggerFilterRule toRemove = options.Rules.FirstOrDefault(rule => rule.ProviderName
                    == "Microsoft.Extensions.Logging.ApplicationInsights.ApplicationInsightsLoggerProvider");
                if (toRemove is not null)
                {
                    options.Rules.Remove(toRemove);
                }
            });

            return services;
        }
    }
}