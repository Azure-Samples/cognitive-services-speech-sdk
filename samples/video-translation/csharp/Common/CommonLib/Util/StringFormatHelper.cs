//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CustomVoice.TtsLib.Util;

using System;
using System.Globalization;

public static class StringFormatHelper
{
    public static string ToIdSuffix(this DateTime timestamp)
    {
        return timestamp.ToString(@"yyMMddhhmmss", CultureInfo.InvariantCulture);
    }
}
