// <copyright file="ModelIdentity.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public sealed class ModelIdentity
    {
        public ModelIdentity(string self)
        {
            this.Self = self;
        }

        public string Self { get; private set; }
    }
}
