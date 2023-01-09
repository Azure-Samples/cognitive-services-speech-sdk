// <copyright file="ConversationSummarizationOptions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    using System.Collections.Generic;

    public class ConversationSummarizationOptions
    {
        public bool Enabled { get; init; }

        public int InputLengthLimit { get; init; }

        public RoleAssignmentStratergy Stratergy { get; init; }

        public IEnumerable<Aspect> Aspects { get; init; }
    }
}
