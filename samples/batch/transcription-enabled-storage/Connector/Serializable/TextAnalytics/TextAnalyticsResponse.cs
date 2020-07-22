// <copyright file="TextAnalyticsResponse.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TextAnalytics
{
    using System.Collections.Generic;

    public class TextAnalyticsResponse
    {
        public TextAnalyticsResponse(List<TextAnalyticsDocument> documents, List<ErrorEntity> errors)
        {
            Documents = documents;
            Errors = errors;
        }

        public List<TextAnalyticsDocument> Documents { get; private set; }

        public List<ErrorEntity> Errors { get; private set; }
    }
}
