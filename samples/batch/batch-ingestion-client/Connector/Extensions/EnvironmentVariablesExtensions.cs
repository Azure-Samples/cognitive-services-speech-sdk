// <copyright file="EnvironmentVariablesExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public static class EnvironmentVariablesExtensions
    {
        public static int ClampInt(this int value, int min, int max)
        {
            return Math.Max(min, Math.Min(value, max));
        }
    }
}
