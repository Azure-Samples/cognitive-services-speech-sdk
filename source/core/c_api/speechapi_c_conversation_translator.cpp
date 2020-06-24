//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_conversation_translator.cpp: Public API definitions for ConversationTranslator related C methods
//

#include "stdafx.h"
#include <ISpxConversationInterfaces.h>
#include <service_helpers.h>
#include "handle_helpers.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;
using namespace Microsoft::CognitiveServices::Speech::Impl::ConversationTranslation;

template<typename TInterface>
static std::shared_ptr<TInterface> GetInstance(typename TInterface::HandleType handle)
{
    SPX_IFTRUE_THROW_HR(handle == SPXHANDLE_INVALID, SPXERR_INVALID_HANDLE);

    auto handles = CSpxSharedPtrHandleTableManager::Get<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>();
    auto interface = (*handles)[handle];
    auto ptr = std::dynamic_pointer_cast<TInterface>(interface);

    SPX_IFTRUE_THROW_HR(ptr == nullptr, SPXERR_INVALID_HANDLE);
    return ptr;
}

template<typename TInterface>
static std::shared_ptr<TInterface> TryGetInstance(typename TInterface::HandleType handle)
{
    if (handle == SPXHANDLE_INVALID)
    {
        return nullptr;
    }

    auto handles = CSpxSharedPtrHandleTableManager::Get<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>();
    auto interface = (*handles)[handle];
    return std::dynamic_pointer_cast<TInterface>(interface);
}

static inline std::shared_ptr<ISpxConversation> GetConversation(SPXCONVERSATIONHANDLE hconv)
{
    return GetInstance<ISpxConversation>(hconv);
}

static inline std::shared_ptr<ISpxConversationTranslator> GetTranslator(SPXCONVERSATIONTRANSLATORHANDLE handle)
{
    return GetInstance<ISpxConversationTranslator>(handle);
}

template<typename TInterface>
static inline bool _Handle_IsValid(SPXHANDLE handle)
{
    // NOTE: GCC gets upset by the HandleInterface Type being abstract. So just copy method contents here
    //return Handle_IsValid<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>(static_cast<typename TInterface::HandleType>(handle));

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handletable = CSpxSharedPtrHandleTableManager::Get<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>();
        return handletable->IsTracked(static_cast<typename TInterface::HandleType>(handle));
    }
    SPXAPI_CATCH_AND_RETURN(hr, false);
}

