// <copyright file="TranscriptionFiles.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionFiles
{
    using System.Collections.Generic;
    using Newtonsoft.Json;

    public class TranscriptionFiles
    {
        public TranscriptionFiles(IEnumerable<TranscriptionFile> values, string nextLink)
        {
            this.Values = values;
            this.NextLink = nextLink;
        }

        public IEnumerable<TranscriptionFile> Values { get; set; }

        [JsonProperty("@nextLink")]
        public string NextLink { get; set; }
    }
}
