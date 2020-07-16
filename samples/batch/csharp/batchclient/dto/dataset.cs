//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;

    public class Dataset : ScopedLocalizedDtoBase
    {
        public DatasetKind Kind { get; set; }

        public Uri ContentUrl { get; set; }

        public Links Links { get; set; }
    }
}
