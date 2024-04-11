//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.DataContracts.Deprecated;

using System;
using Microsoft.SpeechServices.CommonLib.Enums;

public abstract class StatefulResourceBase : StatelessResourceBase
{
    public OneApiState Status { get; set; }

    public DateTime LastActionDateTime { get; set; }
}
