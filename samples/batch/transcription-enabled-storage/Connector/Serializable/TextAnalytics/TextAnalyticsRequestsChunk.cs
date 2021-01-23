// <copyright file="TextAnalyticsRequestsChunk.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable
{
    using System.Collections.Generic;

    public class TextAnalyticsRequestsChunk
    {
        public TextAnalyticsRequestsChunk(IEnumerable<TextAnalyticsRequest> documents)
        {
            Documents = documents;
        }

        public IEnumerable<TextAnalyticsRequest> Documents { get; private set; }
    }
}
