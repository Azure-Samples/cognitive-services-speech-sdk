//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_activity.cpp: Public API definitions for bot connector activities.

#include "stdafx.h"
#include "handle_helpers.h"
#include "create_object_helpers.h"
#include "site_helpers.h"

#include "speechapi_cxx_enums.h"

#include <mutex>

using namespace Microsoft::CognitiveServices::Speech::Impl;

/* Since the activity model relies on references, we need to protect the user from himself */
std::mutex g_activity_mutex;

SPXAPI bot_activity_create(SPXACTIVITYHANDLE* ph_act)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_act == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_act = SPXHANDLE_INVALID;
        auto activity = SpxCreateObjectWithSite<ISpxActivity>("CSpxActivity", SpxGetRootSite());
        auto activityTable = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        *ph_act = activityTable->TrackHandle(activity);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_activity_from_string(const char* serialized_activity, SPXACTIVITYHANDLE* ph_act)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_act == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        *ph_act = SPXHANDLE_INVALID;
        auto json = nlohmann::json::parse(serialized_activity);
        auto activity = SpxCreateObjectWithSite<ISpxActivity>("CSpxActivity", SpxGetRootSite());
        activity->LoadJSON(json);
        auto activityTable = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        *ph_act = activityTable->TrackHandle(activity);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) bot_activity_handle_is_valid(SPXACTIVITYHANDLE h_act)
{
    return Handle_IsValid<SPXACTIVITYHANDLE, ISpxActivity>(h_act);
}

SPXAPI bot_activity_handle_release(SPXACTIVITYHANDLE h_act)
{
    return Handle_Close<SPXACTIVITYHANDLE, ISpxActivity>(h_act);
}

