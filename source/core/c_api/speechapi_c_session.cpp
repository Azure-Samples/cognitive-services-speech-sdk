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
#include "speechapi_cxx_session_parameter_collection.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

static_assert((int)SessionParameter_SubscriptionKey == (int)::Microsoft::CognitiveServices::Speech::SessionParameter::SubscriptionKey, "SessionParameter_* enum values == SessionParameter::* enum values");
static_assert((int)SessionParameter_AuthorizationToken == (int)::Microsoft::CognitiveServices::Speech::SessionParameter::AuthorizationToken, "SessionParameter_* enum values == SessionParameter::* enum values");

SPXAPI Session_From_Recognizer(SPXRECOHANDLE hreco, SPXSESSIONHANDLE* phsession)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *phsession = SPXHANDLE_INVALID;

        auto recoHandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recoHandles)[hreco];

        auto precognizer = SpxQueryInterface<ISpxSessionFromRecognizer>(recognizer);
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

SPXAPI Session_GetParameter_Name(Session_Parameter parameter, char* name, uint32_t cchName)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        const char* parameterName = "";
        switch (parameter)
        {
            case SessionParameter_SubscriptionKey:
                parameterName = g_SPEECH_SubscriptionKey;
                break;

            case SessionParameter_AuthorizationToken:
                parameterName = g_SPEECH_AuthToken;
                break;

            default:
                hr = SPXERR_INVALID_ARG;
                break;
        }

        PAL::strcpy(name, cchName, parameterName, strlen(parameterName), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_SetParameter_String(SPXSESSIONHANDLE hsession, const char* name, const char* value)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    if (value == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        namedProperties->SetStringValue(PAL::ToWString(name).c_str(), PAL::ToWString(value).c_str());
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_GetParameter_String(SPXSESSIONHANDLE hsession, const char* name, char* value, uint32_t cchValue, const char* defaultValue)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    if (value == nullptr)
        return SPXERR_INVALID_ARG;

    if (defaultValue == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        auto tempValue = PAL::ToString(namedProperties->GetStringValue(PAL::ToWString(name).c_str(), PAL::ToWString(defaultValue).c_str()));

        PAL::strcpy(value, cchValue, tempValue.c_str(), tempValue.size(), true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Session_ContainsParameter_String(SPXSESSIONHANDLE hsession, const char* name)
{
    if (name == nullptr)
        return false;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        return namedProperties->HasStringValue(PAL::ToWString(name).c_str());
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI Session_SetParameter_Int32(SPXSESSIONHANDLE hsession, const char* name, int32_t value)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        namedProperties->SetNumberValue(PAL::ToWString(name).c_str(), value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_GetParameter_Int32(SPXSESSIONHANDLE hsession, const char* name, int32_t* pvalue, int32_t defaultValue)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    if (pvalue == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        auto tempValue = namedProperties->GetNumberValue(PAL::ToWString(name).c_str(), defaultValue);

        *pvalue = (int32_t)tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Session_ContainsParameter_Int32(SPXSESSIONHANDLE hsession, const char* name)
{
    if (name == nullptr)
        return false;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        return namedProperties->HasNumberValue(PAL::ToWString(name).c_str());
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}

SPXAPI Session_SetParameter_Bool(SPXSESSIONHANDLE hsession, const char* name, bool value)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        namedProperties->SetBooleanValue(PAL::ToWString(name).c_str(), value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI Session_GetParameter_Bool(SPXSESSIONHANDLE hsession, const char* name, bool* pvalue, bool defaultValue)
{
    if (name == nullptr)
        return SPXERR_INVALID_ARG;

    if (pvalue == nullptr)
        return SPXERR_INVALID_ARG;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        auto tempValue = namedProperties->GetBooleanValue(PAL::ToWString(name).c_str(), defaultValue);

        *pvalue = tempValue;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) Session_ContainsParameter_Bool(SPXSESSIONHANDLE hsession, const char* name)
{
    if (name == nullptr)
        return false;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto sessionHandles = CSpxSharedPtrHandleTableManager::Get<ISpxSession, SPXSESSIONHANDLE>();
        auto session = (*sessionHandles)[hsession];

        auto namedProperties = SpxQueryInterface<ISpxNamedProperties>(session);
        return namedProperties->HasBooleanValue(PAL::ToWString(name).c_str());
    }
    SPXAPI_CATCH_AND_RETURN(hr, false)
}
