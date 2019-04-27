
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
// speechapi_cxx_conversation_transcriber.h: Public API declarations for ConversationTranscriber C++ class
//

#pragma once
#include <exception>
#include <future>
#include <memory>
#include <string>
#include <cstring>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include <speechapi_c.h>
#include <speechapi_cxx_recognition_async_recognizer.h>
#include <speechapi_cxx_conversation_transcription_result.h>
#include <speechapi_cxx_conversation_transcription_eventargs.h>
#include <speechapi_cxx_properties.h>
#include <speechapi_cxx_speech_config.h>
#include <speechapi_cxx_audio_stream.h>

#include <speechapi_cxx_user.h>
#include <speechapi_cxx_participant.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Conversation {

class Session;
constexpr size_t MAX_CONVERSATION_ID_LEN = 1024;
/// <summary>
/// Class for conversation transcriber.
/// Note: This is only available on the Cognitive Services Speech Devices SDK to optimize the experience for multi-microphone devices.
/// Added in version 1.5.0.
/// </summary>
class ConversationTranscriber : public AsyncRecognizer<ConversationTranscriptionResult, ConversationTranscriptionEventArgs, ConversationTranscriptionCanceledEventArgs>
{
public:

    using BaseType = AsyncRecognizer<ConversationTranscriptionResult, ConversationTranscriptionEventArgs, ConversationTranscriptionCanceledEventArgs>;

    /// <summary>
    /// Create a conversation transcriber from a speech config and audio config.
    /// </summary>
    /// <param name="speechconfig">Speech configuration.</param>
    /// <param name="audioInput">Audio configuration.</param>
    /// <returns>A smart pointer wrapped conversation transcriber pointer.</returns>
    static std::shared_ptr<ConversationTranscriber> FromConfig(std::shared_ptr<SpeechConfig> speechconfig, std::shared_ptr<Audio::AudioConfig> audioInput = nullptr)
    {
        SPXRECOHANDLE hreco;
        SPX_THROW_ON_FAIL(::recognizer_create_conversation_transcriber_from_config(
            &hreco,
            HandleOrInvalid<SPXSPEECHCONFIGHANDLE,SpeechConfig>(speechconfig),
            HandleOrInvalid<SPXAUDIOCONFIGHANDLE, Audio::AudioConfig>(audioInput)));
        return std::make_shared<ConversationTranscriber>(hreco);
    }

    /// <summary>
    /// Internal constructor. Creates a new instance using the provided handle.
    /// </summary>
    /// <param name="hreco">Recognizer handle.</param>
    explicit ConversationTranscriber(SPXRECOHANDLE hreco) : BaseType(hreco), Properties(m_properties)
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~ConversationTranscriber()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        TermRecognizer();
    }

    /// <summary>
    /// Set the id of a conversation.
    /// </summary>
    /// <param name="conversationId">A conversation id.</param>
    /// <returns>void.</returns>
    void SetConversationId(const SPXSTRING& conversationId)
    {
        SPX_THROW_ON_FAIL(conversation_transcriber_set_conversation_id(m_hreco, Utils::ToUTF8(conversationId.c_str())));
    }

    /// <summary>
    /// Get the id of a conversation.
    /// </summary>
    /// <returns>A conversation id.</returns>
    SPXSTRING GetConversationId()
    {
        char id[MAX_CONVERSATION_ID_LEN+1];
        std::memset(id, 0, MAX_CONVERSATION_ID_LEN);

        SPX_THROW_ON_FAIL(conversation_transcriber_get_conversation_id(m_hreco, id, MAX_CONVERSATION_ID_LEN));

        return id;
    }

    /// <summary>
    /// Asynchronously starts a conversation transcribing.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StartTranscribingAsync()
    {
        return StartContinuousRecognitionAsync();
    }

    /// <summary>
    /// Asynchronously stops a conversation transcribing.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StopTranscribingAsync()
    {
        return StopContinuousRecognitionAsync();
    }

    /// <summary>
    /// Asynchronously ends a conversation.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> EndConversationAsync()
    {
        auto keepAlive = this->shared_from_this();
        auto future = std::async(std::launch::async, [keepAlive, this]() -> void {
            SPX_THROW_ON_FAIL(conversation_transcriber_end_conversation(m_hreco));
        });
        return future;
    }

    /// <summary>
    /// Add a participant to a conversation using the user's id.
    ///
    // Note: The returned participant can be used to remove. If the client changes the participant's attributes,
    // the changed attributes are passed on to the service only when the participant is added again.
    //
    /// </summary>
    /// <param name="userId">A user id.</param>
    /// <returns>a shared smart pointer of the participant.</returns>
    std::shared_ptr<Participant> AddParticipant(const SPXSTRING& userId)
    {
        const auto participant = Participant::From(userId);
        SPX_THROW_ON_FAIL(conversation_transcriber_update_participant(m_hreco, true, (SPXPARTICIPANTHANDLE)(*participant)));
        return participant;
    }

    /// <summary>
    /// Add a participant to a conversation using the User object.
    /// </summary>
    /// <param name="user">A shared smart pointer to a User object.</param>
    /// <returns>The passed in User object.</returns>
    std::shared_ptr<User> AddParticipant(const std::shared_ptr<User>& user)
    {
        SPX_THROW_ON_FAIL(conversation_transcriber_update_participant_by_user(m_hreco, true, (SPXUSERHANDLE)(*user)));
        return user;
    }

    /// <summary>
    /// Add a participant to a conversation using the participant object
    /// </summary>
    /// <param name="participant">A shared smart pointer to a participant object.</param>
    /// <returns>void.</returns>
    std::shared_ptr<Participant> AddParticipant(const std::shared_ptr<Participant>& participant)
    {
        SPX_THROW_ON_FAIL(conversation_transcriber_update_participant(m_hreco, true, (SPXPARTICIPANTHANDLE)(*participant)));
        return participant;
    }

    /// <summary>
    /// Remove a participant from a conversation using the participant object
    /// </summary>
    /// <param name="participant">A shared smart pointer of a participant object.</param>
    /// <returns>void.</returns>
    void RemoveParticipant(const std::shared_ptr<Participant>& participant)
    {
        SPX_THROW_ON_FAIL(conversation_transcriber_update_participant(m_hreco, false, SPXPARTICIPANTHANDLE(*participant)));
    }

    /// <summary>
    /// Remove a participant from a conversation using the User object
    /// </summary>
    /// <param name="user">A smart pointer of a User.</param>
    /// <returns>void.</returns>
    void RemoveParticipant(const std::shared_ptr<User>& user)
    {
        SPX_THROW_ON_FAIL(conversation_transcriber_update_participant_by_user(m_hreco, false, SPXUSERHANDLE(*user)));
    }

    /// <summary>
    /// Remove a participant from a conversation using a user id object
    /// </summary>
    /// <param name="userId">A user id.</param>
    /// <returns>void.</returns>
    void RemoveParticipant(const SPXSTRING& userId)
    {
        SPX_THROW_ON_FAIL(conversation_transcriber_update_participant_by_user_id(m_hreco, false, Utils::ToUTF8(userId.c_str())));
    }

    /// <summary>
    /// A collection or properties and their values defined for this <see cref="ConversationTranscriber"/>.
    /// </summary>
    PropertyCollection& Properties;

    /// <summary>
    /// Sets the authorization token that will be used for connecting the server.
    /// </summary>
    /// <param name="token">The authorization token.</param>
    void SetAuthorizationToken(const SPXSTRING& token)
    {
        Properties.SetProperty(PropertyId::SpeechServiceAuthorization_Token, token);
    }

    /// <summary>
    /// Gets the authorization token.
    /// </summary>
    /// <returns>Authorization token</returns>
    SPXSTRING GetAuthorizationToken()
    {
        return Properties.GetProperty(PropertyId::SpeechServiceAuthorization_Token, SPXSTRING());
    }

