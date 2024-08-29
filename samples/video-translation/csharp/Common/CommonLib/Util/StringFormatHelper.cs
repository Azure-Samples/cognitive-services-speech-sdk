// <copyright file="StringFormatHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.CustomVoice.TtsLib.Util;

using System;
using System.Globalization;
using System.Text.RegularExpressions;

public static class StringFormatHelper
{
    public static string ToIdSuffix(this DateTime timestamp)
    {
        return timestamp.ToString(@"yyMMddhhmmss", CultureInfo.InvariantCulture);
    }
}