template<typename TInterface>
static inline SPXHR _Handle_Close(SPXHANDLE handle)
{
    // NOTE: GCC gets upset by the HandleInterface Type being abstract. So just copy method contents here
    //return Handle_Close<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>(static_cast<typename TInterface::HandleType>(handle));

    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, handle == nullptr);
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto handletable = CSpxSharedPtrHandleTableManager::Get<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>();
        handletable->StopTracking(static_cast<typename TInterface::HandleType>(handle));
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename TInterface>
static SPXHR SetCallback(
    EventSignal<std::shared_ptr<TInterface>> ISpxConversationTranslator::* pEvt,
    SPXCONVERSATIONTRANSLATORHANDLE hConvTrans,
    PCONV_TRANS_CALLBACK pCallback,
    void * pvCtxt)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto convTrans = GetTranslator(hConvTrans);
        std::weak_ptr<ISpxInterfaceBase> weak(convTrans->shared_from_this());

        auto eventHandler = [weak, hConvTrans, pCallback, pvCtxt](std::shared_ptr<TInterface> e)
        {
            auto keepAlive = weak.lock();
            // safety checks
            if (keepAlive == nullptr || keepAlive != GetTranslator(hConvTrans))
            {
                SPX_TRACE_ERROR("The conversation translator instance has been disposed OR does not match retrieved handle value");
                return;
            }

            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>();
            auto hevent = eventhandles->TrackHandle(e);
            (*pCallback)(hConvTrans, hevent, pvCtxt);
        };

        (convTrans.get()->*pEvt).Disconnect(eventHandler);
        if (pCallback != nullptr)
        {
            (convTrans.get()->*pEvt).Connect(eventHandler);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename TInterface>
static SPXHR SetCallback(
    EventSignal<std::shared_ptr<TInterface>> ISpxConversationTranslator::* pEvt,
    SPXCONVERSATIONTRANSLATORHANDLE hConvTrans,
    CONNECTION_CALLBACK_FUNC pCallback,
    void * pvCtxt)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto convTrans = GetTranslator(hConvTrans);
        std::weak_ptr<ISpxInterfaceBase> weak(convTrans->shared_from_this());

        auto eventHandler = [weak, hConvTrans, pCallback, pvCtxt](std::shared_ptr<TInterface> e)
        {
            auto keepAlive = weak.lock();
            // safety checks
            if (keepAlive == nullptr || keepAlive != GetTranslator(hConvTrans))
            {
                SPX_TRACE_ERROR("The conversation translator instance has been disposed OR does not match retrieved handle value");
                return;
            }

            auto eventhandles = CSpxSharedPtrHandleTableManager::Get<typename TInterface::HandleInterfaceType, typename TInterface::HandleType>();
            auto hevent = eventhandles->TrackHandle(e);
            (*pCallback)(hevent, pvCtxt);
        };

        (convTrans.get()->*pEvt).Disconnect(eventHandler);
        if (pCallback != nullptr)
        {
            (convTrans.get()->*pEvt).Connect(eventHandler);
        }
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename TValue, typename TInterface, typename THandle>
static inline SPXHR _RetrieveValue(THandle handle, TValue* pVal, TValue(TInterface::*getter)() const)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pVal == nullptr);
    auto evt = TryGetInstance<TInterface>(handle);
    SPX_IFTRUE_RETURN_HR(evt == nullptr, SPXERR_INVALID_HANDLE);
    *pVal = (evt.get()->*getter)();
    return SPX_NOERROR;
}

template<typename TValue, typename TInterface, typename THandle>
static inline SPXHR RetrieveValue(THandle handle, TValue* pVal, TValue(TInterface::*getter)() const)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        return _RetrieveValue(handle, pVal, getter);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

template<typename TValue, typename TInterface, typename THandle>
static inline SPXHR TryRetrieveValue(THandle handle, TValue* pVal, TValue(TInterface::*getter)() const)
{
    try
    {
        return _RetrieveValue(handle, pVal, getter);
    }
    catch (...)
    {
        // return default value
        *pVal = TValue{};
        return SPX_NOERROR;
    }
}

/// <summary>
/// Helper method to read a string value.
/// </summary>
/// <param name="handle">The handle to the object to read from</param>
/// <param name="psz">A pointer to null terminated string. We will copy the string to here. Set this
/// to nullptr to query the length of the string.</param>
/// <param name="pcch">A pointer to an in/out length value. If psz is a valid pointer, this will be
/// used to determine the length of the psz buffer available. On success, this will always be set
/// be set to the length of the buffer needed or the number of bytes copied.</param>
/// <param name="getter">The pointer to an instance method to retrieve a string value</param>
template<typename TInterface, typename THandle, typename TString>
static inline SPXHR _RetrieveStringValue(THandle handle, char * psz, uint32_t * pcch, TString(TInterface::*getter)() const)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pcch == nullptr);

    auto evt = TryGetInstance<TInterface>(handle);
    SPX_IFTRUE_RETURN_HR(evt == nullptr, SPXERR_INVALID_HANDLE);

    std::string str = Utils::ToUTF8((evt.get()->*getter)());

    if (psz == nullptr)
    {
        // we are querying the string length so just set that and return
        *pcch = static_cast<int32_t>(str.length() + 1);
    }
    else
    {
        // copy over the string value with a trailing \0. This will truncate
        // strings that are too long
        size_t length = str.length() + 1;
        if (*pcch < length)
        {
            length = static_cast<size_t>(*pcch);
        }

        *pcch = static_cast<uint32_t>(snprintf(psz, length, "%s", str.c_str()));
    }

    return SPX_NOERROR;
}

/// <summary>
/// Retrieves strings capturing exceptions to handle
/// </summary>
template<typename TInterface, typename THandle, typename TString>
static inline SPXHR RetrieveStringValue(THandle handle, char * psz, uint32_t * pcch, TString(TInterface::*getter)() const)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        return _RetrieveStringValue(handle, psz, pcch, getter);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

