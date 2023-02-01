// <copyright file="RoleAssignmentStratergy.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;

    public class RoleAssignmentStratergy
    {
        public RoleAssignmentMappingKey Key { get; init; }

        public Dictionary<int, Role> Mapping { get; init; }

        public Role FallbackRole { get; init; }
    }
}
