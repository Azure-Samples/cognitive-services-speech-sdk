//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System.Collections.Generic;

    public class NBest
    {
        public float Confidence { get; set; }

        public string Lexical { get; set; }

        public string ITN { get; set; }

        public string MaskedITN { get; set; }

        public string Display { get; set; }

        public IEnumerable<WordDetails> Words { get; set; }

        public IEnumerable<DisplayWord> DisplayWords { get; set; }
    }
}
