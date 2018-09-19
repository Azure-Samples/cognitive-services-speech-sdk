//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_intent_recognizer.h: Public API declarations for IntentRecognizer related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI intent_recognizer_add_intent(SPXRECOHANDLE hreco, const char* intentId, SPXTRIGGERHANDLE htrigger);
