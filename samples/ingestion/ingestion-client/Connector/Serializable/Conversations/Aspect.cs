// <copyright file="Aspect.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Text.Json.Serialization;

    [JsonConverter(typeof(JsonStringEnumConverter))]
    public enum Aspect
    {
        None = 0,
        Issue = 1,
        Resolution = 2,
        ChapterTitle = 3,
        Narrative = 4,
    }
}
