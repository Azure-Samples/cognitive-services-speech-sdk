//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Extensions;

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

public static class StringExtensions
{
    public static string MaskSasToken(this string str)
    {
        if (string.IsNullOrWhiteSpace(str))
        {
            return str;
        }

        var sasRegexPattern = "(?<signature>sig=[\\w%]+)";
        var matches = Regex.Matches(str, sasRegexPattern);
        foreach (Match match in matches)
        {
            str = str.Replace(match.Groups["signature"].Value, "SIGMASKED");
        }

        return str;
    }

    public static IReadOnlyDictionary<string, string> ToDictionaryWithDelimeter(this string value)
    {
        var headers = new Dictionary<string, string>();
        if (!string.IsNullOrEmpty(value))
        {
            var headerPairs = value.Split(new char[] { ',', ';' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (var headerPair in headerPairs.Where(x => !string.IsNullOrEmpty(x)))
            {
                var delimeterIndex = headerPair.IndexOf('=');
                if (delimeterIndex < 0)
                {
                    throw new InvalidDataException($"Invalid argument format: {value}");
                }

                headers[headerPair.Substring(0, delimeterIndex)] = headerPair.Substring(delimeterIndex + 1);
            }
        }

        return headers;
    }
}
