// <copyright file="DbModelBase.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database.Models
{
    public abstract class DbModelBase
    {
        public const int MaxTimeSpanColumnLength = 255;

        public const int MaxLocaleLength = 255;

        public const int MaxDefaultStringLength = 500;

        public const int MaxWordLength = 511;

        public const int MaxStateLength = 32;
    }
}
