//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_translator.cpp: implementation declarations for conversation translator
//

#include "stdafx.h"
#include <spxdebug.h>
#include <http_utils.h>
#include <site_helpers.h>
#include <thread_service.h>
#include <guid_utils.h>
#include "conversation_manager.h"
#include "translation_recognizer.h"
#include "conversation_translator.h"
#include "conversation_utils.h"
#include "conversation_events.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {
    extern void PlatformInit(const char* proxyHost, int proxyPort, const char* proxyUsername, const char* proxyPassword);
}}}} // Microsoft::CognitiveServices::Speech::USP

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

// The Android GCC compiler expects a void * argument when the %p format specifier is used
#if defined(ANDROID) || defined(__ANDROID__)
    #define P_FORMAT_POINTER(T) ((void *)T)
#else
    #define P_FORMAT_POINTER(T) T
#endif

    using namespace std;
    using StringUtils = PAL::StringUtils;

    /// <summary>
    /// Creates an object with the specified site. This will implicitly call the Init() method on that
    /// object if it implements ISpxObjectInit
    /// </summary>
    template <class I>
    static inline shared_ptr<I> CreateAndInitObjectWithSite(const char* className, shared_ptr<ISpxRecognizerSite> site)
    {
        // get the base site if available
        auto baseSite = SpxQueryInterface<ISpxGenericSite>(site);
        SPX_IFTRUE_THROW_HR(baseSite == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        auto instance = SpxCreateObjectWithSite<I>(className, baseSite);
        return instance;
    }

    template <typename TClass, typename ...TArgs>
    static inline shared_ptr<TClass> CreateShared(TArgs&&... args)
    {
        shared_ptr<ISpxInterfaceBase> ptr(static_cast<ISpxInterfaceBase*>(new TClass(std::forward<TArgs>(args)...)));
        return dynamic_pointer_cast<TClass>(ptr);
    }

    template <typename I>
    static inline shared_ptr<I> As(shared_ptr<ISpxTranslationRecognizer> reco)
    {
        SPX_IFTRUE_THROW_HR(reco == nullptr, SPXERR_UNINITIALIZED);
        auto cast = reco->QueryInterface<I>();
        SPX_IFTRUE_THROW_HR(cast == nullptr, SPXERR_RUNTIME_ERROR);
        return cast;
    }

    static inline shared_ptr<ISpxNamedProperties> AsNamedProperties(shared_ptr<ISpxTranslationRecognizer> reco)
    {
        return As<ISpxNamedProperties>(reco);
    }

    /// <summary>
    /// Helper method to create event arguments
    /// </summary>
    /// <typeparam name="C">The concrete type that implements the interface</typeparam>
    /// <typeparam name="I">The interface</typeparam>
    /// <typeparam name="...Args">The concrete type constructor arguments</typeparam>
    /// <returns>A shared_ptr to the instance type</returns>
    template<typename C, typename I, typename ...Args>
    static inline shared_ptr<I> CreateEventArgs(Args... args)
    {
        auto instance = CreateShared<C>(args...);
        return dynamic_pointer_cast<I>(instance);
    }

    /// <summary>
    /// Helper method to raise events and catch exceptions
    /// </summary>
    /// <typeparam name="I">The interface type</typeparam>
    template<typename I>
    static inline void RaiseEvent(EventSignal<shared_ptr<I>>& event, shared_ptr<I> eventArgs)
    {
        try
        {
            event.Signal(eventArgs);
        }
        catch (exception& ex)
        {
            SPX_TRACE_ERROR("Exception while raising event. EventArgs: %p, What: %s", P_FORMAT_POINTER(eventArgs.get()), ex.what());
        }
        catch (...)
        {
            SPX_TRACE_ERROR("Unknown throwable while raising event. EventArgs: %p", P_FORMAT_POINTER(eventArgs.get()));
        }
    }

    /// <summary>
    /// Helper method to raise events and catch exceptions
    /// </summary>
    /// <typeparam name="C">The concrete type that implements the interface</typeparam>
    /// <typeparam name="I">The interface type</typeparam>
    template<typename C, typename I>
    static inline void RaiseEvent(EventSignal<shared_ptr<I>>& event, shared_ptr<C> eventArgs)
    {
        RaiseEvent<I>(event, dynamic_pointer_cast<I>(eventArgs));
    }

    /// <summary>
    /// Helper method to raise events. This will create a new shared_ptr instance of the concrete
    /// type the arguments passed in. It will then raise the event and catch exceptions
    /// </summary>
    /// <typeparam name="C">The concrete type that implements the interface</typeparam>
    /// <typeparam name="I">The interface type</typeparam>
    /// <typeparam name="...Args">The concrete type constructor arguments</typeparam>
    template<typename C, typename I, typename ...Args>
    static inline void RaiseEvent(EventSignal<shared_ptr<I>>& event, Args... args)
    {
        RaiseEvent<I>(event, CreateEventArgs<C, I, Args...>(args...));
    }

    static CancellationErrorCode ToCancellationErrorCode(bool isWebSocket, ConversationErrorCode error)
    {
        if (!isWebSocket)
        {
            return CancellationErrorCode::RuntimeError;
        }

        switch (error)
        {
            case ConversationErrorCode::AuthenticationError:
                return CancellationErrorCode::AuthenticationFailure;
            case ConversationErrorCode::BadRequest:
                return CancellationErrorCode::BadRequest;
            case ConversationErrorCode::ConnectionError:
                return CancellationErrorCode::ConnectionFailure;
            case ConversationErrorCode::Forbidden:
                return CancellationErrorCode::Forbidden;
            case ConversationErrorCode::RuntimeError:
                return CancellationErrorCode::RuntimeError;
            case ConversationErrorCode::ServiceError:
                return CancellationErrorCode::ServiceError;
            case ConversationErrorCode::ServiceUnavailable:
                return CancellationErrorCode::ServiceUnavailable;
            case ConversationErrorCode::TooManyRequests:
                return CancellationErrorCode::TooManyRequests;
            default:
                return CancellationErrorCode::RuntimeError;
        }
    }

    static CancellationErrorCode ToCancellationErrorCode(const USP::WebSocketDisconnectReason reason)
    {
        switch (reason)
        {
            default:
            case USP::WebSocketDisconnectReason::Unknown:
                return CancellationErrorCode::RuntimeError;

            case USP::WebSocketDisconnectReason::Normal:
                return CancellationErrorCode::NoError;

            case USP::WebSocketDisconnectReason::EndpointUnavailable:
                return CancellationErrorCode::ServiceUnavailable;

            case USP::WebSocketDisconnectReason::ProtocolError:
            case USP::WebSocketDisconnectReason::CannotAcceptDataType:
            case USP::WebSocketDisconnectReason::InvalidPayloadData:
            case USP::WebSocketDisconnectReason::PolicyViolation:
            case USP::WebSocketDisconnectReason::MessageTooBig:
            case USP::WebSocketDisconnectReason::UnexpectedCondition:
                return CancellationErrorCode::BadRequest;

            case USP::WebSocketDisconnectReason::InternalServerError: return CancellationErrorCode::ServiceError;
        }
    }

    static ParticipantChangedReason ToReason(ConversationParticipantAction action)
    {
        switch (action)
        {
            default:
            case ConversationParticipantAction::Join: return ParticipantChangedReason::JoinedConversation;
            case ConversationParticipantAction::Leave: return ParticipantChangedReason::LeftConversation;
            case ConversationParticipantAction::Update: return ParticipantChangedReason::Updated;
        }
    }

    /// <summary>
    /// Register an instance method as an event callback. This uses a weak_ptr to ensure we don't
    /// try to invoke a callback on a disposed instance.
    /// </summary>
    template<typename I>
    static void AddHandler(EventSignal<shared_ptr<I>>& event, CSpxConversationTranslator* instance, void (CSpxConversationTranslator:: *callback)(shared_ptr<I>))
    {
        weak_ptr<ISpxConversationTranslator> weak(instance->ISpxConversationTranslator::shared_from_this());
        event.Connect([weak, callback](shared_ptr<I> eventArgs)
        {
            auto ptr = dynamic_pointer_cast<CSpxConversationTranslator>(weak.lock());
            if (ptr != nullptr)
            {
                (ptr.get()->*callback)(eventArgs);
            }
        });
    }

    CSpxConversationTranslator::CSpxConversationTranslator() :
        m_state_(ConversationState::Closed),
        _m_conv(),
        m_convInternals(),
        m_endConversationOnLeave(false),
        m_recognizer(),
        m_recognizerConnected(false),
        m_speechLang(),
        m_audioInput(),
        m_utteranceId()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        // we should create the TranslatorRecognizer instance here but NOT call Init()
        // on it, nor set its site
        auto recognizer = SpxCreateObject<ISpxRecognizer>("CSpxTranslationRecognizer", SpxGetRootSite());
        SPX_IFTRUE_THROW_HR(recognizer == nullptr, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
        m_recognizer = dynamic_pointer_cast<ISpxTranslationRecognizer>(recognizer);
        SPX_IFTRUE_THROW_HR(m_recognizer == nullptr, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
    }

    CSpxConversationTranslator::~CSpxConversationTranslator()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        Term();
    }

    void CSpxConversationTranslator::Init()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        ThreadingHelpers::Init();

        shared_ptr<ISpxRecognizerSite> site = GetSite();
        auto genericSite = dynamic_pointer_cast<ISpxGenericSite>(site);
        SPX_IFTRUE_THROW_HR(genericSite == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        auto factory = SpxQueryService<ISpxObjectFactory>(site);
        SPX_IFTRUE_THROW_HR(factory == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        auto properties = SpxQueryService<ISpxNamedProperties>(site);
        SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        // Initialize Azure C shared library HTTP platform
        USP::PlatformInit(nullptr, 0, nullptr, nullptr);

        m_speechLang = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), "");
        SPX_IFTRUE_THROW_HR(m_speechLang.empty(), SPXERR_INVALID_ARG);

        // Initialise the speech translator recognizer instance by setting the site.
        auto withSite = As<ISpxObjectWithSite>(m_recognizer);
        withSite->SetSite(genericSite);

        ConnectTranslationRecognizer(m_recognizer);
    }

    void CSpxConversationTranslator::Term()
    {
        m_recognizerConnected = false;
        m_recognizer = nullptr;
        DisconnectTranslationRecognizer();
        DisconnectConversation();
        m_audioInput.reset();
    }

    const vector<shared_ptr<ISpxConversationParticipant>> CSpxConversationTranslator::GetParticipants()
    {
        return RunSynchronously<vector<shared_ptr<ISpxConversationParticipant>>>([this]()
        {
            vector<shared_ptr<ISpxConversationParticipant>> participants;

            auto convInternals = m_convInternals.lock();
            if (convInternals != nullptr)
            {
                auto conn = convInternals->GetConversationConnection();
                if (conn != nullptr)
                {
                    for (auto p : conn->GetParticipants())
                    {
                        auto part = CreateShared<CSpxConversationParticipant>(move(p));
                        participants.push_back(part);
                    }
                }
            }

            return participants;
        });
    }

    void CSpxConversationTranslator::JoinConversation(shared_ptr<ISpxConversation> conversation, const string & nickname, bool endConversationOnLeave)
    {
        SPX_DBG_TRACE_FUNCTION();

        SPX_IFTRUE_THROW_HR(GetSite() == nullptr || m_threadService == nullptr, SPXERR_UNINITIALIZED);
        SPX_IFTRUE_THROW_HR(conversation == nullptr, SPXERR_INVALID_ARG);

        RunSynchronously([this, conversation, nickname, endConversationOnLeave]()
        {
            switch (GetState())
            {
                case ConversationState::Closed:
                    break;

                default:
                case ConversationState::Closing:
                case ConversationState::Failed:
                case ConversationState::Open:
                case ConversationState::Opening:
                case ConversationState::PartiallyOpen:
                    SPX_THROW_HR(SPXERR_INVALID_STATE);
                    break;
            }

            auto impl = SafeQueryInterface<ISpxConversationWithImpl>(conversation);
            SPX_IFTRUE_THROW_HR(impl == nullptr, SPXERR_UNEXPECTED_CONVERSATION_SITE_FAILURE);

            ConnectConversation(conversation, endConversationOnLeave);

            // since we now have the nickname to use, let's change it here
            {
                auto internals = m_convInternals.lock();
                if (internals)
                {
                    internals->GetConversationConnection()->SetNickname(nickname);
                }
            }

            RaiseEvent<ConversationSessionEventArgs>(SessionStarted, GetSessionId());
            RaiseEvent<ConversationConnectionEventArgs>(Connected, GetSessionId());

            auto eventArgs = CreateShared<ConversationParticipantChangedEventArgs>(
                GetSessionId(), ParticipantChangedReason::JoinedConversation);

            auto conv = m_convInternals.lock();
            SPX_IFTRUE_THROW_HR(conv == nullptr, SPXERR_UNINITIALIZED);

            for (const auto& p : conv->GetConversationConnection()->GetParticipants())
            {
                auto part = CreateShared<CSpxConversationParticipant>(p);
                eventArgs->AddParticipant(part);
            }

            RaiseEvent(ParticipantsChanged, eventArgs);
        });
    }

    void CSpxConversationTranslator::StartTranscribing()
    {
        RunSynchronously([this]()
        {
            switch (GetState())
            {
                case ConversationState::Open:
                case ConversationState::PartiallyOpen:
                    break;

                default:
                case ConversationState::Closed:
                case ConversationState::Closing:
                case ConversationState::Failed:
                case ConversationState::Opening:
                    SPX_THROW_HR(SPXERR_INVALID_STATE);
                    break;
            }

            As<ISpxRecognizer>(m_recognizer)->StartContinuousRecognitionAsync().Future.get();
        });
    }

    void CSpxConversationTranslator::StopTranscribing()
    {
        RunSynchronously([this]()
        {
            switch (GetState())
            {
                case ConversationState::Open:
                case ConversationState::PartiallyOpen:
                    break;

                default:
                case ConversationState::Closed:
                case ConversationState::Closing:
                case ConversationState::Failed:
                case ConversationState::Opening:
                    SPX_THROW_HR(SPXERR_INVALID_STATE);
                    break;
            }

            As<ISpxRecognizer>(m_recognizer)->StopContinuousRecognitionAsync().Future.get();
        });
    }

    void CSpxConversationTranslator::SendTextMsg(const string& message)
    {
        RunSynchronously([this, message]()
        {
            switch (GetState())
            {
                case ConversationState::Open:
                    break;

                case ConversationState::PartiallyOpen:
                    // check if the conversation connection is open
                    if (!IsConversationConnected())
                    {
                        SPX_THROW_HR(SPXERR_INVALID_STATE);
                    }
                    break;

                default:
                case ConversationState::Closed:
                case ConversationState::Closing:
                case ConversationState::Failed:
                case ConversationState::Opening:
                    SPX_THROW_HR(SPXERR_INVALID_STATE);
                    break;
            }

            auto convInternals = m_convInternals.lock();
            if (convInternals != nullptr)
            {
                convInternals->GetConversationConnection()->SendTextMessage(message);
            }
        });
    }

    void CSpxConversationTranslator::LeaveConversation()
    {
        if (m_threadService == nullptr)
        {
            // we are not initialized and should not throw exceptions here
            return;
        }

        std::promise<bool> promise;
        std::future<bool> future = promise.get_future();

        RunAsynchronously([this]()
        {
            auto state = GetState();
            SPX_TRACE_INFO("Called leave conversation on 0x%p in %d state", P_FORMAT_POINTER(this), state);

            switch (GetState())
            {
                default:
                    SPX_TRACE_ERROR("Unknown state on 0x%p while leaving conversation: %d", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Failed:
                case ConversationState::Closing:
                case ConversationState::Closed:
                    // nothing to do here
                    return;

                case ConversationState::Open:
                case ConversationState::PartiallyOpen:
                case ConversationState::Opening:
                    // continue
                    break;
            }

            SetState(ConversationState::Closing);
            RaiseEvent<ConversationConnectionEventArgs>(Disconnected, GetSessionId());

            // stop the transcriber, disconnect the conversation
            As<ISpxRecognizer>(m_recognizer)->StopContinuousRecognitionAsync().Future.get();
            DisconnectTranslationRecognizer();

            if (m_endConversationOnLeave && _m_conv != nullptr)
            {
                _m_conv->EndConversation();
            }

            // we want to get off the background thread now to give the event handlers a
            // chance to run. Those will set the state to closed
        }, std::move(promise));

        bool success = future.get();
        SPX_IFTRUE_THROW_HR(false == success, SPXERR_CANCELED);

        RunSynchronously([this]() { DisconnectConversation(); });
    }

    string CSpxConversationTranslator::GetStringValue(const char* name, const char* defaultValue) const
    {
        return AsNamedProperties(m_recognizer)->GetStringValue(name, defaultValue);
    }

    void CSpxConversationTranslator::SetStringValue(const char * name, const char * value)
    {
        AsNamedProperties(m_recognizer)->SetStringValue(name, value);
    }

    bool CSpxConversationTranslator::HasStringValue(const char * name) const
    {
        return AsNamedProperties(m_recognizer)->HasStringValue(name);
    }

    void CSpxConversationTranslator::Copy(ISpxNamedProperties * from)
    {
        AsNamedProperties(m_recognizer)->Copy(from);
    }

    shared_ptr<ISpxSession> CSpxConversationTranslator::GetDefaultSession()
    {
        return As<ISpxSessionFromRecognizer>(m_recognizer)->GetDefaultSession();
    }

    shared_ptr<ISpxConnection> CSpxConversationTranslator::GetConnection()
    {
        return As<ISpxConnectionFromRecognizer>(m_recognizer)->GetConnection();
    }

    void CSpxConversationTranslator::SetAudioConfig(std::weak_ptr<ISpxAudioConfig> audio_config)
    {
        m_audioInput = audio_config;
    }

    std::shared_ptr<ISpxAudioConfig> CSpxConversationTranslator::GetAudioConfig()
    {
        return m_audioInput.lock();
    }

    void CSpxConversationTranslator::OnConnected()
    {
        RunAsynchronously([this]()
        {
            bool sendSessionStarted = false;
            bool sendConnected = false;

            auto state = GetState();
            switch (state)
            {
                default:
                    SPX_TRACE_ERROR("Unknown state on 0x%p for connected conversation event: %d", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Failed:
                case ConversationState::Closed:
                case ConversationState::Closing:
                    // Should theoretically never get here
                    SPX_TRACE_WARNING("Unexpected state on 0x%p when for connected conversation event: %d", P_FORMAT_POINTER(this), state);
                    break;

                case ConversationState::Opening:
                    sendSessionStarted = true;
                    sendConnected = true;
                    ChangeState(state, m_recognizerConnected ? ConversationState::Open : ConversationState::PartiallyOpen);
                    break;

                case ConversationState::PartiallyOpen:
                    if (m_recognizerConnected)
                    {
                        sendConnected = true;
                        ChangeState(state, ConversationState::Open);
                    }
                    break;

                case ConversationState::Open:
                    // In theory, this shouldn't happen so we log that we may have a logic bug and move on
                    SPX_TRACE_WARNING("Got connected conversation event on 0x%p when state is open. Possible logic bug.", P_FORMAT_POINTER(this));
                    break;
            }

            if (sendSessionStarted)
            {
                RaiseEvent<ConversationSessionEventArgs>(SessionStarted, GetSessionId());
            }

            if (sendConnected)
            {
                RaiseEvent<ConversationConnectionEventArgs>(Connected, GetSessionId());
            }
        });
    }

    void CSpxConversationTranslator::OnDisconnected(const USP::WebSocketDisconnectReason reason, const string & message)
    {
        RunAsynchronously([this, reason, message]()
        {
            auto state = GetState();
            SPX_TRACE_INFO("Disconnected conversation event on 0x%p in state %d. Reason: %d, Message: '%s'", P_FORMAT_POINTER(this), state, reason, message.c_str());

            bool sendDisconnected = false;
            bool sendStopped = false;

            CancellationErrorCode errorCode = ToCancellationErrorCode(reason);
            
            switch (state)
            {
                default:
                    SPX_TRACE_ERROR("Unknown state on 0x%p for connected conversation event: %d", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Failed:
                    // Ignore. This can happen if the failure was caused by the translator
                    // recognizer. In that case, we will tear down the conversation session
                    // recognizer
                    break;

                case ConversationState::Closed:
                    // should never get here
                    SPX_TRACE_WARNING("Got disconnected conversation event on 0x%p when state is already closed. Possible logic bug.", P_FORMAT_POINTER(this));
                    break;

                case ConversationState::Closing:
                    // If the recognizer is also not connected, we should transition to the
                    // closed state. Otherwise we wait for that trigger the closed state
                    if (!m_recognizerConnected)
                    {
                        sendStopped = true;
                        ChangeState(state, ConversationState::Closed);

                        DisconnectConversation();
                    }
                    break;

                case ConversationState::Opening:
                    // Something went wrong and the conversation connection closed before it
                    // became open. Handle as an error
                    ToFailedState(false, errorCode, message);
                    break;

                case ConversationState::PartiallyOpen:
                case ConversationState::Open:
                    // if the server requested the close, treat this as the client closing
                    // the connection
                    if (reason == USP::WebSocketDisconnectReason::Normal)
                    {
                        sendDisconnected = true;

                        if (state == ConversationState::PartiallyOpen || !m_recognizerConnected)
                        {
                            sendStopped = true;
                            ChangeState(state, ConversationState::Closed);
                        }
                        else
                        {
                            ChangeState(state, ConversationState::Closing);
                        }

                        DisconnectTranslationRecognizer();
                    }
                    else
                    {
                        ToFailedState(false, errorCode, message);
                    }
                    break;
            }

            if (sendDisconnected)
            {
                RaiseEvent<ConversationConnectionEventArgs>(Disconnected, GetSessionId());
            }

            if (sendStopped)
            {
                RaiseEvent<ConversationSessionEventArgs>(SessionStopped, GetSessionId());
            }
        });
    }

    void CSpxConversationTranslator::OnSpeechRecognition(const ConversationSpeechRecognitionMessage & reco)
    {
        RunAsynchronously([this, reco]()
        {
            if (!IsConsideredOpen())
            {
                SPX_TRACE_WARNING("Got speech recognition event on 0x%p when state is not considered open.", P_FORMAT_POINTER(this));
                return;
            }

            if (StringUtils::ToUpper(reco.ParticipantId) == StringUtils::ToUpper(GetParticipantId()))
            {
                // This is your own speech recognition. For best latency, we are sending the results
                // from the translation recognizer directly so we don't send it here to avoid duplicate
                // events
                return;
            }

            bool isFinal = reco.Type == MessageType::Final;

            ResultReason reason = isFinal
                ? ResultReason::TranslatedParticipantSpeech
                : ResultReason::TranslatingParticipantSpeech;

            // TODO ralphe: how to compute the offset and duration for other people's recognitions?
            auto result = CreateShared<ConversationRecognitionResult>(
                StringUtils::ToUpper(reco.Id),
                reco.Recognition,
                reco.OriginalLanguage,
                reason,
                reco.ParticipantId
            );

            for (const auto& entry : reco.Translations)
            {
                result->AddTranslation(entry.first, entry.second);
            }

            RaiseEvent<ConversationTranslationEventArgs>(isFinal ? Transcribed : Transcribing, GetSessionId(), result);
        });
    }

    void CSpxConversationTranslator::OnInstantMessage(const ConversationTranslatedMessage & im)
    {
        RunAsynchronously([this, im]()
        {
            if (!IsConsideredOpen())
            {
                SPX_TRACE_WARNING("Got an instant message event on 0x%p when state is not considered open.", P_FORMAT_POINTER(this));
                return;
            }

            ResultReason reason = StringUtils::ToUpper(im.ParticipantId) == GetParticipantId()
                ? ResultReason::TranslatedInstantMessage
                : ResultReason::TranslatedParticipantInstantMessage;

            // TODO ralphe: Can you determine the offset and duration for IMs?
            auto result = CreateShared<ConversationRecognitionResult>(
                im.Id,
                im.Text,
                im.OriginalLanguage,
                reason,
                im.ParticipantId
            );

            for (const auto& entry : im.Translations)
            {
                result->AddTranslation(entry.first, entry.second);
            }

            RaiseEvent<ConversationTranslationEventArgs>(TextMessageReceived, GetSessionId(), result);
        });
    }

    void CSpxConversationTranslator::OnParticipantChanged(const ConversationParticipantAction action, const vector<ConversationParticipant>& participants)
    {
        RunAsynchronously([this, action, participants]()
        {
            if (!IsConsideredOpen())
            {
                SPX_TRACE_WARNING("Got a participant changed event on 0x%p when state is not considered open.", P_FORMAT_POINTER(this));
                return;
            }

            auto eventArgs = CreateShared<ConversationParticipantChangedEventArgs>(GetSessionId(), ToReason(action));
            for (auto& p : participants)
            {
                auto part = CreateShared<CSpxConversationParticipant>(move(p));
                eventArgs->AddParticipant(part);
            }

            RaiseEvent(ParticipantsChanged, eventArgs);
        });
    }

    void CSpxConversationTranslator::OnRoomExpirationWarning(const int32_t minutesLeft)
    {
        RunAsynchronously([this, minutesLeft]()
        {
            if (!IsConsideredOpen())
            {
                SPX_TRACE_WARNING("Got a room expiration event on 0x%p when state is not considered open.", P_FORMAT_POINTER(this));
                return;
            }

            RaiseEvent<ConversationExpirationEventArgs>(ConversationExpiration, GetSessionId(), minutesLeft);
        });
    }

    void CSpxConversationTranslator::OnError(const bool isWebSocket, const ConversationErrorCode error, const string & message)
    {
        RunAsynchronously([this, isWebSocket, error, message]()
        {
            SPX_TRACE_ERROR("Got an error event on 0x%p. Error: %d-%d, Message: '%s'", P_FORMAT_POINTER(this), isWebSocket, error, message.c_str());
            ToFailedState(false, ToCancellationErrorCode(isWebSocket, error), message);
        });
    }

    void CSpxConversationTranslator::ConnectConversation(shared_ptr<ISpxConversation> conversation, bool endConversationOnLeave)
    {
        std::shared_ptr<ISpxConversation> internalConv = conversation;

        // check if this conversation has an internal implementation and extract it if so
        auto withImpl = conversation->QueryInterface<ISpxConversationWithImpl>();
        if (withImpl != nullptr)
        {
            internalConv = withImpl->GetConversationImpl();
        }

        auto internals = SafeQueryInterface<ISpxConversationInternals>(internalConv);
        SPX_IFTRUE_THROW_HR(internals == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(!internals->IsConnected(), SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(internals->GetConversationArgs() == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(internals->GetConversationConnection() == nullptr, SPXERR_INVALID_ARG);
        SPX_IFTRUE_THROW_HR(internals->GetConversationManager() == nullptr, SPXERR_INVALID_ARG);

        SPX_IFTRUE_THROW_HR(_m_conv != nullptr, SPXERR_ALREADY_INITIALIZED);

        // NOTE:
        // Since the instance of ISpxConversationInternals is contained in the parent
        // ISpxConveration instance, we shouldn't take ownership of it nor extend its
        // lifetime here. Doing so can lead to problems as the parent instance going
        // out of scope will terminate the child ISpxConversationInternals instance
        // (it calls Term() on it). We would then be left holding onto a zombie.
        _m_conv = conversation; // shared_ptr
        m_convInternals = internals; // weak_ptr
        m_endConversationOnLeave = endConversationOnLeave;

        // Set the conversation token to be sent to the speech service. This enables the
        // service to service call from the speech service to the Capito service for relaying
        // recognitions.
        SetStringValue(ConversationKeys::Conversation_Token, internals->GetConversationArgs()->SessionToken.c_str());
        SetStringValue(PropertyId::SpeechServiceAuthorization_Token, internals->GetConversationArgs()->CognitiveSpeechAuthToken.c_str());

        try
        {
            // The recognizer code is very finicky about not allowing you to set values that are already
            // "set" at any level in the hierarchy *even if the previous values are empty*. Fine, let's
            // be sneaky and bypass the translation recognizer and set the property directly on the site
            auto properties = SafeQueryInterface<ISpxNamedProperties>(GetSite());
            if (properties != nullptr)
            {
                // This needs to be set otherwise the USP socket code won't try to connect. If
                // you are using a custom endpoint URL, you don't need to set the region
                if (GetStringValue(PropertyId::SpeechServiceConnection_Region).empty()
                    && GetStringValue(PropertyId::SpeechServiceConnection_Endpoint).empty())
                {
                    properties->SetStringValue(
                        GetPropertyName(PropertyId::SpeechServiceConnection_Region),
                        internals->GetConversationArgs()->CognitiveSpeechRegion.c_str());
                }

                // if you don't set a translate to language, the speech web socket connection will return
                // an HTTP 400 error
                if (GetStringValue(PropertyId::SpeechServiceConnection_TranslationToLanguages).empty())
                {
                    properties->SetStringValue(
                        GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages),
                        m_speechLang.c_str());
                }
            }
        }
        catch (...) { /* don't care, it means the region is already set */ }

        internals->GetConversationConnection()->SetCallbacks(
            dynamic_pointer_cast<ConversationCallbacks>(ISpxInterfaceBase::shared_from_this()));

        ChangeState(ConversationState::Closed, ConversationState::PartiallyOpen);
    }

    void CSpxConversationTranslator::DisconnectConversation()
    {
        if (_m_conv == nullptr)
        {
            return;
        }

        auto convInternals = m_convInternals.lock();
        if (convInternals == nullptr)
        {
            return;
        }

        if (convInternals->IsConnected())
        {
            convInternals->GetConversationConnection()->Disconnect();

            // we will call this method again when we transition from the closing
            // to closed state to clean up
            return;
        }

        // remove callbacks
        convInternals->GetConversationConnection()->SetCallbacks(nullptr);

        // release our references in the right order
        convInternals = nullptr;
        _m_conv = nullptr;
    }

    void CSpxConversationTranslator::ConnectTranslationRecognizer(std::shared_ptr<ISpxTranslationRecognizer> recognizer)
    {
        auto events = recognizer->QueryInterface<ISpxRecognizerEvents>();
        SPX_IFTRUE_THROW_HR(events == nullptr, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);

        auto baseRecognizer = recognizer->QueryInterface<ISpxRecognizer>();
        SPX_IFTRUE_THROW_HR(baseRecognizer == nullptr, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);

        AddHandler(events->SessionStarted, this, &CSpxConversationTranslator::OnRecognizerSessionStarted);
        AddHandler(events->SessionStopped, this, &CSpxConversationTranslator::OnRecognizerSessionStopped);
        AddHandler(events->Connected, this, &CSpxConversationTranslator::OnRecognizerConnected);
        AddHandler(events->Disconnected, this, &CSpxConversationTranslator::OnRecognizerDisconnected);
        AddHandler(events->IntermediateResult, this, &CSpxConversationTranslator::OnRecognizerResult);
        AddHandler(events->FinalResult, this, &CSpxConversationTranslator::OnRecognizerResult);
        AddHandler(events->Canceled, this, &CSpxConversationTranslator::OnRecognizerCanceled);

        auto session = GetDefaultSession();
        if (session != nullptr)
        {
            session->AddRecognizer(baseRecognizer);
        }
        else
        {
            SPX_TRACE_WARNING("Unexpected null default session on 0x%p", P_FORMAT_POINTER(this));
        }
    }

    void CSpxConversationTranslator::DisconnectTranslationRecognizer()
    {
        if (m_recognizer == nullptr)
        {
            return;
        }

        auto events = m_recognizer->QueryInterface<ISpxRecognizerEvents>();
        if (events == nullptr)
        {
            return;
        }

        events->SessionStarted.DisconnectAll();
        events->Connected.DisconnectAll();
        events->Disconnected.DisconnectAll();
        events->IntermediateResult.DisconnectAll();
        events->FinalResult.DisconnectAll();
        events->Canceled.DisconnectAll();

        // if we're already disconnected let's clear the SessionStopped event handler
        // otherwise the session stopped callback will do this
        if (false == m_recognizerConnected.load())
        {
            events->SessionStopped.DisconnectAll();
            m_recognizer = nullptr;
        }
        else
        {
            auto reco = As<ISpxRecognizer>(m_recognizer);
            reco->StopContinuousRecognitionAsync().Future.get();
            reco->CloseConnection();
        }
    }

    inline bool CSpxConversationTranslator::IsConsideredOpen() const
    {
        SPX_DBG_TRACE_FUNCTION();
        auto state = GetState();
        switch (GetState())
        {
            default:
            case ConversationState::Failed:
            case ConversationState::Closed:
            case ConversationState::Closing:
                SPX_DBG_TRACE_INFO("State is considered closed on 0x%p (%d)", P_FORMAT_POINTER(this), state);
                return false;

            case ConversationState::PartiallyOpen:
            case ConversationState::Open:
            case ConversationState::Opening:
                return true;
        }
    }

    inline const string CSpxConversationTranslator::GetSessionId() const
    {
        auto convInternals = m_convInternals.lock();
        if (convInternals != nullptr)
        {
            auto args = convInternals->GetConversationArgs();
            if (args != nullptr)
            {
                return args->CorrelationId;
            }
        }

        return string();
    }

    inline const string CSpxConversationTranslator::GetParticipantId() const
    {
        auto convInternals = m_convInternals.lock();
        if (convInternals != nullptr)
        {
            auto args = convInternals->GetConversationArgs();
            if (args != nullptr)
            {
                return StringUtils::ToUpper(args->ParticipantId);
            }
        }

        return string();
    }

    inline bool CSpxConversationTranslator::IsConversationConnected() const
    {
        auto convInternals = m_convInternals.lock();
        return convInternals != nullptr && convInternals->IsConnected();
    }

    void CSpxConversationTranslator::ToFailedState(bool isRecognizer, CancellationErrorCode error, const std::string& message)
    {
        // method should be called in background thread
        SPX_TRACE_ERROR("About to go to failed state on 0x%p. ErrorCode: %d, Message: '%s'", P_FORMAT_POINTER(this), error, message.c_str());

        bool sendDisconnected = false;

        auto state = GetState();
        switch (state)
        {
            default:
                SPX_TRACE_ERROR("Unknown state on 0x%p for when attempting to move to failed state: %d", P_FORMAT_POINTER(this), state);
                return;
            
            case ConversationState::Failed:
            case ConversationState::Closed:
                // should theoretically not get here
                SPX_TRACE_WARNING("Attempting to go to failed state on 0x%p when state is already closed/failed (%d). Possible logic bug.", P_FORMAT_POINTER(this), state);
                return;

            case ConversationState::Opening:
            case ConversationState::Closing:
                // nothing special to do here
                break;

            case ConversationState::PartiallyOpen:
            case ConversationState::Open:
                sendDisconnected = true;
                break;
        }

        ChangeState(state, ConversationState::Failed);

        try
        {
            if (isRecognizer)
            {
                DisconnectConversation();
            }
            else
            {
                DisconnectTranslationRecognizer();
            }
        }
        catch (exception& ex)
        {
            SPX_TRACE_ERROR(
                "Exception while attempting to disconnect %s on 0x%p in failed state. What: %s",
                (isRecognizer ? "recognizer" : "conversation"), P_FORMAT_POINTER(this), ex.what());
        }
        catch (...)
        {
            SPX_TRACE_ERROR(
                "Unknown throwable while attempting to disconnect %s on 0x%p in failed state",
                (isRecognizer ? "recognizer" : "conversation"), P_FORMAT_POINTER(this));
        }

        string sessionId = GetSessionId();
        auto recoResult = CreateShared<ConversationCancellationResult>(GetParticipantId(), CancellationReason::Error, error);
        if (!message.empty())
        {
            recoResult->SetCancellationErrorDetails(message);
        }

        RaiseEvent<ConversationTranslationEventArgs>(Canceled, sessionId, recoResult);

        if (sendDisconnected)
        {
            RaiseEvent<ConversationConnectionEventArgs>(Disconnected, sessionId);
        }

        RaiseEvent<ConversationSessionEventArgs>(SessionStopped, sessionId);
    }

    void CSpxConversationTranslator::OnRecognizerSessionStarted(shared_ptr<ISpxSessionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            bool sendConnected = false;

            bool wasConnected = m_recognizerConnected.exchange(true);
            if (wasConnected)
            {
                SPX_TRACE_WARNING("Got a recognizer session started event on 0x%p when the recognizer is already open. Possible logic bug.", P_FORMAT_POINTER(this));
            }

            auto state = GetState();
            switch (state)
            {
                default:
                    SPX_TRACE_ERROR("Unknown state on 0x%p for recognizer session started event: %d", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Failed:
                case ConversationState::Closed:
                case ConversationState::Closing:
                    // should theoretically never get here
                    SPX_TRACE_WARNING("Got recognizer session started event on 0x%p when state is already closing/closed/failed (%d). Possible logic bug.", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Opening:
                    sendConnected = true;
                    ChangeState(state, ConversationState::PartiallyOpen);
                    break;

                case ConversationState::PartiallyOpen:
                    // if the conversation web socket connection is open, we can move to Open state
                    if (IsConversationConnected())
                    {
                        sendConnected = true;
                        ChangeState(state, ConversationState::Open);
                    }
                    break;

                case ConversationState::Open:
                    // In theory, this shouldn't happen so we log that we may have a logic bug and move on
                    SPX_TRACE_WARNING("Got recognizer session started event on 0x%p when state is open. Possible logic bug.", P_FORMAT_POINTER(this));
                    break;
            }

            if (sendConnected)
            {
                RaiseEvent<ConversationConnectionEventArgs>(Connected, GetSessionId());
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerSessionStopped(shared_ptr<ISpxSessionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            bool wasConnected = m_recognizerConnected.exchange(false);
            if (false == wasConnected)
            {
                SPX_TRACE_WARNING("Got a recognizer session stopped event on 0x%p when the recognizer is already closed. Possible logic bug.", P_FORMAT_POINTER(this));
            }

            bool sendDisconnected = false;
            bool sendStopped = false;

            auto state = GetState();
            switch (state)
            {
                default:
                    SPX_TRACE_ERROR("Unknown state on 0x%p for recognizer session stopped event: %d", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Failed:
                    // Ignore. This can happen if the failure was caused by the conversation
                    // web socket connection. In that case, we will tear down the translator
                    // recognizer
                    return;

                case ConversationState::Opening:
                case ConversationState::Closed:
                    // This is unexpected. Log this but ignore
                    SPX_TRACE_WARNING("Got recognizer session stopped event on 0x%p when state is Opening/Closed (%d). Possible logic bug.", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Closing:
                    // If the conversation connection is also closed, we should transition to
                    // the closed state. Otherwise we wait for that to trigger the closed state
                    if (false == IsConversationConnected())
                    {
                        sendStopped = true;
                        ChangeState(state, ConversationState::Closed);

                        DisconnectTranslationRecognizer();
                    }
                    break;

                case ConversationState::PartiallyOpen:
                    // do nothing for now. The conversation events will trigger a state transition as needed
                    SPX_TRACE_WARNING("Got recognizer session stopped event on 0x%p when state is PartiallyOpen (%d).", P_FORMAT_POINTER(this), state);
                    break;

                case ConversationState::Open:
                    sendDisconnected = true;
                    ChangeState(state, ConversationState::PartiallyOpen);
                    break;
            }

            string sessionId = GetSessionId();

            if (sendDisconnected)
            {
                RaiseEvent<ConversationConnectionEventArgs>(Disconnected, sessionId);
            }

            if (sendStopped)
            {
                RaiseEvent<ConversationSessionEventArgs>(SessionStopped, sessionId);
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerConnected(shared_ptr<ISpxConnectionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            m_recognizerConnected.exchange(true);

            bool sendConnected = false;

            auto state = GetState();
            switch (state)
            {
                default:
                    SPX_TRACE_ERROR("Unknown state on 0x%p for recognizer connected event: %d", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Failed:
                case ConversationState::Closing:
                case ConversationState::Closed:
                    // This is unexpected. Log but ignore
                    SPX_TRACE_WARNING("Got recognizer session connected event on 0x%p when state is already closing/closed/failed (%d). Possible logic bug.", P_FORMAT_POINTER(this), state);
                    return;


                case ConversationState::PartiallyOpen:
                    if (IsConversationConnected())
                    {
                        sendConnected = true;
                        ChangeState(state, ConversationState::Open);
                    }
                    break;

                case ConversationState::Open:
                    // Unexpected since we think we are already connection
                    SPX_TRACE_WARNING("Got recognizer session connected event on 0x%p when state is open. Possible logic bug.", P_FORMAT_POINTER(this));
                    break;

                case ConversationState::Opening:
                    sendConnected = true;
                    if (IsConversationConnected())
                    {
                        ChangeState(state, ConversationState::Open);
                    }
                    else
                    {
                        ChangeState(state, ConversationState::PartiallyOpen);
                    }
                    break;
            }

            if (sendConnected)
            {
                RaiseEvent<ConversationConnectionEventArgs>(Connected, GetSessionId());
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerDisconnected(shared_ptr<ISpxConnectionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            bool wasConnected = m_recognizerConnected.exchange(false);
            if (false == wasConnected)
            {
                SPX_TRACE_WARNING("Got a recognizer disconnected event on 0x%p when the recognizer is already closed. Possible logic bug.", P_FORMAT_POINTER(this));
            }

            bool sendStopped = false;
            bool sendDisconnected = false;

            auto state = GetState();
            switch (state)
            {
                default:
                    SPX_TRACE_ERROR("Unknown state on 0x%p for recognizer connected event: %d", P_FORMAT_POINTER(this), state);
                    return;

                case ConversationState::Failed:
                    // Ignore. This can happen if the failure was caused by the conversation
                    // web socket connection. In that case, we will tear down the translator
                    // recognizer
                    return;

                case ConversationState::Closed:
                case ConversationState::Opening:
                    // This is unexpected. Log this but ignore
                    SPX_TRACE_WARNING("Got recognizer session disconnected event on 0x%p when state is already opening/closed (%d). Possible logic bug.", P_FORMAT_POINTER(this), state);
                    return;
                
                case ConversationState::Closing:
                    // If the conversation connection is also closed, we should transition to
                    // the closed state. Otherwise we wait for that to trigger the closed state
                    if (false == IsConversationConnected())
                    {
                        sendStopped = true;
                        ChangeState(state, ConversationState::Closed);

                        DisconnectTranslationRecognizer();
                    }
                    break;

                case ConversationState::PartiallyOpen:
                    // do nothing for now. The conversation events will trigger a state transition as needed
                    SPX_TRACE_WARNING("Got recognizer disconnected event on 0x%p when state is PartiallyOpen (%d).", P_FORMAT_POINTER(this), state);
                    break;

                case ConversationState::Open:
                    sendDisconnected = true;
                    ChangeState(state, ConversationState::PartiallyOpen);
                    break;
                
            }

            string sessionId = GetSessionId();

            if (sendDisconnected)
            {
                RaiseEvent<ConversationConnectionEventArgs>(Disconnected, sessionId);
            }

            if (sendStopped)
            {
                RaiseEvent<ConversationSessionEventArgs>(SessionStopped, sessionId);
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerResult(shared_ptr<ISpxRecognitionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            if (!IsConsideredOpen())
            {
                SPX_TRACE_WARNING("Got a recognizer speech recognition event on 0x%p when state is not considered open.", P_FORMAT_POINTER(this));
                return;
            }

            shared_ptr<ISpxRecognitionResult> result;
            if (args == nullptr || (result = args->GetResult()) == nullptr)
            {
                // TODO log error
                return;
            }

            bool isFinal;
            ResultReason reason;

            switch (result->GetReason())
            {
                case ResultReason::RecognizedSpeech:
                case ResultReason::TranslatedSpeech:
                    isFinal = true;
                    reason = ResultReason::TranslatedSpeech;
                    break;

                case ResultReason::RecognizingSpeech:
                case ResultReason::TranslatingSpeech:
                    isFinal = false;
                    reason = ResultReason::TranslatingSpeech;
                    break;

                default:
                    SPX_TRACE_WARNING("a recognizer speech recognition event on 0x%p with an unknown reason %d.", P_FORMAT_POINTER(this), result->GetReason());
                    return;
            }

            // The recognizer generates a different result for each partial and final
            // for the same utterance. Instead we would like a consistent ID across
            // the same utterance.
            string resultId = m_utteranceId;
            if (isFinal || resultId.empty())
            {
                m_utteranceId = StringUtils::ToUpper(PAL::CreateGuidWithDashesUTF8());
                if (resultId.empty())
                {
                    resultId = m_utteranceId;
                }
            }

            auto recoResult = CreateShared<ConversationRecognitionResult>(
                resultId,
                PAL::ToString(result->GetText()),
                m_speechLang,
                reason,
                GetParticipantId(),
                result->GetOffset(),
                result->GetDuration()
            );

            auto translationArgs = result->QueryInterface<ISpxTranslationRecognitionResult>();
            if (translationArgs != nullptr)
            {
                for (const auto& entry : translationArgs->GetTranslationText())
                {
                    recoResult->AddTranslation(entry.first, entry.second);
                }
            }

            RaiseEvent<ConversationTranslationEventArgs>(isFinal ? Transcribed : Transcribing, GetSessionId(), recoResult);
        });
    }

    void CSpxConversationTranslator::OnRecognizerCanceled(shared_ptr<ISpxRecognitionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

            shared_ptr<ISpxRecognitionResult> result;
            if (args == nullptr
                || (result = args->GetResult()) == nullptr
                || result->GetReason() != ResultReason::Canceled)
            {
                SPX_TRACE_ERROR("Got a malformed recognizer canceled event on 0x%p. Event args were null, result was null, or result reason was not canceled", P_FORMAT_POINTER(this));
                return;
            }

            switch (result->GetCancellationReason())
            {
                case CancellationReason::EndOfStream:
                {
                    m_recognizerConnected.exchange(false);

                    SPX_TRACE_INFO("Got an end of audio stream event from the recognizer on 0x%p", P_FORMAT_POINTER(this));
                    auto canceledResult = CreateShared<ConversationCancellationResult>(
                        GetParticipantId(),
                        result->GetCancellationReason(),
                        result->GetCancellationErrorCode());

                    RaiseEvent<ConversationTranslationEventArgs>(Canceled, GetSessionId(), canceledResult);
                    break;
                }

                case CancellationReason::Error:
                {
                    SPX_TRACE_ERROR("Got an error canceled event from the recognizer on 0x%p. ErrorCode: %d", P_FORMAT_POINTER(this), result->GetCancellationErrorCode());
                    std::string errorDetails;
                    auto namedProperties = args->QueryInterface<ISpxNamedProperties>();
                    if (namedProperties != nullptr)
                    {
                        errorDetails = namedProperties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), "");
                    }

                    ToFailedState(true, result->GetCancellationErrorCode(), errorDetails);
                    break;
                }

                default:
                    SPX_TRACE_ERROR("Got an error canceled event from the recognizer on 0x%p with an unknown cancellation reason: %d", P_FORMAT_POINTER(this), result->GetCancellationReason());
                    break;
            }
        });
    }

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