/// <summary>
/// Retrieves strings *without* capturing exceptions to handle
/// </summary>
template<typename TInterface, typename THandle, typename TString>
static inline SPXHR TryRetrieveStringValue(THandle handle, char * psz, uint32_t * pcch, TString(TInterface::*getter)() const)
{
    try
    {
        return _RetrieveStringValue(handle, psz, pcch, getter);
    }
    catch (...)
    {
        uint32_t oldLength = pcch == nullptr ? 0 : *pcch;
        *pcch = 0;
        if (oldLength > 0)
        {
            *psz = '\0';
        }

        return SPX_NOERROR;
    }
}

static void ConversationTranslatorJoin(std::shared_ptr<ISpxConversationTranslator> convTranslator, SPXCONVERSATIONHANDLE hconv, const char* psznickname, bool isHost)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, hconv == nullptr);
    SPX_THROW_HR_IF(SPXERR_INVALID_ARG, psznickname == nullptr);

    // protect against calling join twice and accidentally destroying internal state
    SPX_IFTRUE_THROW_HR(!convTranslator->CanJoin(), SPXERR_INVALID_STATE);

    auto conv = GetConversation(hconv);

    auto factory = SpxQueryService<ISpxSpeechApiFactory>(conv);
    SPX_IFTRUE_THROW_HR(factory == nullptr, SPXERR_RUNTIME_ERROR);

    auto session = SpxQueryService<ISpxSession>(conv);
    SPX_IFTRUE_THROW_HR(session == nullptr, SPXERR_RUNTIME_ERROR);

    auto session_as_site = SpxQueryInterface<ISpxGenericSite>(session); //ISpxRecognizerSite
    auto conv_translator_with_site = SpxQueryInterface<ISpxObjectWithSite>(convTranslator);

    // NOTE: The SetSite() call will call Init() on the conversation translator instance. This
    // will also register a recognizer with the session;
    conv_translator_with_site->SetSite(session_as_site);

    // hook audio input to session
    auto audio_input = SpxQueryInterface<ISpxObjectWithAudioConfig>(conv_translator_with_site);
    factory->InitSessionFromAudioInputConfig(SpxQueryInterface<ISpxAudioStreamSessionInit>(session), audio_input->GetAudioConfig());

    // Call the implementation join method
    convTranslator->JoinConversation(conv, psznickname, isHost);
}

