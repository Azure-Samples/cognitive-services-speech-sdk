//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using Newtonsoft.Json;
    using System;
    using System.Collections.Generic;

    public class PaginatedFiles
    {
        public IEnumerable<File> Values { get; private set; }

        [JsonProperty(PropertyName = "@nextLink")]
        public Uri NextLink { get; set; }
    }
}
