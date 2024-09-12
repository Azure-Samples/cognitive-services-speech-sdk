//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CustomVoice.TtsLib.Util;

using Microsoft.SpeechServices.CommonLib.Extensions;
using System;
using System.Diagnostics;
using System.IO;

public static class ConsoleHelper
{
    public static void WriteLineErrorMaskSas(string message)
    {
        Console.Error.WriteLine(message.MaskSasToken());
    }

    public static void WriteLineErrorMaskSas()
    {
        Console.Error.WriteLine();
    }

    public static void WriteMaskSas(string message)
    {
        Console.Write(message.MaskSasToken());
    }

    public static void WriteLineMaskSas()
    {
        Console.WriteLine();
    }

    public static void WriteLineMaskSas(string message)
    {
        Console.WriteLine(message.MaskSasToken());
    }

    public static string TempRoot
    {
        get
        {
            return Path.Combine(Path.GetPathRoot(Process.GetCurrentProcess().MainModule.FileName), "Temp");
        }
    }

    public static string GetTempDir()
    {
        return Path.Combine(TempRoot, Guid.NewGuid().ToString());
    }
}