SPXAPI conversation_translator_get_property_bag(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator, SPXPROPERTYBAGHANDLE * phpropertyBag)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconvtranslator == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phpropertyBag == nullptr);

    SPXAPI_INIT_HR_TRY(hr)
    {
        *phpropertyBag = SPXHANDLE_INVALID;

        auto convTranslator = GetTranslator(hconvtranslator);
        auto namedProperties = SpxQueryService<ISpxNamedProperties>(convTranslator);

        auto handles = CSpxSharedPtrHandleTableManager::Get<ISpxNamedProperties, SPXPROPERTYBAGHANDLE>();
        *phpropertyBag = handles->TrackHandle(namedProperties);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_join(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator, SPXCONVERSATIONHANDLE hconv, const char* psznickname)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        auto instance = GetTranslator(hconvtranslator);

        // protect against calling join twice and accidentally destroying internal state
        SPX_IFTRUE_THROW_HR(!instance->CanJoin(), SPXERR_INVALID_STATE);

        ConversationTranslatorJoin(instance, hconv, psznickname, true);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_join_with_id(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator, const char * pszconversationid, const char * psznickname, const char * pszlang)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, hconvtranslator == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pszconversationid == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, psznickname == nullptr);
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, pszlang == nullptr);

    SPXSPEECHCONFIGHANDLE h_speechConfig = SPXHANDLE_INVALID;
    SPXPROPERTYBAGHANDLE h_speechConfigPropertyBag = SPXHANDLE_INVALID;
    SPXPROPERTYBAGHANDLE h_convTransPropertyBag = SPXHANDLE_INVALID;
    SPXCONVERSATIONHANDLE h_converation = SPXHANDLE_INVALID;

    SPXHR hr = SPX_NOERROR;
    try
    {
        auto instance = GetTranslator(hconvtranslator);

        // protect against calling join twice and accidentally destroying internal state
        SPX_IFTRUE_THROW_HR(!instance->CanJoin(), SPXERR_INVALID_STATE);

        // create a placeholder speech config first. Since we don't need a subscription key when joining,
        // pass in dummy values
        SPX_THROW_ON_FAIL(speech_config_from_subscription(
            &h_speechConfig,
            "abcdefghijklmnopqrstuvwxyz012345",
            "westus"));

        // retrieve the property bag handle and clear the dummy values we set
        SPX_THROW_ON_FAIL(speech_config_get_property_bag(h_speechConfig, &h_speechConfigPropertyBag));
        SPX_THROW_ON_FAIL(property_bag_set_string(
            h_speechConfigPropertyBag,
            static_cast<int>(PropertyId::SpeechServiceConnection_Key),
            nullptr,
            ""));
        SPX_THROW_ON_FAIL(property_bag_set_string(
            h_speechConfigPropertyBag,
            static_cast<int>(PropertyId::SpeechServiceConnection_Region),
            nullptr,
            ""));

        // set the speech recognition language
        SPX_THROW_ON_FAIL(property_bag_set_string(
            h_speechConfigPropertyBag,
            static_cast<int>(PropertyId::SpeechServiceConnection_RecoLanguage),
            nullptr,
            pszlang));

        // copy over the properties set on the conversation translator to the speech config object
        SPX_THROW_ON_FAIL(conversation_translator_get_property_bag(hconvtranslator, &h_convTransPropertyBag));
        SPX_THROW_ON_FAIL(property_bag_copy(h_convTransPropertyBag, h_speechConfigPropertyBag));

        // now create a conversation instance and start the conversation
        SPX_THROW_ON_FAIL(conversation_create_from_config(&h_converation, h_speechConfig, pszconversationid));
        SPX_THROW_ON_FAIL(conversation_start_conversation(h_converation));

        // now join the conversation instance
        ConversationTranslatorJoin(instance, h_converation, psznickname, false);

        SPX_REPORT_ON_FAIL(conversation_release_handle(h_converation));
        h_converation = SPXHANDLE_INVALID;
    }
    SPXAPI_CATCH(hr);

    // finally clean up handles
    conversation_release_handle(h_converation);
    property_bag_release(h_convTransPropertyBag);
    property_bag_release(h_speechConfigPropertyBag);
    speech_config_release(h_speechConfig);

    return hr;
}

SPXAPI conversation_translator_start_transcribing(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetTranslator(hconvtranslator)->StartTranscribing();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_stop_transcribing(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetTranslator(hconvtranslator)->StopTranscribing();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_send_text_message(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator, const char * pszmessage)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetTranslator(hconvtranslator)->SendTextMsg(pszmessage);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_set_authorization_token(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator, const char* pszAuthToken, const char* pszRegion)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetTranslator(hconvtranslator)->SetAuthorizationToken(pszAuthToken, pszRegion);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_leave(SPXCONVERSATIONTRANSLATORHANDLE hconvtranslator)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        GetTranslator(hconvtranslator)->LeaveConversation();
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI_(bool) conversation_translator_handle_is_valid(SPXCONVERSATIONTRANSLATORHANDLE handle)
{
    return handle != SPXHANDLE_INVALID
        && Handle_IsValid<SPXCONVERSATIONTRANSLATORHANDLE, ISpxConversationTranslator>(handle);
}

SPXAPI conversation_translator_handle_release(SPXHANDLE handle)
{
    if (handle == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (Handle_IsValid<SPXCONVERSATIONTRANSLATORHANDLE, ISpxConversationTranslator>(handle))
    {
        return Handle_Close<SPXCONVERSATIONTRANSLATORHANDLE, ISpxConversationTranslator>(handle);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}

SPXAPI conversation_translator_session_started_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::SessionStarted, hConvTrans, pCallback, pvContext);
}

SPXAPI conversation_translator_session_stopped_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::SessionStopped, hConvTrans, pCallback, pvContext);
}