SPXAPI bot_activity_serialized_size(SPXACTIVITYHANDLE h_act, size_t* size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, size == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        auto act = (*handles)[h_act];
        auto& json = act->GetJSON();
        *size = json.dump().size();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_activity_serialize(SPXACTIVITYHANDLE h_act, char* buffer, size_t max_size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, buffer == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        auto act = (*handles)[h_act];
        auto& json = act->GetJSON();
        auto serialized = json.dump();
        if (max_size < serialized.size())
        {
            return SPXERR_BUFFER_TOO_SMALL;
        }
        PAL::strcpy(buffer, max_size, serialized.c_str(), serialized.size(), false);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_activity_property_size(SPXACTIVITYHANDLE h_act, const char* prop_name, size_t* size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, prop_name == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, size == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        auto act = (*handles)[h_act];
        auto& json = act->GetJSON();
        auto& prop = json[prop_name];
        if (!prop.is_null() && !prop.is_string())
        {
            /* This in an invariant */
            return SPXERR_INVALID_STATE;
        }
        *size = prop.is_null() ? 0 : prop.get_ref<const std::string&>().size();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_activity_property_get(SPXACTIVITYHANDLE h_act, const char* prop_name, char* buffer, size_t max_size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, prop_name == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, buffer == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        auto act = (*handles)[h_act];
        auto& json = act->GetJSON();
        auto& prop = json[prop_name];
        auto& value = prop.get_ref<const std::string&>();
        std::size_t size = value.size();
        if (max_size < size)
        {
            return SPXERR_BUFFER_TOO_SMALL;
        }
        std::memcpy(buffer, value.c_str(), size);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);

}

SPXAPI bot_activity_property_set(SPXACTIVITYHANDLE h_act, const char* prop_name, const char* buffer)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, prop_name == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, buffer == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        auto act = (*handles)[h_act];
        auto& json = act->GetJSON();
        auto& prop = json[prop_name];
        prop = std::string{ buffer };
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_activity_complex_field_handle(SPXACTIVITYHANDLE h_act, const char* prop_name, SPXACTIVITYJSONHANDLE* ph_json)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, prop_name == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_json == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivity, SPXACTIVITYHANDLE>();
        auto act = (*handles)[h_act];
        auto& json = act->GetJSON();
        auto& prop = json[prop_name];
        if (!prop.is_null() && !prop.is_object() && !prop.is_array())
        {
            return SPXERR_INVALID_STATE;
        }
        auto accessor = SpxCreateObjectWithSite<ISpxActivityJSONAccessor>("CSpxActivityJSONAccessor", SpxGetRootSite());
        accessor->SetAccessor([wk = std::weak_ptr<ISpxActivity>{ act }, field = std::string{ prop_name }]() -> nlohmann::json&
        {
            if (auto act = wk.lock())
            {
                auto& root = act->GetJSON();
                return root[field];
            }
            else
            {
                throw SPXERR_INVALID_STATE;
            }
        });
        auto jsonTable = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        *ph_json = jsonTable->TrackHandle(accessor);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) bot_activity_json_handle_is_valid(SPXACTIVITYJSONHANDLE h_json)
{
    return Handle_IsValid<SPXACTIVITYJSONHANDLE, ISpxActivityJSONAccessor>(h_json);
}
SPXAPI bot_activity_json_handle_release(SPXACTIVITYJSONHANDLE h_json)
{
    return Handle_Close<SPXACTIVITYHANDLE, ISpxActivityJSONAccessor>(h_json);
}

template<typename T>
constexpr auto enum_cast(T v)
{
    return static_cast<std::underlying_type_t<T>>(v);
}

SPXAPI bot_activity_json_get_type(SPXACTIVITYHANDLE h_json, int* type)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, type == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto act = (*handles)[h_json];
        auto& json = act->Get();
        if (json.is_null())
        {
            *type = enum_cast(BotActivityJSONType::Null);
        }
        else if (json.is_array())
        {
            *type = enum_cast(BotActivityJSONType::Array);
        }
        else if (json.is_object())
        {
            *type = enum_cast(BotActivityJSONType::Object);
        }
        else if (json.is_string())
        {
            *type = enum_cast(BotActivityJSONType::String);
        }
        else if (json.is_number_unsigned())
        {
            *type = enum_cast(BotActivityJSONType::UInt);
        }
        else if (json.is_number_integer())
        {
            *type = enum_cast(BotActivityJSONType::Int);
        }
        else if (json.is_number_float())
        {
            *type = enum_cast(BotActivityJSONType::Double);
        }
        else if (json.is_boolean())
        {
            *type = enum_cast(BotActivityJSONType::Boolean);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

/* JSON Object */
SPXAPI bot_activity_json_has_field(SPXACTIVITYJSONHANDLE h_json, const char* field, bool* result)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, field == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, result == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        *result = false;
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto act = (*handles)[h_json];
        auto& json = act->Get();
        *result = !json.is_null();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


SPXAPI bot_activity_json_field_handle(SPXACTIVITYJSONHANDLE h_json, const char* field_name, SPXACTIVITYJSONHANDLE* ph_field)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, field_name == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_field == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto base = (*handles)[h_json];
        /* This will throw if things are not ok */
        base->Get();
        auto accessor = SpxCreateObjectWithSite<ISpxActivityJSONAccessor>("CSpxActivityJSONAccessor", SpxGetRootSite());
        accessor->SetAccessor([wk = std::weak_ptr<ISpxActivityJSONAccessor>{ base }, field = std::string{ field_name }]() -> nlohmann::json&
        {
            if (auto json = wk.lock())
            {
                auto& base = json->Get();
                return base[field];
            }
            else
            {
                throw SPXERR_INVALID_STATE;
            }
        });
        auto jsonTable = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        *ph_field = jsonTable->TrackHandle(accessor);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename T>
SPXHR set_json_value(SPXACTIVITYJSONHANDLE handle, T&& value)
{
    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto accessor = (*handles)[handle];
        auto& json = accessor->Get();
        nlohmann::json json_val = value;
        json = json_val;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename T>
SPXHR get_json_value(SPXACTIVITYJSONHANDLE handle, T* value)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, value == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto accessor = (*handles)[handle];
        auto& json = accessor->Get();
        *value = json.get<T>();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


/* JSON Value */
SPXAPI bot_activity_json_set_int(SPXACTIVITYJSONHANDLE h_json, int64_t val)
{
    return set_json_value(h_json, val);
}

SPXAPI bot_activity_json_set_uint(SPXACTIVITYJSONHANDLE h_json, uint64_t val)
{
    return set_json_value(h_json, val);
}

SPXAPI bot_activity_json_set_double(SPXACTIVITYJSONHANDLE h_json, double val)
{
    return set_json_value(h_json, val);
}

SPXAPI bot_activity_json_set_bool(SPXACTIVITYJSONHANDLE h_json, bool val)
{
    return set_json_value(h_json, val);
}

SPXAPI bot_activity_json_set_string(SPXACTIVITYJSONHANDLE h_json, const char* val)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, val == nullptr);

    return set_json_value(h_json, std::string{ val });
}

SPXAPI bot_activity_json_get_int(SPXACTIVITYJSONHANDLE h_json, int64_t* val)
{
    return get_json_value<int64_t>(h_json, val);
}

SPXAPI bot_activity_json_get_uint(SPXACTIVITYJSONHANDLE h_json, uint64_t* val)
{
    return get_json_value<uint64_t>(h_json, val);
}

SPXAPI bot_activity_json_get_double(SPXACTIVITYJSONHANDLE h_json, double* val)
{
    return get_json_value<double>(h_json, val);
}

SPXAPI bot_activity_json_get_bool(SPXACTIVITYJSONHANDLE h_json, bool* val)
{
    return get_json_value<bool>(h_json, val);
}

SPXAPI bot_activity_json_get_string_size(SPXACTIVITYJSONHANDLE h_json, size_t* size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, size == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto accessor = (*handles)[h_json];
        auto& json = accessor->Get();
        *size = json.get_ref<const std::string&>().size();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_activity_json_get_string(SPXACTIVITYJSONHANDLE h_json, char* buffer, size_t max_size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, buffer == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto accessor = (*handles)[h_json];
        auto& json = accessor->Get();
        auto str = json.get_ref<const std::string&>();
        PAL::strcpy(buffer, max_size, str.c_str(), str.size(), false);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

/* JSON Array */
SPXAPI bot_activity_json_array_size(SPXACTIVITYJSONHANDLE h_json, size_t* size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, size == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto accessor = (*handles)[h_json];
        auto& json = accessor->Get();
        if (!json.is_null() && !json.is_array())
        {
            return SPXERR_INVALID_STATE;
        }
        *size = json.is_null() ? 0 : json.size();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI bot_activity_json_array_item(SPXACTIVITYJSONHANDLE h_json, size_t index, SPXACTIVITYJSONHANDLE* ph_item)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, ph_item == nullptr);

    std::lock_guard<std::mutex> lk{ g_activity_mutex };
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        auto base = (*handles)[h_json];
        /* This will throw if things are not ok */
        auto& json = base->Get();
        if (!json.is_null() && !json.is_array())
        {
            return SPXERR_INVALID_STATE;
        }
        size_t size = json.is_null() ? 0 : json.size();
        if (index > size)
        {
            return SPXERR_OUT_OF_RANGE;
        }
        auto accessor = SpxCreateObjectWithSite<ISpxActivityJSONAccessor>("CSpxActivityJSONAccessor", SpxGetRootSite());
        accessor->SetAccessor([wk = std::weak_ptr<ISpxActivityJSONAccessor>{ base }, index]()->nlohmann::json&
        {
            if (auto json = wk.lock())
            {
                auto& base = json->Get();
                return base[index];
            }
            else
            {
                throw SPXERR_INVALID_STATE;
            }
        });
        auto jsonTable = CSpxSharedPtrHandleTableManager::Get<ISpxActivityJSONAccessor, SPXACTIVITYJSONHANDLE>();
        *ph_item = jsonTable->TrackHandle(accessor);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
