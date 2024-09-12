//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CustomVoice.TtsLib.Util;

using System;
using System.Linq;

public static class UriHelper
{
    public static string GetFileName(Uri url)
    {
        if (string.IsNullOrWhiteSpace(url?.OriginalString))
        {
            throw new ArgumentNullException(nameof(url));
        }

        var videoFileName = url.Segments.Last();
        videoFileName = Uri.UnescapeDataString(videoFileName);
        return videoFileName;
    }

    public static string AppendOptionalQuery(this string query, string name, string value)
    {
        if (string.IsNullOrEmpty(value))
        {
            return query;
        }

        var item = $"{name}={Uri.EscapeDataString(value)}";
        return string.IsNullOrEmpty(query) ? item : $"{query}&{item}";
    }

    // query include ?
    public static Uri SetQuery(Uri uri, string query)
    {
        if (uri == null)
        {
            throw new ArgumentNullException(nameof(uri));
        }

        var uriString = uri.ToString();
        var index = uriString.IndexOf("?");
        if (index < 0)
        {
            return uri;
        }

        return new Uri($"{uriString.Substring(0, index)}{query}");
    }
}
