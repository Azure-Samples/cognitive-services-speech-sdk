//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "handle_helpers.h"
#include "event_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

SPXAPI connection_from_recognizer(SPXRECOHANDLE recognizerHandle, SPXCONNECTIONHANDLE* connectionHandle)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, connectionHandle == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !recognizer_handle_is_valid(recognizerHandle));

    SPXAPI_INIT_HR_TRY(hr)
    {
        *connectionHandle = SPXHANDLE_INVALID;

        auto recoHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recoHandleTable)[recognizerHandle];
        SPX_IFTRUE_THROW_HR(recognizer == nullptr, SPXERR_INVALID_HANDLE);

        auto recoForConnection = SpxQueryInterface<ISpxConnectionFromRecognizer>(recognizer);
        SPX_IFTRUE_THROW_HR(recoForConnection == nullptr, SPXERR_EXPLICIT_CONNECTION_NOT_SUPPORTED_BY_RECOGNIZER);
        auto connection = recoForConnection->GetConnection();

        auto connectionHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnection, SPXCONNECTIONHANDLE>();
        *connectionHandle = connectionHandleTable->TrackHandle(connection);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}


SPXAPI_(bool) connection_handle_is_valid(SPXCONNECTIONHANDLE handle)
{
    return Handle_IsValid<SPXCONNECTIONHANDLE, ISpxConnection>(handle);
}

SPXAPI connection_handle_release(SPXCONNECTIONHANDLE handle)
{
    return Handle_Close<SPXCONNECTIONHANDLE, ISpxConnection>(handle);
}

SPXAPI connection_connected_set_callback(SPXCONNECTIONHANDLE connection, CONNECTION_CALLBACK_FUNC callback, void* context)
{
    return connection_set_event_callback(&ISpxRecognizerEvents::Connected, connection, callback, context);
}

SPXAPI connection_disconnected_set_callback(SPXCONNECTIONHANDLE connection, CONNECTION_CALLBACK_FUNC callback, void* context)
{
    return connection_set_event_callback(&ISpxRecognizerEvents::Disconnected, connection, callback, context);
}

SPXAPI connection_open(SPXCONNECTIONHANDLE handle, bool forContinuousRecognition)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, !connection_handle_is_valid(handle));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnection, SPXCONNECTIONHANDLE>();
        auto connection = (*handleTable)[handle];
        SPX_IFTRUE_THROW_HR(connection == nullptr, SPXERR_INVALID_HANDLE);
        connection->Open(forContinuousRecognition);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_close(SPXCONNECTIONHANDLE handle)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !connection_handle_is_valid(handle));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnection, SPXCONNECTIONHANDLE>();
        auto connection = (*handleTable)[handle];
        SPX_IFTRUE_THROW_HR(connection == nullptr, SPXERR_INVALID_HANDLE);
        connection->Close();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_set_message_property(SPXCONNECTIONHANDLE handle, const char* path, const char* name, const char* value)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, handle == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, name == nullptr || !(*name));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, name == nullptr || !(*path));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, value == nullptr || !(*value));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnection, SPXCONNECTIONHANDLE>();
        auto connection = (*handleTable)[handle];
        SPX_IFTRUE_THROW_HR(connection == nullptr, SPXERR_INVALID_HANDLE);

        auto setter = SpxQueryInterface<ISpxMessageParamFromUser>(connection);
        SPX_IFTRUE_THROW_HR(setter == nullptr, SPXERR_INVALID_ARG);

        setter->SetParameter(path, name, value);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_send_message(SPXCONNECTIONHANDLE handle, const char* path, const char* payload)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, handle == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, payload == nullptr || !(*payload));
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, path == nullptr || !(*path));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnection, SPXCONNECTIONHANDLE>();
        auto connection = (*handleTable)[handle];

        auto setter = SpxQueryInterface<ISpxMessageParamFromUser>(connection);
        SPX_IFTRUE_THROW_HR(setter == nullptr, SPXERR_INVALID_ARG);

        setter->SendNetworkMessage(path, payload);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}
