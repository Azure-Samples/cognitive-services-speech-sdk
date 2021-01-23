//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Collections.Generic;

    public sealed class Transcription : LocalizedDtoBase
    {
        public IEnumerable<Uri> ContentUrls { get; set; }

        public Uri ContentContainerUrl { get; set; }

        public EntityReference Model { get; set; }

        public TranscriptionProperties Properties { get; set; }

        public Links Links { get; set; }
    }
}