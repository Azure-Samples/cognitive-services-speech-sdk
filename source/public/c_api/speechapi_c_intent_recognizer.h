//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_intent_recognizer.h: Public API declarations for IntentRecognizer related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI IntentRecognizer_AddIntent(SPXRECOHANDLE hreco, const wchar_t* intentId, SPXTRIGGERHANDLE htrigger);
