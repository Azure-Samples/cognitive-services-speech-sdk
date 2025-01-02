//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Public.Enums;

using System;
using System.Runtime.Serialization;

[DataContract]
public enum OneApiState
{
    [Obsolete("Do not use directly - used to discover deserializer issues.")]
    None = 0,

    NotStarted,

    Running,

    Succeeded,

    Failed,
}