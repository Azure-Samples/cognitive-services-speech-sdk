// <copyright file="ConversationPiiCategory.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Enums
{
    public enum ConversationPiiCategory
    {
        All = 0,
        Name,
        PhoneNumber,
        Email,
        Address,
        NumericIdentifier,
        CreditCard
    }
}
