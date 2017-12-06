//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// event_helpers.h: Private implementation declarations for EventSignal related C methods
//

#pragma once
#include <speechapi_c_common.h>


namespace CARBON_IMPL_NAMESPACE() {
    

SPXAPI_PRIVATE Recognizer_Event_SetCallback(ISpxRecognizerEvents::RecoEvent_Type ISpxRecognizerEvents::*precoEvent, SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);


}; // CARBON_IMPL_NAMESPACE()
