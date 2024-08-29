//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.CommandParser;

public sealed class ExitCode
{
    public const int NoError = 0;

    public const int InvalidArgument = -1;

    public const int GenericError = 999;

    private ExitCode()
    {
    }
}