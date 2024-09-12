//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using Microsoft.SpeechServices.CommonLib.Extensions;
using System;

public static class ConsoleMaskSasHelper
{
    public static bool ShowSas { get; set; }

    static ConsoleMaskSasHelper()
    {
        ShowSas = false;
    }

    public static void WriteLineMaskSas(string message)
    {
        Console.WriteLine(ShowSas ? message : message.MaskSasToken());
    }
}
