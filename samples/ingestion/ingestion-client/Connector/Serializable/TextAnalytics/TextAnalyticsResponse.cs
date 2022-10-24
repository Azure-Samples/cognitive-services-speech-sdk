// <copyright file="TextAnalyticsResponse.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    using System.Collections.Generic;

    public class TextAnalyticsResponse
    {
        public TextAnalyticsResponse(IEnumerable<TextAnalyticsDocument> documents, IEnumerable<ErrorEntity> errors)
        {
            this.Documents = documents;
            this.Errors = errors;
        }

        public IEnumerable<TextAnalyticsDocument> Documents { get; private set; }

        public IEnumerable<ErrorEntity> Errors { get; private set; }
    }
}
