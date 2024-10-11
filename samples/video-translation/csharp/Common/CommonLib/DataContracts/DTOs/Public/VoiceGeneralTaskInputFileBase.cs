//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VoiceGeneralTask;

using System;

public class VoiceGeneralTaskInputFileBase : StatefulResourceBase
{
    public string FileContentSha256 { get; set; }

    public Uri Url { get; set; }

    public long? Version { get; set; }
}
