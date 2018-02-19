//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// event_helpers.h: Private implementation declarations for EventSignal related C methods
//

#pragma once
#include <speechapi_c_common.h>


namespace CARBON_IMPL_NAMESPACE() {
    

SPXAPI_PRIVATE Recognizer_SessionEvent_SetCallback(ISpxRecognizerEvents::SessionEvent_Type ISpxRecognizerEvents::*psessionEvent, SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI_PRIVATE Recognizer_RecoEvent_SetCallback(ISpxRecognizerEvents::RecoEvent_Type ISpxRecognizerEvents::*precoEvent, SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);


} // CARBON_IMPL_NAMESPACE
