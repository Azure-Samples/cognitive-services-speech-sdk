//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Public.Interface;

public interface IAppLogger
{
    public void LogLine();

    public void LogLine(string message);

    public void Log(string message);
}
