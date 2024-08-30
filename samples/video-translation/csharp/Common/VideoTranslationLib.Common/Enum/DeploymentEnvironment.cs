//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationLib.Enums;

using Microsoft.SpeechServices.CommonLib.Attributes;
using System;
using System.Runtime.Serialization;

[DataContract]
public enum DeploymentEnvironment
{
    [EnumMember]
    Default,

    [EnumMember]
    [DeploymentEnvironment(
        regionIdentifier: "eastus")]
    ProductionEUS,
}