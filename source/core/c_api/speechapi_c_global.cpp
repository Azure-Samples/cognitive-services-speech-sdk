//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "string_utils.h"
#include "resource_manager.h"
#include "service_helpers.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI Global_SetParameter_String(const wchar_t* name, const wchar_t* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        namedProperties->SetStringValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Global_GetParameter_String(const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        auto tempValue = namedProperties->GetStringValue(name, defaultValue);
        PAL::wcscpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Global_HasParameter_String(const wchar_t* name)
{
    bool hasParameter = false;
    SPXAPI_TRY()
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        hasParameter = namedProperties->HasStringValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hasParameter);
}

SPXAPI Global_SetParameter_Int32(const wchar_t* name, int32_t value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        namedProperties->SetNumberValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Global_GetParameter_Int32(const wchar_t* name, int32_t* pvalue, int32_t defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        auto tempValue = namedProperties->GetNumberValue(name, defaultValue);
        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Global_HasParameter_Int32(const wchar_t* name)
{
    bool hasParameter = false;
    SPXAPI_TRY()
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        hasParameter = namedProperties->HasNumberValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hasParameter);
}

SPXAPI Global_SetParameter_Bool(const wchar_t* name, bool value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        namedProperties->SetBooleanValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Global_GetParameter_Bool(const wchar_t* name, bool* pvalue, bool defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        auto tempValue = namedProperties->GetBooleanValue(name, defaultValue);
        *pvalue = tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Global_HasParameter_Bool(const wchar_t* name)
{
    bool hasParameter = false;
    SPXAPI_TRY()
    {
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(CSpxResourceManager::GetObjectFactory());
        hasParameter = namedProperties->HasBooleanValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hasParameter);
}