static SPXCONVERSATIONTRANSLATORHANDLE GetConversationTranslatorHandleFromConnection(SPXCONNECTIONHANDLE hConnection)
{
    auto connection = GetInstance<ISpxConnection>(hConnection);

    auto convTransConnection = connection->QueryInterface<ISpxConversationTranslatorConnection>();
    SPX_IFTRUE_THROW_HR(convTransConnection == nullptr, SPXERR_INVALID_HANDLE);

    auto convTrans = convTransConnection->GetConversationTranslator();
    SPX_IFTRUE_THROW_HR(convTrans == nullptr, SPXERR_INVALID_HANDLE);

    auto convTransHandleTable = CSpxSharedPtrHandleTableManager::Get<ISpxConversationTranslator, SPXCONVERSATIONTRANSLATORHANDLE>();
    return (*convTransHandleTable)[convTrans.get()];
}

SPXAPI conversation_translator_connection_connected_set_callback(SPXCONNECTIONHANDLE hConnection, CONNECTION_CALLBACK_FUNC pCallback, void * pvContext)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPXCONVERSATIONTRANSLATORHANDLE hConvTrans = GetConversationTranslatorHandleFromConnection(hConnection);
        return SetCallback(&ISpxConversationTranslator::Connected, hConvTrans, pCallback, pvContext);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_connection_disconnected_set_callback(SPXCONNECTIONHANDLE hConnection, CONNECTION_CALLBACK_FUNC pCallback, void * pvContext)
{
    SPXAPI_INIT_HR_TRY(hr)
    {
        SPXCONVERSATIONTRANSLATORHANDLE hConvTrans = GetConversationTranslatorHandleFromConnection(hConnection);
        return SetCallback(&ISpxConversationTranslator::Disconnected, hConvTrans, pCallback, pvContext);
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_canceled_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::Canceled, hConvTrans, pCallback, pvContext);
}

SPXAPI conversation_translator_participants_changed_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::ParticipantsChanged, hConvTrans, pCallback, pvContext);
}

SPXAPI conversation_translator_conversation_expiration_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::ConversationExpiration, hConvTrans, pCallback, pvContext);
}

SPXAPI conversation_translator_transcribing_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::Transcribing, hConvTrans, pCallback, pvContext);
}

SPXAPI conversation_translator_transcribed_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::Transcribed, hConvTrans, pCallback, pvContext);
}

SPXAPI conversation_translator_text_message_recevied_set_callback(SPXCONVERSATIONTRANSLATORHANDLE hConvTrans, PCONV_TRANS_CALLBACK pCallback, void * pvContext)
{
    return SetCallback(&ISpxConversationTranslator::TextMessageReceived, hConvTrans, pCallback, pvContext);
}

SPXAPI conversation_translator_event_handle_release(SPXHANDLE hevt)
{
    if (hevt == SPXHANDLE_INVALID)
    {
        return SPX_NOERROR;
    }
    else if (_Handle_IsValid<ISpxConversationSessionEventArgs>(hevt))
    {
        return _Handle_Close<ISpxConversationSessionEventArgs>(hevt);
    }
    else if (_Handle_IsValid<ISpxConversationConnectionEventArgs>(hevt))
    {
        return _Handle_Close<ISpxConversationConnectionEventArgs>(hevt);
    }
    else if (_Handle_IsValid<ISpxConversationExpirationEventArgs>(hevt))
    {
        return _Handle_Close<ISpxConversationExpirationEventArgs>(hevt);
    }
    else if (_Handle_IsValid<ISpxConversationParticipantChangedEventArgs>(hevt))
    {
        return _Handle_Close<ISpxConversationParticipantChangedEventArgs>(hevt);
    }
    else if (_Handle_IsValid<ISpxConversationTranslationEventArgs>(hevt))
    {
        return _Handle_Close<ISpxConversationTranslationEventArgs>(hevt);
    }
    else if (_Handle_IsValid<ISpxConversationTranslationResult>(hevt))
    {
        return _Handle_Close<ISpxConversationTranslationResult>(hevt);
    }
    else
    {
        return SPXERR_INVALID_HANDLE;
    }
}

