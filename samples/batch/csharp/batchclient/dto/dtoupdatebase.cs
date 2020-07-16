//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System.Collections.Generic;

    public abstract class DtoUpdateBase
    {
        public string DisplayName { get; set; }

        public string Description { get; set; }

        public IReadOnlyDictionary<string, string> CustomProperties { get; set; }
    }
}
