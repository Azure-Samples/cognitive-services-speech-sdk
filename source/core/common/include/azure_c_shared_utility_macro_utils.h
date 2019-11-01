//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// Fixes "warnings as errors" issues with azure_c_shared_utility library include files
// 

#pragma once

#ifndef _MSC_VER
#pragma GCC system_header
#endif

#include "azure_c_shared_utility/macro_utils.h"
