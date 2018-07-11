//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// guid_utils.h: Utility classes/functions dealing with GUIDs
//

#pragma once

namespace PAL
{
// Creates a GUID without dashes.
std::wstring CreateGuidWithoutDashes();

std::string DeviceUuid();

} // PAL
