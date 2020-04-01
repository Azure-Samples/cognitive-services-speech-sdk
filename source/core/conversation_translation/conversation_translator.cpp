//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// conversation_translator.cpp: implementation declarations for conversation translator
//

#include "common.h"
#include <sstream>
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
#include "conversation_translator_logging.h"
#include "conversation_translator_connection.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    using namespace std;
    using StringUtils = PAL::StringUtils;


    const char * ConversationStateString(const CSpxConversationTranslator::ConversationState state)
    {
        switch (state)
        {
            case CSpxConversationTranslator::ConversationState::Failed: return "Failed";
            case CSpxConversationTranslator::ConversationState::Initial: return "Initial";
            case CSpxConversationTranslator::ConversationState::Closed: return "Closed";
            case CSpxConversationTranslator::ConversationState::Closing: return "Closing";
            case CSpxConversationTranslator::ConversationState::CreatingOrJoining: return "CreatingOrJoining";
            case CSpxConversationTranslator::ConversationState::CreatedOrJoined: return "CreatedOrJoined";
            case CSpxConversationTranslator::ConversationState::Opening: return "Opening";
            case CSpxConversationTranslator::ConversationState::Open: return "Open";
            default:  return "<<UnknownState>>";
        }
    }

    /// <summary>
    /// Creates an object with the specified site. This will implicitly call the Init() method on that
    /// object if it implements ISpxObjectInit
    /// </summary>
    template <class I>
    static inline shared_ptr<I> CreateAndInitObjectWithSite(const char* className, shared_ptr<ISpxRecognizerSite> site)
    {
        // get the base site if available
        auto baseSite = SpxQueryInterface<ISpxGenericSite>(site);
        CT_THROW_HR_IF(baseSite == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

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
    static inline shared_ptr<I> As(shared_ptr<ISpxRecognizer> reco)
    {
        CT_THROW_HR_IF(reco == nullptr, SPXERR_UNINITIALIZED);
        auto cast = reco->QueryInterface<I>();
        CT_THROW_HR_IF(cast == nullptr, SPXERR_RUNTIME_ERROR);
        return cast;
    }

    static inline shared_ptr<ISpxNamedProperties> AsNamedProperties(shared_ptr<ISpxRecognizer> reco)
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
            CT_LOG_ERROR("Exception while raising event. EventArgs: %p, What: %s", P_FORMAT_POINTER(eventArgs.get()), ex.what());
        }
        catch (...)
        {
            CT_LOG_ERROR("Unknown throwable while raising event. EventArgs: %p", P_FORMAT_POINTER(eventArgs.get()));
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

    struct ErrorCodeHandling
    {
        bool isPermanentFailure;
        CancellationErrorCode errorCode;
    };

    static ErrorCodeHandling ToCancellationErrorCode(bool isWebSocket, ConversationErrorCode error)
    {
        ErrorCodeHandling ret;
        ret.isPermanentFailure = false;
        ret.errorCode = CancellationErrorCode::RuntimeError;

        if (!isWebSocket)
        {
            ret.isPermanentFailure = false;
            ret.errorCode = CancellationErrorCode::RuntimeError;
            return ret;
        }

        switch (error)
        {
            case ConversationErrorCode::AuthenticationError:
                ret.isPermanentFailure = true;
                ret.errorCode = CancellationErrorCode::AuthenticationFailure;
                break;
            case ConversationErrorCode::BadRequest:
                ret.isPermanentFailure = true;
                ret.errorCode = CancellationErrorCode::BadRequest;
                break;
            case ConversationErrorCode::ConnectionError:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::ConnectionFailure;
                break;
            case ConversationErrorCode::Forbidden:
                ret.isPermanentFailure = true;
                ret.errorCode = CancellationErrorCode::Forbidden;
                break;
            case ConversationErrorCode::RuntimeError:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::RuntimeError;
                break;
            case ConversationErrorCode::ServiceError:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::ServiceError;
                break;
            case ConversationErrorCode::ServiceUnavailable:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::ServiceUnavailable;
                break;
            case ConversationErrorCode::TooManyRequests:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::TooManyRequests;
                break;
            default:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::RuntimeError;
                break;
        }

        return ret;
    }

    static ErrorCodeHandling ParseDisconnectReason(const USP::WebSocketDisconnectReason reason)
    {
        ErrorCodeHandling ret;
        ret.isPermanentFailure = false;
        ret.errorCode = CancellationErrorCode::RuntimeError;

        switch (reason)
        {
            default:
            case USP::WebSocketDisconnectReason::Unknown:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::RuntimeError;
                break;

            case USP::WebSocketDisconnectReason::Normal:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::NoError;
                break;

            case USP::WebSocketDisconnectReason::EndpointUnavailable:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::ServiceUnavailable;
                break;

            case USP::WebSocketDisconnectReason::ProtocolError:
            case USP::WebSocketDisconnectReason::CannotAcceptDataType:
            case USP::WebSocketDisconnectReason::InvalidPayloadData:
            case USP::WebSocketDisconnectReason::PolicyViolation:
            case USP::WebSocketDisconnectReason::MessageTooBig:
            case USP::WebSocketDisconnectReason::UnexpectedCondition:
                ret.isPermanentFailure = true;
                ret.errorCode = CancellationErrorCode::BadRequest;
                break;

            case USP::WebSocketDisconnectReason::InternalServerError:
                ret.isPermanentFailure = false;
                ret.errorCode = CancellationErrorCode::ServiceError;
                break;
        }

        return ret;
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
    /// <param name="event">The event to register a handler for</param>
    /// <param name="instance">The instance to call the callback method on</param>
    /// <param name="callback">The callback method to invoke</param>
    /// <typeparam name="TEventArgs">The type of the event arguments for this event</typeparam>
    template<typename TEventArgs>
    static void AddHandler(EventSignal<shared_ptr<TEventArgs>>& event, CSpxConversationTranslator* instance, void (CSpxConversationTranslator:: *callback)(shared_ptr<TEventArgs>))
    {
        weak_ptr<ISpxConversationTranslator> weak(instance->ISpxConversationTranslator::shared_from_this());
        event.Connect([weak, callback](shared_ptr<TEventArgs> eventArgs)
        {
            auto ptr = dynamic_pointer_cast<CSpxConversationTranslator>(weak.lock());
            if (ptr != nullptr)
            {
                (ptr.get()->*callback)(eventArgs);
            }
        });
    }

    CSpxConversationTranslator::CSpxConversationTranslator() :
        ThreadingHelpers(ISpxThreadService::Affinity::User),
        m_state_(ConversationState::Initial),
        _m_conv(),
        m_convInternals(),
        m_endConversationOnLeave(false),
        m_recognizer(),
        m_recognizerConnected(false),
        m_speechLang(),
        m_audioInput(),
        m_utteranceId(),
        m_convDeletedEvtHandlerId()
    {
        CT_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

        // we should create the TranslatorRecognizer instance here but NOT call Init()
        // on it, nor set its site
        m_recognizer = SpxCreateObject<ISpxRecognizer>("CSpxTranslationRecognizer", SpxGetRootSite());
        CT_I_THROW_HR_IF(m_recognizer == nullptr, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);
    }

    CSpxConversationTranslator::~CSpxConversationTranslator()
    {
        CT_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        Term();
    }

    void CSpxConversationTranslator::Init()
    {
        CT_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        ThreadingHelpers::Init();

        shared_ptr<ISpxRecognizerSite> site = GetSite();
        CT_I_THROW_HR_IF(site == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        auto genericSite = SpxQueryInterface<ISpxGenericSite>(site);
        CT_I_THROW_HR_IF(genericSite == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        auto factory = SpxQueryService<ISpxObjectFactory>(site);
        CT_I_THROW_HR_IF(factory == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        auto properties = SpxQueryService<ISpxNamedProperties>(site);
        CT_I_THROW_HR_IF(properties == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        m_speechLang = properties->GetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoLanguage), "");
        CT_I_THROW_HR_IF(m_speechLang.empty(), SPXERR_INVALID_ARG);

        // Initialise the speech translator recognizer instance by setting the site.
        auto withSite = As<ISpxObjectWithSite>(m_recognizer);
        withSite->SetSite(genericSite);

        ConnectRecognizer(m_recognizer);
    }

    void CSpxConversationTranslator::Term()
    {
        m_recognizerConnected = false;
        DisconnectRecognizer(false);
        m_recognizer = nullptr;
        DisconnectConversation(false);

        m_audioInput.reset();

        // release our references in the right order
        m_convInternals.reset();
        _m_conv.reset();
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
        CT_DBG_TRACE_FUNCTION();

        CT_I_THROW_HR_IF(GetSite() == nullptr || m_threadService == nullptr, SPXERR_UNINITIALIZED);
        CT_I_THROW_HR_IF(conversation == nullptr, SPXERR_INVALID_ARG);

        RunSynchronously([this, conversation, nickname, endConversationOnLeave]()
        {
            CT_GET_AND_LOG_STATE(
                "JoinConversation 0x%p, ConverastionId: %s, nickname: %s, endOnLeave: %d",
                P_FORMAT_POINTER(conversation.get()), conversation->GetConversationId().c_str(), nickname.c_str(), endConversationOnLeave);

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Closed:
                case ConversationState::Closing:
                case ConversationState::CreatingOrJoining:
                case ConversationState::CreatedOrJoined:
                case ConversationState::Opening:
                case ConversationState::Open:
                    CT_I_THROW_HR(SPXERR_INVALID_STATE);
                    return;

                case ConversationState::Initial:
                    // Because we are connecting to a CSPxConversationImpl instance, we always start at the
                    // open. Thus we should "jump ahead"
                    break;
            }

            // Hook up to this conversation to start receiving events
            ConnectConversation(conversation, endConversationOnLeave);

            auto convInternals = m_convInternals.lock();
            CT_I_THROW_HR_IF(convInternals == nullptr, SPXERR_UNINITIALIZED);

            // Update the user's nickname since we now have it
            if (!nickname.empty())
            {
                convInternals->GetConversationConnection()->SetNickname(nickname);
            }

            // send session started
            RaiseEvent<ConversationSessionEventArgs>(SessionStarted, GetSessionId());

            // Go to the correct state
            if (convInternals->IsConnected())
            {
                ToOpenState();
            }
            else
            {
                ToOpeningState();
            }

            // send participants event
            auto eventArgs = CreateShared<ConversationParticipantChangedEventArgs>(
                GetSessionId(), ParticipantChangedReason::JoinedConversation);

            auto conv = m_convInternals.lock();
            CT_I_THROW_HR_IF(conv == nullptr, SPXERR_UNINITIALIZED);

            for (const auto& p : conv->GetConversationConnection()->GetParticipants())
            {
                auto part = CreateShared<CSpxConversationParticipant>(p);
                eventArgs->AddParticipant(part);
            }

            RaiseEvent(ParticipantsChanged, eventArgs);
        });
    }

    void CSpxConversationTranslator::Connect()
    {
        CT_I_THROW_HR_IF(m_threadService == nullptr, SPXERR_INVALID_STATE);

        RunSynchronously([this]()
        {
            CT_GET_AND_LOG_STATE("User initiated connect");

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Initial:
                case ConversationState::Failed:
                case ConversationState::Closed:
                case ConversationState::Closing:
                case ConversationState::CreatingOrJoining:
                    CT_LOG_ERROR("Invalid state for connecting/reconnecting");
                    CT_I_THROW_HR(SPXERR_INVALID_STATE);
                    return;

                case ConversationState::Opening:
                case ConversationState::Open:
                    // Nothing to do since we are already reconnecting or connected
                    return;

                case ConversationState::CreatedOrJoined:
                    // let's try to rejoin
                    break;
            }

            // can we rejoin?
            auto convInternals = m_convInternals.lock();
            CT_I_THROW_HR_IF(convInternals == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

            if (convInternals->CanRejoin())
            {
                ToOpeningState();
            }
            else
            {
                CT_I_THROW_HR(SPXERR_INVALID_STATE);
            }
        });
    }

    void CSpxConversationTranslator::StartTranscribing()
    {
        RunSynchronously([this]()
        {
            CT_GET_AND_LOG_STATE("Start Transcribing");
            if (false == IsConsideredOpen(state))
            {
                CT_I_THROW_HR(SPXERR_INVALID_STATE);
            }

            As<ISpxRecognizer>(m_recognizer)->StartContinuousRecognitionAsync().Future.get();
        });
    }

    void CSpxConversationTranslator::StopTranscribing()
    {
        RunSynchronously([this]()
        {
            CT_GET_AND_LOG_STATE("Stop Transcribing");
            if (false == IsConsideredOpen(state))
            {
                CT_I_THROW_HR(SPXERR_INVALID_STATE);
            }

            As<ISpxRecognizer>(m_recognizer)->StopContinuousRecognitionAsync().Future.get();
        });
    }

    void CSpxConversationTranslator::SendTextMsg(const string& message)
    {
        RunSynchronously([this, message]()
        {
            CT_GET_AND_LOG_STATE("Send text message (%zu chars)", message.length());
            if (false == IsConsideredOpen(state))
            {
                CT_I_THROW_HR(SPXERR_INVALID_STATE);
            }

            auto convInternals = m_convInternals.lock();
            if (convInternals == nullptr)
            {
                CT_I_THROW_HR(SPXERR_INVALID_STATE);
            }

            convInternals->GetConversationConnection()->SendTextMessage(message);
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
            CT_GET_AND_LOG_STATE("Leave conversation");
            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Initial:
                case ConversationState::CreatingOrJoining:
                case ConversationState::Closed:
                case ConversationState::Closing:
                    // Calling leave should not throw exceptions even if you are not in the right state
                    CT_LOG_INFO("Leaving is not actionable in current state.");
                    return;

                case ConversationState::CreatedOrJoined:
                case ConversationState::Opening:
                case ConversationState::Open:
                    // Can leave
                    ToClosingState();
                    break;
            }

            // we want to get off the background thread now to give the event handlers a
            // chance to run. Those will set the state to closed
        }, std::move(promise));

        bool success = future.get();
        CT_I_THROW_HR_IF(false == success, SPXERR_CANCELED);

        RunSynchronously([this]() { DisconnectConversation(true); });
    }

    bool CSpxConversationTranslator::CanJoin() const
    {
        if (m_threadService == nullptr)
        {
            return true; // we are not initialised yet so assume we can join
        }

        bool canJoin = false;

        RunSynchronously([this, &canJoin]()
        {
            CT_GET_AND_LOG_STATE("Checking if can join conversation translator");
            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Closed:
                case ConversationState::Closing:
                case ConversationState::CreatingOrJoining:
                case ConversationState::CreatedOrJoined:
                case ConversationState::Opening:
                case ConversationState::Open:
                    canJoin = false;
                    break;

                case ConversationState::Initial:
                    canJoin = true;
                    break;
            }
        });

        return canJoin;
    }

    void CSpxConversationTranslator::Disconnect()
    {
        if (m_threadService == nullptr)
        {
            // we are not initialized and should not throw exceptions here
            return;
        }

        RunSynchronously([this]()
        {
            CT_GET_AND_LOG_STATE("Disconnect conversation");
            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Initial:
                case ConversationState::Closed:
                case ConversationState::Closing:
                case ConversationState::CreatingOrJoining:
                case ConversationState::CreatedOrJoined:
                    // nothing to do here
                    break;

                case ConversationState::Opening:
                case ConversationState::Open:
                    ToCreatedOrJoinedState(CancellationErrorCode::NoError, "");
                    break;
            }
        });
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
        // we cannot rely on the site to create objects since we may want to get a Connection
        // instance from this *before* the site is set. Since the Connection instance doesn't
        // need to have a site, we can create directly here
        auto ptr = new CSpxConversationTranslatorConnection();
        auto interface = static_cast<ISpxConnection*>(ptr);
        std::shared_ptr<ISpxConnection> connection(interface);

        auto connectionInit = connection->QueryInterface<ISpxConversationTranslatorConnection>();
        CT_I_THROW_HR_IF(connection == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);

        connectionInit->Init(ISpxConversationTranslator::shared_from_this(), m_recognizer);

        return connection;
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
            CT_GET_AND_LOG_STATE("Conversation connected");
            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Initial:
                case ConversationState::Closed:
                case ConversationState::CreatingOrJoining:
                case ConversationState::CreatedOrJoined:
                    CT_I_LOG_WARNING("Not expected here");
                    break;

                case ConversationState::Failed:
                case ConversationState::Open:
                    // don't care
                    break;

                case ConversationState::Closing:
                    // If the recognizer is also not connected, we should treat this as if the conversation
                    // connection successfully closed and go to the closed state
                    CT_I_LOG_WARNING("Not expected here");
                    if (!m_recognizerConnected.load())
                    {
                        ToClosedState();
                    }
                    
                    break;

                case ConversationState::Opening:
                    ToOpenState();
                    break;
            }
        });
    }

    void CSpxConversationTranslator::OnDisconnected(const USP::WebSocketDisconnectReason reason, const string & message, bool serverRequested)
    {
        RunAsynchronously([this, reason, message, serverRequested]()
        {
            CT_GET_AND_LOG_STATE(
                "Conversation disconnected. Reason: %d, Message: '%s', Server requested: %d",
                reason, message.c_str(), serverRequested);

            const auto parsedReason = ParseDisconnectReason(reason);

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                    // Ignore. This can happen if the failure was caused by the translator
                    // recognizer. In that case, we will tear down the conversation session
                    // recognizer
                    break;

                case ConversationState::Initial:
                case ConversationState::Closed:
                case ConversationState::CreatingOrJoining:
                    // should never get here
                    CT_I_LOG_WARNING("Not expected here");
                    break;

                case ConversationState::Closing:
                    // If the recognizer is also not connected, we should transition to the
                    // closed state. Otherwise we wait for that trigger the closed state
                    if (!m_recognizerConnected)
                    {
                        ToClosedState();
                    }
                    break;

                case ConversationState::CreatedOrJoined:
                    // Did we get here because of an error that is non recoverable (e.g. authentication failure,
                    // conversation has been deleted, etc...)? If so, go to failed state
                    if (parsedReason.isPermanentFailure)
                    {
                        ToFailedState(false, parsedReason.errorCode, message);
                    }
                    else
                    {
                        // no need to do anything more
                    }
                    
                    break;

                case ConversationState::Opening:
                case ConversationState::Open:
                    // Did the server request the disconnection and there are no errors? If so, and the
                    // "end conversation on leaving" flag is set (usually means you are the participant)
                    // then we treat this as the host has deleted the conversation and so we move to the
                    // closed state
                    if (reason == USP::WebSocketDisconnectReason::Normal
                        && serverRequested
                        && m_endConversationOnLeave)
                    {
                        CT_I_LOG_INFO("The host has most likely deleted the conversation. Will go to closed state");
                        ToClosedState();
                    }
                    else if (parsedReason.isPermanentFailure)
                    {
                        ToFailedState(false, parsedReason.errorCode, message);
                    }
                    else
                    {
                        ToCreatedOrJoinedState(parsedReason.errorCode, message);
                    }
                    break;
            }
        });
    }

    void CSpxConversationTranslator::OnSpeechRecognition(const ConversationSpeechRecognitionMessage & reco)
    {
        RunAsynchronously([this, reco]()
        {
            CT_GET_AND_LOG_STATE(
                "Conversation speech recognition. Id: %s, Time: %s, From: %s (%s), %zu chars",
                reco.Id.c_str(), reco.Timestamp.c_str(), reco.Nickname.c_str(), reco.ParticipantId.c_str(), reco.Recognition.length());

            if (!IsConsideredOpen(state))
            {
                CT_I_LOG_WARNING("Got speech recognition when state is not considered open.");

                // though not expected, we should err on the side of caution and try to raise the event
                // anyway in case we ran into a bug in our state machine. That is better than losing
                // events
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
            CT_GET_AND_LOG_STATE(
                "Conversation instant message. Id: %s, Time: %s, From: %s (%s), %zu chars",
                im.Id.c_str(), im.Timestamp.c_str(), im.Nickname.c_str(), im.ParticipantId.c_str(), im.Text.length());

            if (!IsConsideredOpen(state))
            {
                CT_I_LOG_WARNING("Got an instant message event when state is not considered open.");

                // though not expected, we should err on the side of caution and try to raise the event
                // anyway in case we ran into a bug in our state machine. That is better than losing
                // events
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
            CT_GET_AND_LOG_STATE(
                "Conversation participant changed event. Action: %u, Participants changed: %zu",
                action, participants.size());

            if (!IsConsideredOpen(state))
            {
                CT_I_LOG_WARNING("Got a participant changed event when state is not considered open.");

                // though not expected, we should err on the side of caution and try to raise the event
                // anyway in case we ran into a bug in our state machine. That is better than losing
                // events
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
            CT_GET_AND_LOG_STATE("Conversation will expire in %d minutes", minutesLeft);
            if (!IsConsideredOpen(state))
            {
                CT_I_LOG_WARNING("Got a room expiration event when state is not considered open.");
                return;
            }

            RaiseEvent<ConversationExpirationEventArgs>(ConversationExpiration, GetSessionId(), minutesLeft);
        });
    }

    void CSpxConversationTranslator::OnError(const bool isWebSocket, const ConversationErrorCode error, const string & message)
    {
        RunAsynchronously([this, isWebSocket, error, message]()
        {
            CT_GET_AND_LOG_STATE(
                "Conversation connection error. IsWebSocket: %d, Error: %u, Message: '%s'",
                isWebSocket, error, message.c_str());

            auto parsedError = ToCancellationErrorCode(isWebSocket, error);

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Closed:
                    // Ignore this
                    return;

                case ConversationState::Initial:
                case ConversationState::CreatingOrJoining:
                    CT_I_LOG_WARNING("Not expected");
                    return;

                case ConversationState::Closing:
                    // If the recognizer is also not connected, we should treat this as if the conversation
                    // connection successfully closed and go to the closed state
                    if (!m_recognizerConnected)
                    {
                        ToClosedState();
                    }
                    break;

                case ConversationState::CreatedOrJoined:
                    if (parsedError.isPermanentFailure)
                    {
                        ToFailedState(false, parsedError.errorCode, message);
                    }
                    else
                    {
                        // nothing more to do here
                    }
                    break;

                case ConversationState::Opening:
                case ConversationState::Open:
                    if (parsedError.isPermanentFailure)
                    {
                        ToFailedState(false, parsedError.errorCode, message);
                    }
                    else
                    {
                        ToCreatedOrJoinedState(parsedError.errorCode, message);
                    }

                    break;
            }
        });
    }

    void CSpxConversationTranslator::OnConversationDeleted()
    {
        // In order to ensure we move to the correct state, we need to run this synchronously
        // to block execution from continuing.
        RunSynchronously([this]()
        {
            CT_GET_AND_LOG_STATE("Conversation was deleted");

            switch (state)
            {
            default:
                CT_HANDLE_UNSUPPORTED_STATE();
                return;

            case ConversationState::Failed:
            case ConversationState::Closed:
                // Ignore this
                return;

            case ConversationState::Initial:
            case ConversationState::CreatingOrJoining:
                CT_I_LOG_WARNING("Not expected");
                ToClosedState();
                return;

            case ConversationState::Closing:
            case ConversationState::CreatedOrJoined:
            case ConversationState::Opening:
            case ConversationState::Open:
                ToClosedState();
                break;
            }
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
        CT_I_THROW_HR_IF(internals == nullptr, SPXERR_INVALID_ARG);
        CT_I_THROW_HR_IF(internals->GetConversationArgs() == nullptr, SPXERR_INVALID_ARG);
        CT_I_THROW_HR_IF(internals->GetConversationConnection() == nullptr, SPXERR_INVALID_ARG);
        CT_I_THROW_HR_IF(internals->GetConversationManager() == nullptr, SPXERR_INVALID_ARG);

        CT_I_THROW_HR_IF(_m_conv != nullptr, SPXERR_ALREADY_INITIALIZED);

        // NOTE:
        // Since the instance of ISpxConversationInternals is contained in the parent
        // ISpxConveration instance, we shouldn't take ownership of it nor extend its
        // lifetime here. Doing so can lead to problems as the parent instance going
        // out of scope will terminate the child ISpxConversationInternals instance
        // (it calls Term() on it). We would then be left holding onto a zombie.
        _m_conv = conversation; // shared_ptr
        m_convInternals = internals; // weak_ptr
        m_endConversationOnLeave = endConversationOnLeave;

        // register event handlers
        internals->GetConversationConnection()->SetCallbacks(
            dynamic_pointer_cast<ConversationCallbacks>(ISpxInterfaceBase::shared_from_this()));

        auto self = std::dynamic_pointer_cast<CSpxConversationTranslator>(ISpxInterfaceBase::shared_from_this());
        CT_I_THROW_HR_IF(self == nullptr, SPXERR_UNEXPECTED_CONVERSATION_TRANSLATOR_SITE_FAILURE);
        m_convDeletedEvtHandlerId = internals->ConversationDeleted.add(self, &CSpxConversationTranslator::OnConversationDeleted);

        // Set the conversation token to be sent to the speech service. This enables the
        // service to service call from the speech service to the Capito service for relaying
        // recognitions.
        SetStringValue(ConversationKeys::Conversation_Token, internals->GetConversationArgs()->SessionToken.c_str());
        SetStringValue(PropertyId::SpeechServiceAuthorization_Token, internals->GetConversationArgs()->CognitiveSpeechAuthToken.c_str());

        if (IsMultiChannelAudio())
        {
            // set the additional endpoint URL query parameters
            auto args = internals->GetConversationArgs();

            std::string existingQuery = GetStringValue(PropertyId::SpeechServiceConnection_UserDefinedQueryParameters);

            std::ostringstream queryParms; 
            queryParms << existingQuery;
            if (!existingQuery.empty())
            {
                queryParms << "&";
            }

            queryParms << "language=" << HttpUtils::UrlEscape(GetStringValue(PropertyId::SpeechServiceConnection_RecoLanguage))
                       << "&meetingId=" << HttpUtils::UrlEscape(args->RoomCode)
                       << "&deviceId=" << HttpUtils::UrlEscape(args->ParticipantId);

            SetStringValue(PropertyId::SpeechServiceConnection_UserDefinedQueryParameters, queryParms.str().c_str());
        }

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
    }

    bool CSpxConversationTranslator::StopConversation()
    {
        auto convInternals = m_convInternals.lock();
        if (convInternals != nullptr && convInternals->IsConnected())
        {
            convInternals->GetConversationConnection()->Disconnect();
            return true;
        }

        return false;
    }

    void CSpxConversationTranslator::DisconnectConversation(bool waitForEvents)
    {
        auto conv = _m_conv;
        if (conv == nullptr)
        {
            return;
        }

        auto convInternals = m_convInternals.lock();
        if (convInternals == nullptr)
        {
            return;
        }

        bool wasConnected = StopConversation();

        if (!waitForEvents || !wasConnected)
        {
            // remove callbacks
            convInternals->GetConversationConnection()->SetCallbacks(nullptr);
            convInternals->ConversationDeleted.remove(m_convDeletedEvtHandlerId);
            m_convDeletedEvtHandlerId = 0;
        }
    }

    void CSpxConversationTranslator::ConnectRecognizer(std::shared_ptr<ISpxRecognizer> recognizer)
    {
        auto events = recognizer->QueryInterface<ISpxRecognizerEvents>();
        CT_I_THROW_HR_IF(events == nullptr, SPXERR_UNEXPECTED_CREATE_OBJECT_FAILURE);

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
            session->AddRecognizer(recognizer);
        }
        else
        {
            CT_I_LOG_WARNING("Unexpected null default session on 0x%p", P_FORMAT_POINTER(this));
        }
    }

    bool CSpxConversationTranslator::StopRecognizer()
    {
        auto reco = m_recognizer;
        if (reco == nullptr)
        {
            return false;
        }

        // attempt to stop audio capture
        try
        {
            reco->StopContinuousRecognitionAsync().Future.get();
        }
        catch (exception& ex)
        {
            CT_I_LOG_ERROR("Exception while attempting to stop continuous audio recognition on recognizer. What: %s", ex.what());
        }
        catch (...)
        {
            CT_I_LOG_ERROR("Unknown throwable while attempting to stop continuous audio recognition on recognizer");
        }

        if (m_recognizerConnected.load())
        {
            reco->CloseConnection();
            return true;
        }

        return false;
    }

    void CSpxConversationTranslator::DisconnectRecognizer(bool waitForEvents)
    {
        auto reco = m_recognizer;
        if (reco == nullptr)
        {
            return;
        }

        auto events = reco->QueryInterface<ISpxRecognizerEvents>();
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

        bool wasConnected = StopRecognizer();

        if (!waitForEvents || !wasConnected)
        {
            events->SessionStopped.DisconnectAll();
        }
    }


    inline bool CSpxConversationTranslator::IsConsideredOpen(ConversationState state)
    {
        switch (state)
        {
            default:
            case ConversationState::Failed:
            case ConversationState::Initial:
            case ConversationState::Closed:
            case ConversationState::Closing:
            case ConversationState::CreatingOrJoining:
            case ConversationState::CreatedOrJoined:
            case ConversationState::Opening:
                return false;

            case ConversationState::Open:
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

    inline bool CSpxConversationTranslator::IsMultiChannelAudio() const
    {
        auto multiChannelAudio = GetStringValue(ConversationKeys::Conversation_MultiChannelAudio, "false");
        return PAL::ToBool(multiChannelAudio);
    }

    struct CSpxConversationTranslator::EventsToSend
    {
        EventsToSend() :
            sessionStarted(false),
            connected(false),
            canceled(false),
            cancellationResult(),
            disconnected(false),
            sessionStopped(false)
        {
        }

        bool sessionStarted;
        bool connected;
        bool canceled;
        std::shared_ptr<ConversationCancellationResult> cancellationResult;
        bool disconnected;
        bool sessionStopped;
    };

    void CSpxConversationTranslator::SendStateEvents(const EventsToSend & evt)
    {
        std::string sessionId = GetSessionId();

        if (evt.sessionStarted)
        {
            RaiseEvent<ConversationSessionEventArgs>(SessionStarted, sessionId);
        }

        if (evt.connected)
        {
            RaiseEvent<ConversationConnectionEventArgs>(Connected, sessionId);
        }

        if (evt.canceled)
        {
            RaiseEvent<ConversationTranslationEventArgs>(Canceled, sessionId, evt.cancellationResult);
        }

        if (evt.disconnected)
        {
            RaiseEvent<ConversationConnectionEventArgs>(Disconnected, sessionId);
        }

        if (evt.sessionStopped)
        {
            RaiseEvent<ConversationSessionEventArgs>(SessionStopped, sessionId);
        }
    }

    CSpxConversationTranslator::EventsToSend CSpxConversationTranslator::GetStateExitEvents()
    {
        EventsToSend evt;

        switch (m_state_.load())
        {
            case ConversationState::Failed:
            case ConversationState::Initial:
            case ConversationState::Closed:
            case ConversationState::CreatedOrJoined:
            case ConversationState::Opening:
            case ConversationState::Closing:
                // no exit events to send
                break;

            case ConversationState::CreatingOrJoining:
                evt.sessionStarted = true;
                break;
            case ConversationState::Open:
                evt.disconnected = true;
                break;
        }

        return evt;
    }

    void CSpxConversationTranslator::ToFailedState(bool isRecognizer, CancellationErrorCode error, const std::string& message)
    {
        auto evt = GetStateExitEvents();

        CT_GET_AND_LOG_STATE(
            "Transition to failed state. IsRecognizer: %d, Error: %d, Message: '%s'",
            isRecognizer, error, message.c_str());

        // Failed state always sends canceled and session stopped events except if coming from creating or joining state.
        // This is because we have not yet sent any session started event
        if (state != ConversationState::CreatingOrJoining)
        {
            evt.canceled = true;
            evt.sessionStopped = true;

            evt.cancellationResult = CreateShared<ConversationCancellationResult>(GetParticipantId(), CancellationReason::Error, error);
            if (!message.empty())
            {
                evt.cancellationResult->SetCancellationErrorDetails(message);
            }
        }

        SetState(ConversationState::Failed);

        // Disconnect if we are still connected and remove all registered handlers
        DisconnectRecognizer(false);
        DisconnectConversation(false);

        // TODO: once CSpxConversationImpl and this class are merged, we need to call Delete the conversation
        //       here for both host and participant

        // send events
        SendStateEvents(evt);
    }

    void CSpxConversationTranslator::ToClosedState()
    {
        CT_I_LOG_INFO("Transition to closed state");

        auto evt = GetStateExitEvents();
        evt.sessionStopped = true;

        SetState(ConversationState::Closed);

        DisconnectRecognizer(false);
        DisconnectConversation(false);

        // TODO: once CSpxConversationImpl and this class are merged, we need to call Delete the conversation
        //       here for both host and participant

        SendStateEvents(evt);
    }

    void CSpxConversationTranslator::ToClosingState()
    {
        CT_I_LOG_INFO("Transition to closing state");

        auto evt = GetStateExitEvents();

        SetState(ConversationState::Closing);

        // Should always stop the recognizer
        DisconnectRecognizer(true);

        // Disconnect and end the conversation if needed (e.g. joined as a participant)
        if (m_endConversationOnLeave)
        {
            DisconnectConversation(true);
        }

        SendStateEvents(evt);
    }

    void CSpxConversationTranslator::ToCreatingOrJoiningState()
    {
        CT_I_LOG_INFO("Transition to creating or joining state");

        auto evt = GetStateExitEvents();
        SetState(ConversationState::CreatingOrJoining);
        // This state does nothing at the moment since the logic here is part of CSpxConversationImpl
        // At some point I hope to merge that into this class for cleaner overall state
        SendStateEvents(evt);
    }

    void CSpxConversationTranslator::ToCreatedOrJoinedState(CancellationErrorCode error, const std::string& message)
    {
        auto evt = GetStateExitEvents();

        CT_GET_AND_LOG_STATE(
            "Transition to created or joined state. Error: %d, Message: '%s'",
            error, message.c_str());

        if (error != CancellationErrorCode::NoError)
        {
            evt.canceled = true;
            evt.cancellationResult = CreateShared<ConversationCancellationResult>(GetParticipantId(), CancellationReason::Error, error);
            if (!message.empty())
            {
                evt.cancellationResult->SetCancellationErrorDetails(message);
            }
        }

        SetState(ConversationState::CreatedOrJoined);

        // stop recognizer and conversation connection
        if (state != ConversationState::CreatedOrJoined)
        {
            StopRecognizer();
            StopConversation();
        }

        SendStateEvents(evt);
    }

    void CSpxConversationTranslator::ToOpeningState()
    {
        CT_I_LOG_INFO("Transition to opening state");

        auto evt = GetStateExitEvents();

        SetState(ConversationState::Opening);

        bool wasConnected = false;

        // attempt to connect/reconnect to the conversation
        auto convInternals = m_convInternals.lock();
        if (convInternals != nullptr)
        {
            if (convInternals->IsConnected())
            {
                wasConnected = true;
            }
            else
            {
                auto connection = convInternals->GetConversationConnection();
                auto args = convInternals->GetConversationArgs();

                connection->Connect(args->ParticipantId, args->SessionToken);
            }
        }

        SendStateEvents(evt);

        if (wasConnected)
        {
            ToOpenState();
        }
    }

    void CSpxConversationTranslator::ToOpenState()
    {
        CT_I_LOG_INFO("Transition to open state");

        auto evt = GetStateExitEvents();

        evt.connected = true;

        SetState(ConversationState::Open);

        SendStateEvents(evt);
    }

    void CSpxConversationTranslator::OnRecognizerSessionStarted(shared_ptr<ISpxSessionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            bool wasConnected = m_recognizerConnected.exchange(true);
            CT_GET_AND_LOG_STATE("Recognizer session started. Was connected: %d, Session ID: %ls", wasConnected, args->GetSessionId().c_str());

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Initial:
                case ConversationState::Closed:
                case ConversationState::CreatingOrJoining:
                case ConversationState::CreatedOrJoined:
                case ConversationState::Opening:
                    CT_I_LOG_WARNING("Not expected");
                    break;

                case ConversationState::Closing:
                    if (!IsConversationConnected())
                    {
                        ToClosedState();
                    }
                    break;
                
                case ConversationState::Open:
                    // This is normal. nothing to do here
                    break;
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerSessionStopped(shared_ptr<ISpxSessionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            bool wasConnected = m_recognizerConnected.exchange(false);
            CT_GET_AND_LOG_STATE("Recognizer session stopped. Was connected: %d, Session ID: %ls", wasConnected, args->GetSessionId().c_str());

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;
                
                case ConversationState::Initial:
                case ConversationState::CreatingOrJoining:
                    CT_I_LOG_WARNING("Not expected");
                    break;

                case ConversationState::Closing:
                    // If the conversation connection is also closed, we should transition to
                    // the closed state. Otherwise we wait for that to trigger the closed state
                    if (false == IsConversationConnected())
                    {
                        ToClosedState();
                    }
                    break;

                case ConversationState::Failed:
                case ConversationState::Closed:
                case ConversationState::Opening:
                case ConversationState::CreatedOrJoined:
                case ConversationState::Open:
                    // nothing to do here
                    break;
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerConnected(shared_ptr<ISpxConnectionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            bool wasConnected = m_recognizerConnected.exchange(true);
            CT_GET_AND_LOG_STATE("Recognizer connected. Was connected: %d, Session ID: %ls", wasConnected, args->GetSessionId().c_str());

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Initial:
                case ConversationState::Closed:
                case ConversationState::CreatingOrJoining:
                case ConversationState::CreatedOrJoined:
                case ConversationState::Opening:
                    CT_I_LOG_WARNING("Not expected");
                    break;

                case ConversationState::Closing:
                    if (!IsConversationConnected())
                    {
                        ToClosedState();
                    }
                    break;

                case ConversationState::Open:
                    // This is normal. nothing to do here
                    break;
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerDisconnected(shared_ptr<ISpxConnectionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            bool wasConnected = m_recognizerConnected.exchange(false);
            CT_GET_AND_LOG_STATE("Recognizer disconnected. Was connected: %d, Session ID: %ls", wasConnected, args->GetSessionId().c_str());

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Initial:
                case ConversationState::CreatingOrJoining:
                    CT_I_LOG_WARNING("Not expected");
                    break;

                case ConversationState::Closing:
                    // If the conversation connection is also closed, we should transition to
                    // the closed state. Otherwise we wait for that to trigger the closed state
                    if (false == IsConversationConnected())
                    {
                        ToClosedState();
                    }
                    break;

                case ConversationState::Failed:
                case ConversationState::Closed:
                case ConversationState::Opening:
                case ConversationState::CreatedOrJoined:
                case ConversationState::Open:
                    // nothing to do here
                    break;
            }
        });
    }

    void CSpxConversationTranslator::OnRecognizerResult(shared_ptr<ISpxRecognitionEventArgs> args)
    {
        RunAsynchronously([this, args]()
        {
            if (args == nullptr)
            {
                CT_I_LOG_ERROR("Recognizer result with null argument");
                return;
            }

            shared_ptr<ISpxRecognitionResult> result = args->GetResult();
            if (result == nullptr)
            {
                CT_I_LOG_ERROR("Recognizer result with null argument result");
                return;
            }

            CT_GET_AND_LOG_STATE(
                "Recognizer result. Reason: %d, %zu chars, NoMatchReason: %d, Offset: %llu, CancellationError: %d",
                result->GetReason(), result->GetText().length(), result->GetNoMatchReason(), result->GetOffset(), result->GetCancellationErrorCode());


            if (!IsConsideredOpen(state))
            {
                CT_I_LOG_WARNING("Got a recognizer speech recognition event when state is not considered open.");

                // though not expected, we should err on the side of caution and try to raise the event
                // anyway in case we ran into a bug in our state machine. That is better than losing
                // events
            }

            bool isFinal = true;
            ResultReason reason;

            // TODO suppress empty finals without intermediate non-empty partials?

            switch (result->GetReason())
            {
                // normally we expect a TranslatingSpeech or TranslatedSpeech event. If we get a RecognizedSpeech event
                // instead that means that the translation failed and so we need to log the failure reason
                case ResultReason::RecognizedSpeech:
                    LogTranslationError(result);
                    // fall through
                case ResultReason::TranslatedSpeech:
                    isFinal = true;
                    reason = ResultReason::TranslatedSpeech;
                    break;

                case ResultReason::RecognizingSpeech:
                    LogTranslationError(result);
                    // fall through
                case ResultReason::TranslatingSpeech:
                    isFinal = false;
                    reason = ResultReason::TranslatingSpeech;
                    break;

                default:
                    CT_I_LOG_WARNING("Speech recognition event with an unknown reason %d.", result->GetReason());
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
            CT_DBG_TRACE_FUNCTION();

            bool wasConnected = m_recognizerConnected.exchange(false);

            bool sendCanceledEvent = false;
            shared_ptr<ISpxRecognitionResult> result;

            if (args == nullptr)
            {
                CT_I_LOG_ERROR("Recognizer canceled with null argument");
                return;
            }
            else if ((result = args->GetResult()) == nullptr)
            {
                CT_I_LOG_ERROR("Recognizer canceled with null argument result");
                return;
            }
            else if (result->GetReason() != ResultReason::Canceled)
            {
                CT_I_LOG_ERROR("Recognizer canceled with non-canceled result reason: %d", result->GetReason());
                return;
            }

            CT_GET_AND_LOG_STATE(
                "Recognizer cancelled. Reason: %d, CancellationReason: %d, CancellationError: %d, WasConnected: %d",
                result->GetReason(), result->GetCancellationReason(), result->GetCancellationErrorCode(), wasConnected);

            std::string errorDetails;
            CancellationErrorCode errorCode = result->GetCancellationErrorCode();
            auto namedProperties = args->QueryInterface<ISpxNamedProperties>();
            if (namedProperties != nullptr)
            {
                errorDetails = NamedPropertiesHelper::GetString(namedProperties, PropertyId::SpeechServiceResponse_JsonErrorDetails);
            }

            switch (state)
            {
                default:
                    CT_HANDLE_UNSUPPORTED_STATE();
                    return;

                case ConversationState::Failed:
                case ConversationState::Initial:
                case ConversationState::Closed:
                case ConversationState::CreatingOrJoining:
                    CT_I_LOG_WARNING("Not expected");
                    break;

                case ConversationState::CreatedOrJoined:
                case ConversationState::Opening:
                    // not really expecting events here but send it through anyway
                    sendCanceledEvent = true;
                    break;

                case ConversationState::Open:
                    sendCanceledEvent = true;
                    break;

                case ConversationState::Closing:
                    // treat as recognizer disconnected
                    if (false == IsConversationConnected())
                    {
                        ToClosedState();
                    }
                    break;
            }

            if (sendCanceledEvent)
            {
                switch (result->GetCancellationReason())
                {
                    case CancellationReason::EndOfStream:
                    {
                        CT_I_LOG_INFO("Got an end of audio stream event from the recognizer");
                        break;
                    }

                    case CancellationReason::Error:
                    {
                        CT_I_LOG_ERROR(
                            "Got an error canceled event from the recognizer. ErrorCode: %d, ErrorDetails: '%s'",
                            result->GetCancellationErrorCode(), errorDetails.c_str());
                        break;
                    }

                    default:
                        CT_I_LOG_ERROR(
                            "Got an error canceled event with an unknown cancellation reason: %d", result->GetCancellationReason());
                        break;
                }

                auto eventArgs = CreateShared<ConversationCancellationResult>(
                    GetParticipantId(), result->GetCancellationReason(), errorCode);
                if (!errorDetails.empty())
                {
                    eventArgs->SetCancellationErrorDetails(errorDetails);
                }

                RaiseEvent<ConversationTranslationEventArgs>(Canceled, GetSessionId(), eventArgs);
            }
        });
    }

    void CSpxConversationTranslator::LogTranslationError(shared_ptr<ISpxRecognitionResult> recoResult)
    {
        try
        {
            // let's log why the translation failed
            string translateError;
            auto namedProps = recoResult->QueryInterface<ISpxNamedProperties>();
            if (namedProps != nullptr)
            {
                translateError = NamedPropertiesHelper::GetString(namedProps, PropertyId::SpeechServiceResponse_JsonErrorDetails);
            }

            CT_I_LOG_ERROR("Translation failed. Reason: '%s'", translateError.c_str());
        }
        catch (const std::exception& ex)
        {
            CT_I_LOG_ERROR("Translation failed. Also failed to retrieve error due to '%s'", ex.what());
        }
    }

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
