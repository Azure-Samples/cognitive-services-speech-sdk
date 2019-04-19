//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_c_activity.h: Public API declarations for bot connector activities C methods and types.
//
#pragma once
#include <speechapi_c_common.h>

SPXAPI bot_activity_create(SPXACTIVITYHANDLE* ph_act);
SPXAPI bot_activity_from_string(const char* serialized_activity, SPXACTIVITYHANDLE* ph_act);

SPXAPI_(bool) bot_activity_handle_is_valid(SPXACTIVITYHANDLE h_act);
SPXAPI bot_activity_handle_release(SPXACTIVITYHANDLE h_act);


SPXAPI bot_activity_serialized_size(SPXACTIVITYHANDLE h_act, size_t* size);
SPXAPI bot_activity_serialize(SPXACTIVITYHANDLE h_act, char* buffer, size_t max_size);

/* Properties */
SPXAPI bot_activity_property_size(SPXACTIVITYHANDLE h_act, const char* prop_name, size_t* size);
SPXAPI bot_activity_property_get(SPXACTIVITYHANDLE h_act, const char* prop_name, char* buffer, size_t max_size);
SPXAPI bot_activity_property_set(SPXACTIVITYHANDLE h_act, const char* prop_name, const char* buffer);

/* Complex Fields (JSON objects, JSON arrays) */
SPXAPI bot_activity_complex_field_handle(SPXACTIVITYHANDLE h_act, const char* prop_name, SPXACTIVITYJSONHANDLE* ph_json);

SPXAPI_(bool) bot_activity_json_handle_is_valid(SPXACTIVITYJSONHANDLE h_json);
SPXAPI bot_activity_json_handle_release(SPXACTIVITYJSONHANDLE h_json);


#ifndef __cplusplus
enum BotActivityJSONType
{
    BotActivityJSONType_Null          = 0,
    BotActivityJSONType_Object        = 1,
    BotActivityJSONType_Array         = 2,
    BotActivityJSONType_Value_String  = 3,
    BotActivityJSONType_Value_Double  = 4,
    BotActivityJSONType_Value_UInt    = 5,
    BotActivityJSONType_Value_Int     = 6,
    BotActivityJSONType_Value_Boolean = 7
};
#endif // !__cplusplus

SPXAPI bot_activity_json_get_type(SPXACTIVITYHANDLE h_json, int* type);

/* JSON Object */
SPXAPI bot_activity_json_has_field(SPXACTIVITYJSONHANDLE, const char* field, bool* result);
SPXAPI bot_activity_json_field_handle(SPXACTIVITYJSONHANDLE h_json, const char* field_name, SPXACTIVITYJSONHANDLE* ph_field);

/* JSON Value */
SPXAPI bot_activity_json_set_int(SPXACTIVITYJSONHANDLE h_json, int64_t val);
SPXAPI bot_activity_json_set_uint(SPXACTIVITYJSONHANDLE h_json, uint64_t val);
SPXAPI bot_activity_json_set_double(SPXACTIVITYJSONHANDLE h_json, double val);
SPXAPI bot_activity_json_set_bool(SPXACTIVITYJSONHANDLE h_json, bool val);
SPXAPI bot_activity_json_set_string(SPXACTIVITYJSONHANDLE h_json, const char* val);

SPXAPI bot_activity_json_get_int(SPXACTIVITYJSONHANDLE h_json, int64_t* dbl);
SPXAPI bot_activity_json_get_uint(SPXACTIVITYJSONHANDLE h_json, uint64_t* dbl);
SPXAPI bot_activity_json_get_double(SPXACTIVITYJSONHANDLE h_json, double* dbl);
SPXAPI bot_activity_json_get_bool(SPXACTIVITYJSONHANDLE h_json, bool* b);
SPXAPI bot_activity_json_get_string_size(SPXACTIVITYJSONHANDLE h_json, size_t* size);
SPXAPI bot_activity_json_get_string(SPXACTIVITYJSONHANDLE h_json, char* buffer, size_t max_size);

/* JSON Array */
SPXAPI bot_activity_json_array_size(SPXACTIVITYJSONHANDLE h_json, size_t* size);
SPXAPI bot_activity_json_array_item(SPXACTIVITYJSONHANDLE h_json, size_t index, SPXACTIVITYJSONHANDLE* ph_item);
