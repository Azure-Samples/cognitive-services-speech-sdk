//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_session.cpp: Public API definitions for Session related C methods
//

#include "stdafx.h"
#include "event_helpers.h"
#include "handle_helpers.h"
#include "string_utils.h"
#include "named_properties_constants.h"


using namespace CARBON_IMPL_NAMESPACE();


SPXAPI Session_From_Recognizer(SPXRECOHANDLE hreco, SPXSESSIONHANDLE* phsession)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phsession = SPXHANDLE_INVALID;

        auto recoHandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recoHandles)[hreco];

        auto precognizer = std::dynamic_pointer_cast<ISpxSessionFromRecognizer>(recognizer);
        auto psession = precognizer->GetDefaultSession();

        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        *phsession = sessionHandles->TrackHandle(psession);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Session_Handle_IsValid(SPXSESSIONHANDLE hsession)
{
    return Handle_IsValid<SPXSESSIONHANDLE, ISpxSession>(hsession);
}

SPXAPI Session_Handle_Close(SPXSESSIONHANDLE hsession)
{
    return Handle_Close<SPXSESSIONHANDLE, ISpxSession>(hsession);
}

SPXAPI Session_GetParameter_Name(Session_Parameter parameter, wchar_t* name, uint32_t cchName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        const wchar_t* parameterName = L"";
        switch (parameter)
        {
            case SessionParameter_SpeechSubscriptionKey:
                parameterName = g_SPEECH_SubscriptionKey;
                break;

            case SessionParameter_SpeechEndpoint:
                parameterName = g_SPEECH_Endpoint;
                break;

            default:
                hr = SPXERR_INVALID_ARG;
                break;
        }

        PAL::wcscpy(name, cchName, parameterName, wcslen(parameterName), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_SetParameter_String(SPXSESSIONHANDLE hsession, const wchar_t* name, const wchar_t* value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        namedProperties->SetStringValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_GetParameter_String(SPXSESSIONHANDLE hsession, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        auto tempValue = namedProperties->GetStringValue(name, defaultValue);

        PAL::wcscpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Session_ContainsParameter_String(SPXSESSIONHANDLE hsession, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        return namedProperties->HasStringValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI Session_SetParameter_Int32(SPXSESSIONHANDLE hsession, const wchar_t* name, int32_t value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        namedProperties->SetNumberValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_GetParameter_Int32(SPXSESSIONHANDLE hsession, const wchar_t* name, int32_t* pvalue, int32_t defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        auto tempValue = namedProperties->GetNumberValue(name, defaultValue);

        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Session_ContainsParameter_Int32(SPXSESSIONHANDLE hsession, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        return namedProperties->HasNumberValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI Session_SetParameter_Bool(SPXSESSIONHANDLE hsession, const wchar_t* name, bool value)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        namedProperties->SetBooleanValue(name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_GetParameter_Bool(SPXSESSIONHANDLE hsession, const wchar_t* name, bool* pvalue, bool defaultValue)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        auto tempValue = namedProperties->GetBooleanValue(name, defaultValue);

        *pvalue = tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Session_ContainsParameter_Bool(SPXSESSIONHANDLE hsession, const wchar_t* name)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = std::dynamic_pointer_cast<ISpxNamedProperties>(session);
        return namedProperties->HasBooleanValue(name);
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}
