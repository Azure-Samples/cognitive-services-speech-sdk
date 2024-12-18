//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.TtsUtil;

using System;
using Microsoft.SpeechServices.CommonLib.Public.Interface;

public class PublicAppLogger : IAppLogger
{
    void IAppLogger.Log(string message)
    {
        Console.Write(message);
    }

    void IAppLogger.LogLine(string message)
    {
        Console.WriteLine(message);
    }

    void IAppLogger.LogLine()
    {
        Console.WriteLine();
    }
}
