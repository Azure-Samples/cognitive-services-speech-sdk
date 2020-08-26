// <copyright file="Data.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;

    public class Data
    {
        public string ApiVersion { get; set; }

        public string ClientRequestId { get; set; }

        public string RequestId { get; set; }

        public string ETag { get; set; }

        public string ContentType { get; set; }

        public int ContentLength { get; set; }

        public string BlobType { get; set; }

        public Uri Url { get; set; }

        public string Sequencer { get; set; }

        public StorageDiagnostics StorageDiagnostics { get; set; }
    }
}