protected:
    /// <summary>
    /// Asynchronously initiates continuous speech recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StartContinuousRecognitionAsync() override
    {
        return BaseType::StartContinuousRecognitionAsyncInternal();
    }

    /// <summary>
    /// Asynchronously terminates ongoing continuous speech recognition operation.
    /// </summary>
    /// <returns>An empty future.</returns>
    std::future<void> StopContinuousRecognitionAsync() override
    {
        return BaseType::StopContinuousRecognitionAsyncInternal();
    }

    /// <summary>
    /// Asynchronously initiates keyword recognition operation.
    /// </summary>
    /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK.This functionality is currently not included in the SDK itself.    /// <param name="model">Specifies the keyword model to be used.</param>
    /// <returns>An empty future.</returns>
    std::future<void> StartKeywordRecognitionAsync(std::shared_ptr<KeywordRecognitionModel> model) override
    {
        std::future<void> result;
        UNUSED(model);
        SPX_THROW_ON_FAIL(SPXERR_UNSUPPORTED_API_ERROR);
        return result;
    }

    /// <summary>
    /// Asynchronously terminates keyword recognition operation.
    /// </summary>
    /// Note: Key word spotting functionality is only available on the Cognitive Services Device SDK.This functionality is currently not included in the SDK itself.
    /// <returns>An empty future.</returns>
    std::future<void> StopKeywordRecognitionAsync() override
    {
        std::future<void> result;
        SPX_THROW_ON_FAIL(SPXERR_UNSUPPORTED_API_ERROR);
        return result;
    }

    /// <summary>
    /// Performs speech recognition in a non-blocking (asynchronous) mode.
    /// Note: RecognizeOnceAsync() returns when the first utterance has been recognized,
    /// so it is suitable only for single shot recognition like command or query.
    /// For long-running recognition, use StartContinuousRecognitionAsync() instead.
    /// </summary>
    /// <returns>Future containing result value (a shared pointer to ConversationTranscriber)
    /// of the asynchronous speech recognition.
    /// </returns>
    std::future<std::shared_ptr<ConversationTranscriptionResult>> RecognizeOnceAsync() override
    {
       std::future<std::shared_ptr<ConversationTranscriptionResult>> result;
       SPX_THROW_ON_FAIL(SPXERR_UNSUPPORTED_API_ERROR);

       return result;
    }

private:
    DISABLE_DEFAULT_CTORS(ConversationTranscriber);
    friend class Microsoft::CognitiveServices::Speech::Session;
};

} } } } // Microsoft::CognitiveServices::Speech::Conversation
