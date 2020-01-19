//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// event_helpers.cpp: Private implementation definitions for EventSignal related C methods
//
#include "stdafx.h"
#include "event_helpers.h"
#include "handle_table.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

SPXAPI_PRIVATE recognizer_session_set_event_callback(ISpxRecognizerEvents::SessionEvent_Type ISpxRecognizerEvents::*psessionEvent, SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto pfn = [=](std::shared_ptr<ISpxSessionEventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSessionEventArgs, SPXEVENTHANDLE>();
            auto hevent = eventhandles->TrackHandle(e);
            (*pCallback)(hreco, hevent, pvContext);
        };

        auto pISpxRecognizerEvents = SpxQueryInterface<ISpxRecognizerEvents>(recognizer).get();
        (pISpxRecognizerEvents->*psessionEvent).Disconnect(pfn);

        if (pCallback != nullptr)
        {
            (pISpxRecognizerEvents->*psessionEvent).Connect(pfn);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_PRIVATE recognizer_recognition_set_event_callback(ISpxRecognizerEvents::RecoEvent_Type ISpxRecognizerEvents::*precoEvent, SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto recohandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognizer, SPXRECOHANDLE>();
        auto recognizer = (*recohandles)[hreco];

        auto pfn = [=](std::shared_ptr<ISpxRecognitionEventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxRecognitionEventArgs, SPXEVENTHANDLE>();
            auto hevent = eventhandles->TrackHandle(e);
            (*pCallback)(hreco, hevent, pvContext);
        };

        auto pISpxRecognizerEvents = SpxQueryInterface<ISpxRecognizerEvents>(recognizer).get();
        (pISpxRecognizerEvents->*precoEvent).Disconnect(pfn);

        if (pCallback != nullptr)
        {
            (pISpxRecognizerEvents->*precoEvent).Connect(pfn);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_PRIVATE connection_set_event_callback(ISpxRecognizerEvents::ConnectionEvent_Type ISpxRecognizerEvents::*connectionEvent, SPXCONNECTIONHANDLE connectionHandle, CONNECTION_CALLBACK_FUNC callback, void* context)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(connectionEvent == nullptr, SPXERR_INVALID_ARG);
        auto connectionHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnection, SPXCONNECTIONHANDLE>();
        auto connection = (*connectionHandleTable)[connectionHandle];

        auto pfn = [=](std::shared_ptr<ISpxConnectionEventArgs> e) {
            auto connectionEventHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConnectionEventArgs, SPXEVENTHANDLE>();
            auto eventHandle = connectionEventHandleTable->TrackHandle(e);
            (*callback)(eventHandle, context);
        };

        auto recognizer = connection->GetRecognizer();
        // for Disconnect() call, if the recognizer is not valid, we just return.
        SPX_IFTRUE_THROW_HR(recognizer == nullptr && callback != nullptr, SPXERR_INVALID_RECOGNIZER);
        if (recognizer != nullptr)
        {
            auto pISpxRecognizerEvents = SpxQueryInterface<ISpxRecognizerEvents>(recognizer).get();
            SPX_IFTRUE_THROW_HR(pISpxRecognizerEvents == nullptr, SPXERR_RUNTIME_ERROR);
            (pISpxRecognizerEvents->*connectionEvent).Disconnect(pfn);

            if (callback != nullptr)
            {
                (pISpxRecognizerEvents->*connectionEvent).Connect(pfn);
            }
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_PRIVATE connection_message_set_event_callback(ISpxRecognizerEvents::ConnectionMessageEvent_Type ISpxRecognizerEvents::*connectionMessageEvent, SPXCONNECTIONHANDLE connectionHandle, CONNECTION_CALLBACK_FUNC callback, void* context)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPX_IFTRUE_THROW_HR(connectionMessageEvent == nullptr, SPXERR_INVALID_ARG);
        auto connection = CSpxSharedPtrHandleTableManager::GetPtr<ISpxConnection, SPXCONNECTIONHANDLE>(connectionHandle);

        auto pfn = [=](std::shared_ptr<ISpxConnectionMessageEventArgs> e) {
            auto eventHandle = CSpxSharedPtrHandleTableManager::TrackHandle<ISpxConnectionMessageEventArgs, SPXEVENTHANDLE>(e);
            (*callback)(eventHandle, context);
        };

        auto recognizer = connection->GetRecognizer();
        SPX_IFTRUE_THROW_HR(recognizer == nullptr && callback != nullptr, SPXERR_INVALID_RECOGNIZER);

        auto pISpxRecognizerEvents = SpxQueryInterface<ISpxRecognizerEvents>(recognizer).get();
        SPX_IFTRUE_THROW_HR(pISpxRecognizerEvents == nullptr, SPXERR_RUNTIME_ERROR);
        (pISpxRecognizerEvents->*connectionMessageEvent).Disconnect(pfn);

        if (callback != nullptr)
        {
            (pISpxRecognizerEvents->*connectionMessageEvent).Connect(pfn);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_PRIVATE synthesizer_set_event_callback(std::list<std::pair<void*, std::shared_ptr<ISpxSynthesizerEvents::SynthEvent_Type>>> ISpxSynthesizerEvents::*psynthEvents, SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto pfn = [=](std::shared_ptr<ISpxSynthesisEventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesisEventArgs, SPXEVENTHANDLE>();
            auto hevent = eventhandles->TrackHandle(e);
            (*pCallback)(hsynth, hevent, pvContext);
        };

        auto pISpxSynthesizerEvents = SpxQueryInterface<ISpxSynthesizerEvents>(synthesizer).get();

        // Clean up existing callback hooks to pfn
        auto iterator = (pISpxSynthesizerEvents->*psynthEvents).begin();
        while (iterator != (pISpxSynthesizerEvents->*psynthEvents).end())
        {
            iterator->second->Disconnect(pfn);
            if (!iterator->second->IsConnected())
            {
                std::pair<void*, std::shared_ptr<ISpxSynthesizerEvents::SynthEvent_Type>> pair = { iterator->first, iterator->second };
                iterator++;
                (pISpxSynthesizerEvents->*psynthEvents).remove(pair);
                continue;
            }

            iterator++;
        }

        // Add pfn
        if (pCallback != nullptr)
        {
            auto synthesisEvent = std::make_shared<EventSignal<std::shared_ptr<ISpxSynthesisEventArgs>>>();
            synthesisEvent->Connect(pfn);
            (pISpxSynthesizerEvents->*psynthEvents).emplace_back(pvContext, synthesisEvent);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_PRIVATE synthesizer_word_boundary_set_event_callback(ISpxSynthesizerEvents::WordBoundaryEvent_Type ISpxSynthesizerEvents::*pwordBoundaryEvent, SPXSYNTHHANDLE hsynth, PSYNTHESIS_CALLBACK_FUNC pCallback, void* pvContext)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto synthhandles = CSpxSharedPtrHandleTableManager::Get<ISpxSynthesizer, SPXSYNTHHANDLE>();
        auto synthesizer = (*synthhandles)[hsynth];

        auto pfn = [=](std::shared_ptr<ISpxWordBoundaryEventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<ISpxWordBoundaryEventArgs, SPXEVENTHANDLE>();
            auto hevent = eventhandles->TrackHandle(e);
            (*pCallback)(hsynth, hevent, pvContext);
        };

        auto pISpxSynthesizerEvents = SpxQueryInterface<ISpxSynthesizerEvents>(synthesizer).get();

        // Disconnect pfn first to avoid duplication
        (pISpxSynthesizerEvents->*pwordBoundaryEvent).Disconnect(pfn);

        // Add pfn
        if (pCallback != nullptr)
        {
            (pISpxSynthesizerEvents->*pwordBoundaryEvent).Connect(pfn);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename EventInterface, typename EventArgs, typename Event>
SPXHR dialog_service_connector_set_event_callback(Event event, SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void* pv_context)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxDialogServiceConnector, SPXRECOHANDLE>();
        auto connector = (*handles)[h_connector];

        auto pfn = [=](std::shared_ptr<EventArgs> e) {
            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<EventArgs, SPXEVENTHANDLE>();
            auto h_event = eventhandles->TrackHandle(e);
            (*p_callback)(h_connector, h_event, pv_context);
        };

        auto events = SpxQueryInterface<EventInterface>(connector).get();
        (events->*event).Disconnect(pfn);

        if (p_callback != nullptr)
        {
            (events->*event).Connect(pfn);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_PRIVATE dialog_service_connector_session_set_event_callback(ISpxRecognizerEvents::SessionEvent_Type ISpxRecognizerEvents::*p_session_event, SPXRECOHANDLE h_connector, PSESSION_CALLBACK_FUNC p_callback, void* pv_context)
{
    return dialog_service_connector_set_event_callback<ISpxRecognizerEvents, ISpxSessionEventArgs>(p_session_event, h_connector, p_callback, pv_context);
}

SPXAPI_PRIVATE dialog_service_connector_recognition_set_event_callback(ISpxRecognizerEvents::RecoEvent_Type ISpxRecognizerEvents::*p_reco_event, SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void* pv_context)
{
    return dialog_service_connector_set_event_callback<ISpxRecognizerEvents, ISpxRecognitionEventArgs>(p_reco_event, h_connector, p_callback, pv_context);
}

SPXAPI_PRIVATE dialog_service_connector_activity_received_set_event_callback(ISpxDialogServiceConnectorEvents::ActivityReceivedEvent_Type ISpxDialogServiceConnectorEvents::*p_act_event, SPXRECOHANDLE h_connector, PRECOGNITION_CALLBACK_FUNC p_callback, void* pv_context)
{
    return dialog_service_connector_set_event_callback<ISpxDialogServiceConnectorEvents, ISpxActivityEventArgs>(p_act_event, h_connector, p_callback, pv_context);
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
