//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System.Collections.Generic;
    using System.Globalization;

    public class LanguageIdentificationProperties
    {
        public IEnumerable<CultureInfo> CandidateLocales { get; set; }

        public IReadOnlyDictionary<CultureInfo, EntityReference> SpeechModelMapping { get; set; }
    }
}
