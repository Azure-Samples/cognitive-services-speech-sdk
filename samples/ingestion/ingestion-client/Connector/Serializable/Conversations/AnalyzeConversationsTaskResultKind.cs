// <copyright file="AnalyzeConversationsTaskResultKind.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.Language.Conversations
{
    public enum AnalyzeConversationsTaskResultKind
    {
#pragma warning disable SA1300 // Element should begin with upper-case letter
        conversationalPIIResults,
        conversationalSummarizationResults,
#pragma warning restore SA1300 // Element should begin with upper-case letter
    }
}