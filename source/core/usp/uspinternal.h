//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// uspinternal.h: the header file only used by usplib internally
//

#pragma once

#include <stdint.h>

// Todo: need refactor
#include "private-iot-cortana-sdk.h"

/**
 * The UspContext represents the context data related to a USP client.
 */
typedef struct _UspContext
{
    SPEECH_CONTEXT* speechContext;

} UspContext;

