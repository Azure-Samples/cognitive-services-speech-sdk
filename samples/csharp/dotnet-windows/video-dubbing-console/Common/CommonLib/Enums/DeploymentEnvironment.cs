//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Enums;

using Microsoft.SpeechServices.CommonLib.Attributes;
using System;
using System.Runtime.Serialization;

[DataContract]
public enum DeploymentEnvironment
{
    [EnumMember]
    None = 0,

    [EnumMember]
    [DeploymentEnvironment(
        regionIdentifier: "eastus")]
    ProductionEUS,
}