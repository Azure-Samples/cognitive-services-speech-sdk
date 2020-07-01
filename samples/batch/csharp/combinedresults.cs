//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    public class CombinedResults
    {
        public int Channel { get; set; }

        public string Lexical { get; set; }

        public string ITN { get; set; }

        public string MaskedITN { get; set; }

        public string Display { get; set; }
    }
}