SPXAPI_(bool) conversation_translator_event_handle_is_valid(SPXCONVERSATIONTRANSLATORHANDLE hevt)
{
    if (hevt == SPXHANDLE_INVALID)
    {
        return false;
    }

    return _Handle_IsValid<ISpxConversationSessionEventArgs>(hevt)
        || _Handle_IsValid<ISpxConversationConnectionEventArgs>(hevt)
        || _Handle_IsValid<ISpxConversationExpirationEventArgs>(hevt)
        || _Handle_IsValid<ISpxConversationParticipantChangedEventArgs>(hevt)
        || _Handle_IsValid<ISpxConversationTranslationEventArgs>(hevt)
        || _Handle_IsValid<ISpxConversationTranslationResult>(hevt)
        ;
}

SPXAPI conversation_translator_event_get_expiration_time(SPXEVENTHANDLE hevent, int32_t * pexpirationminutes)
{
    return RetrieveValue(hevent, pexpirationminutes, &ISpxConversationExpirationEventArgs::GetMinutesLeft);
}

SPXAPI conversation_translator_event_get_participant_changed_reason(SPXEVENTHANDLE hevent, Transcription::ParticipantChangedReason * preason)
{
    return RetrieveValue(hevent, preason, &ISpxConversationParticipantChangedEventArgs::GetReason);
}

SPXAPI conversation_translator_event_get_participant_changed_at_index(SPXEVENTHANDLE hevent, int index, SPXPARTICIPANTHANDLE * phparticipant)
{
    SPX_RETURN_HR_IF(SPXERR_INVALID_ARG, phparticipant == nullptr);
    *phparticipant = SPXHANDLE_INVALID;

    SPXAPI_INIT_HR_TRY(hr)
    {
        auto evt = GetInstance<ISpxConversationParticipantChangedEventArgs>(hevent);
        const std::vector<std::shared_ptr<ISpxConversationParticipant>> participants = evt->GetParticipants();
        if (index < 0 || static_cast<size_t>(index) >= participants.size())
        {
            return SPX_NOERROR;
        }

        auto participantHandles = CSpxSharedPtrHandleTableManager::Get<ISpxParticipant, SPXPARTICIPANTHANDLE>();
        *phparticipant = participantHandles->TrackHandle(participants[index]);

        return SPX_NOERROR;
    }
    SPXAPI_CATCH_AND_RETURN_HR(hr);
}

SPXAPI conversation_translator_result_get_original_lang(SPXRESULTHANDLE hresult, char * psz, uint32_t * pcch)
{
    return RetrieveStringValue(hresult, psz, pcch, &ISpxConversationTranslationResult::GetOriginalLanguage);
}

SPXAPI conversation_translator_participant_get_avatar(SPXEVENTHANDLE hevent, char * psz, uint32_t * pcch)
{
    return TryRetrieveStringValue(hevent, psz, pcch, &ISpxConversationParticipant::GetAvatar);
}

SPXAPI conversation_translator_participant_get_displayname(SPXEVENTHANDLE hevent, char * psz, uint32_t * pcch)
{
    return TryRetrieveStringValue(hevent, psz, pcch, &ISpxConversationParticipant::GetDisplayName);
}

SPXAPI conversation_translator_participant_get_id(SPXEVENTHANDLE hevent, char * psz, uint32_t * pcch)
{
    return RetrieveStringValue(hevent, psz, pcch, &ISpxParticipant::GetId);
}

SPXAPI conversation_translator_participant_get_is_muted(SPXEVENTHANDLE hevent, bool * pMuted)
{
    return TryRetrieveValue(hevent, pMuted, &ISpxConversationParticipant::IsMuted);
}

SPXAPI conversation_translator_participant_get_is_host(SPXEVENTHANDLE hevent, bool * pIsHost)
{
    return TryRetrieveValue(hevent, pIsHost, &ISpxConversationParticipant::IsHost);
}

SPXAPI conversation_translator_participant_get_is_using_tts(SPXEVENTHANDLE hevent, bool * ptts)
{
    return TryRetrieveValue(hevent, ptts, &ISpxConversationParticipant::IsUsingTts);
}
