//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_session.h: Public API declarations for Session related C methods
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI Session_From_Recognizer(SPXRECOHANDLE hreco, SPXSESSIONHANDLE* phsession);

SPXAPI_(bool) Session_Handle_IsValid(SPXSESSIONHANDLE hsession);
SPXAPI Session_Handle_Close(SPXSESSIONHANDLE hsession);

SPXAPI session_get_property_bag(SPXSESSIONHANDLE hsession, SPXPROPERTYBAGHANDLE* hpropbag);
