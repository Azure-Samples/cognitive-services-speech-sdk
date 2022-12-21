// <copyright file="ErrorEntity.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    public class ErrorEntity
    {
        public ErrorEntity(string id, TextAnalyticsError error)
        {
            this.Id = id;
            this.Error = error;
        }

        public string Id { get; private set; }

        public TextAnalyticsError Error { get; private set; }
    }
}
