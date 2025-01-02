//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Public.Interface;

using System;

public interface IRegionConfig
{
    public string RegionIdentifier { get; }

    public Uri EndpointUrl { get; }
}
