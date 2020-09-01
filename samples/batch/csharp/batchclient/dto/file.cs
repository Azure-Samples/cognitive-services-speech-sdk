//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;

    public class File
    {
        public Uri Self { get; set; }

        public string Name { get; set; }

        public ArtifactKind Kind { get; set; }

        public FileProperties Properties { get; set; }

        public DateTime CreatedDateTime { get; set; }

        public FileLinks Links { get; set; }
    }
}
