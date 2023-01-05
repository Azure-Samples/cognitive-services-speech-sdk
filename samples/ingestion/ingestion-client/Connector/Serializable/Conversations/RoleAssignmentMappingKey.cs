// <copyright file="RoleAssignmentMappingKey.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Text.Json.Serialization;

    [JsonConverter(typeof(JsonStringEnumConverter))]
    public enum RoleAssignmentMappingKey
    {
        None = 0,
        Channel = 1,
        Speaker = 2,
    }
}
