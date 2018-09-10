//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_property_bag.h: Public API declarations for Property Bag related C methods
//

#pragma once
#include <speechapi_c_common.h>

SPXAPI_(bool) property_bag_is_valid(SPXPROPERTYBAGHANDLE hpropbag);

SPXAPI property_bag_set_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* value);
SPXAPI__(const char*) property_bag_get_string(SPXPROPERTYBAGHANDLE hpropbag, int id, const char* name, const char* defaultValue);
SPXAPI property_bag_free_string(const char* value);

SPXAPI property_bag_close(SPXPROPERTYBAGHANDLE hpropbag);
