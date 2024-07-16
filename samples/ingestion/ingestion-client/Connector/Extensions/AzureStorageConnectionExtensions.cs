// <copyright file="AzureStorageConnectionExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public static class AzureStorageConnectionExtensions
    {
        public static string GetValueFromConnectionString(string key, string connectionString)
            {
                ArgumentNullException.ThrowIfNull(connectionString, nameof(connectionString));
                ArgumentNullException.ThrowIfNull(key, nameof(key));
                var split = connectionString.Split(';');

                foreach (var subConnectionString in split)
                {
                    if (subConnectionString.StartsWith(key, StringComparison.OrdinalIgnoreCase))
                    {
                        return subConnectionString.Substring(key.Length + 1);
                    }
                }

                return string.Empty;
            }
    }
}