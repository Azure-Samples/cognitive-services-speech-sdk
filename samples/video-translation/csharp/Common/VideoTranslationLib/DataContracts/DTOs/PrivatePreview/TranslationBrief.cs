//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

using System.Collections.Generic;
using System.Globalization;

public class TranslationBrief : TranslationBase
{
    public IReadOnlyDictionary<CultureInfo, TranslationTargetLocaleBase> TargetLocales { get; set; }
}
