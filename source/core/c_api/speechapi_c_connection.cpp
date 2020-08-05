//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

#include "stdafx.h"
#include "common.h"
#include "handle_helpers.h"
#include "event_helpers.h"
#include <speechapi_c_conversation_translator.h>

using namespace Microsoft::CognitiveServices::Speech::Impl;

static bool is_conversation_translator_connection(SPXCONNECTIONHANDLE connectionHandle)
{
    // this should throw an exception if the connection handle is invalid
    auto connection = GetInstance<ISpxConnection>(connectionHandle);

    auto convTransConnection = connection->QueryInterface<ConversationTranslation::ISpxConversationTranslatorConnection>();
    return convTransConnection != nullptr;
}

template<typename T>
SPXHR connection_from_object(SPXHANDLE handle, SPXCONNECTIONHANDLE* connectionHandle)
{
    auto handleValid = Handle_IsValid<SPXHANDLE, T>(handle);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, connectionHandle == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !handleValid);
    SPXAPI_INIT_HR_TRY(hr)
    {
        *connectionHandle = SPXHANDLE_INVALID;

        auto connectionProvider = QueryInterfaceFromHandle<T, ISpxConnectionFromRecognizer>(handle);
        SPX_IFTRUE_THROW_HR(connectionProvider == nullptr, SPXERR_EXPLICIT_CONNECTION_NOT_SUPPORTED_BY_RECOGNIZER);
        auto connection = connectionProvider->GetConnection();

        auto connectionHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnection, SPXCONNECTIONHANDLE>();
        *connectionHandle = connectionHandleTable->TrackHandle(connection);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_from_recognizer(SPXRECOHANDLE recognizerHandle, SPXCONNECTIONHANDLE* connectionHandle)
{
    return connection_from_object<ISpxRecognizer>(recognizerHandle, connectionHandle);
}

SPXAPI connection_from_conversation_translator(SPXCONVERSATIONTRANSLATORHANDLE convTransHandle, SPXCONNECTIONHANDLE* connectionHandle)
{
    return connection_from_object<ConversationTranslation::ISpxConversationTranslator>(convTransHandle, connectionHandle);
}

SPXAPI connection_from_dialog_service_connector(SPXRECOHANDLE dialogServiceConnectorHandle, SPXCONNECTIONHANDLE* connectionHandle)
{
    return connection_from_object<ISpxDialogServiceConnector>(dialogServiceConnectorHandle, connectionHandle);
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
    SPXAPI_INIT_HR_TRY(hr)
    {
        // the connected event from a conversation translation is a special case so let's check for that
        // to set the right event handler first
        if (is_conversation_translator_connection(connection))
        {
            hr = conversation_translator_connection_connected_set_callback(connection, callback, context);
        }
        else
        {
            hr = connection_set_event_callback(&ISpxRecognizerEvents::Connected, connection, callback, context);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_disconnected_set_callback(SPXCONNECTIONHANDLE connection, CONNECTION_CALLBACK_FUNC callback, void* context)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        // the connected event from a conversation translation is a special case so let's check for that
        // to set the right event handler first
        if (is_conversation_translator_connection(connection))
        {
            hr = conversation_translator_connection_disconnected_set_callback(connection, callback, context);
        }
        else
        {
            hr = connection_set_event_callback(&ISpxRecognizerEvents::Disconnected, connection, callback, context);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_message_received_set_callback(SPXCONNECTIONHANDLE connection, CONNECTION_CALLBACK_FUNC callback, void* context)
{
    return connection_message_set_event_callback(&ISpxRecognizerEvents::ConnectionMessageReceived, connection, callback, context);
}

SPXAPI connection_open(SPXCONNECTIONHANDLE handle, bool forContinuousRecognition)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_HANDLE, !connection_handle_is_valid(handle));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto connection = GetInstance<ISpxConnection>(handle);
        connection->Open(forContinuousRecognition);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_close(SPXCONNECTIONHANDLE handle)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, !connection_handle_is_valid(handle));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto connection = GetInstance<ISpxConnection>(handle);
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
        auto setter = QueryInterfaceFromHandle<ISpxConnection, ISpxMessageParamFromUser>(handle);
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
        auto setter = QueryInterfaceFromHandle<ISpxConnection, ISpxMessageParamFromUser>(handle);
        SPX_IFTRUE_THROW_HR(setter == nullptr, SPXERR_INVALID_ARG);

        setter->SendNetworkMessage(path, payload);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI connection_send_message_data(SPXCONNECTIONHANDLE handle, const char* path, uint8_t* data, uint32_t size)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, handle == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, path == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, data == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto setter = QueryInterfaceFromHandle<ISpxConnection, ISpxMessageParamFromUser>(handle);
        SPX_IFTRUE_THROW_HR(setter == nullptr, SPXERR_INVALID_ARG);

        std::vector<uint8_t> payload(data, data + size);
        setter->SendNetworkMessage(path, std::move(payload));
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) connection_message_received_event_handle_is_valid(SPXCONNECTIONMESSAGEHANDLE handle)
{
    return Handle_IsValid<SPXEVENTHANDLE, ISpxConnectionMessageEventArgs>(handle);
}

SPXAPI connection_message_received_event_handle_release(SPXEVENTHANDLE hevent)
{
    return Handle_Close<SPXEVENTHANDLE, ISpxConnectionMessageEventArgs>(hevent);
}

SPXAPI connection_message_received_event_get_message(SPXEVENTHANDLE event, SPXCONNECTIONMESSAGEHANDLE* hcm)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        *hcm = SPXHANDLE_INVALID;

        auto connectionEventArgs = GetInstance<ISpxConnectionMessageEventArgs>(event);

        auto message = connectionEventArgs->GetMessage();

        *hcm = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxConnectionMessage, SPXCONNECTIONMESSAGEHANDLE>(message);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) connection_message_handle_is_valid(SPXCONNECTIONMESSAGEHANDLE handle)
{
    return Handle_IsValid<SPXCONNECTIONMESSAGEHANDLE, ISpxConnectionMessage>(handle);
}

SPXAPI connection_message_handle_release(SPXCONNECTIONMESSAGEHANDLE handle)
{
    return Handle_Close<SPXCONNECTIONMESSAGEHANDLE, ISpxConnectionMessage>(handle);
}

SPXAPI connection_message_get_property_bag(SPXCONNECTIONMESSAGEHANDLE hcm, SPXPROPERTYBAGHANDLE* hpropbag)
{
    return GetTargetObjectByInterface<ISpxConnectionMessage, ISpxNamedProperties>(hcm, hpropbag);
}

SPXAPI connection_message_get_data(SPXCONNECTIONMESSAGEHANDLE hcm, uint8_t* data, uint32_t size)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto message = GetInstance<ISpxConnectionMessage>(hcm);

        auto buffer = message->GetBuffer();
        auto bufferSize = message->GetBufferSize();
        SPX_IFTRUE_THROW_HR(size > bufferSize, SPXERR_OUT_OF_RANGE);

        memcpy(data, buffer, size);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(uint32_t) connection_message_get_data_size(SPXCONNECTIONMESSAGEHANDLE hcm)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto message = GetInstance<ISpxConnectionMessage>(hcm);
        return message->GetBufferSize();
    }
    SPXAPI_CATCH_AND_RETURN(hr, 0);
}
