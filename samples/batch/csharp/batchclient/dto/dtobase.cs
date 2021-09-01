//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Collections.Generic;

    public abstract class DtoBase
    {
        public Uri Self { get; set; }

        public string DisplayName { get; set; }

        public string Description { get; set; }

        public DateTime CreatedDateTime { get; set; }

        public DateTime LastActionDateTime { get; set; }

        public string Status { get; set; }

        public IReadOnlyDictionary<string, string> CustomProperties { get; private set; }
    }
}
