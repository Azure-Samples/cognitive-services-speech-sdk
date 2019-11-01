//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include "ISpxConversationInterfaces.h"
#include <string_utils.h>
#include <exception.h>
#include "conversation_connection.h"

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4250 )
// Disable C4250 warning that complains about a parent class implementing a pure abstract method
// from a grandparent class thereby becoming the dominant one in the virtual inheritance hierarchy.
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    using namespace std;
    using ParticipantChangedReason = Transcription::ParticipantChangedReason;

    class CSpxConversationParticipant :
        public virtual ISpxConversationParticipant,
        public ConversationParticipant
    {
    public:
        CSpxConversationParticipant() = default;
        CSpxConversationParticipant(const ConversationParticipant& part)
            : ConversationParticipant(part)
        {
        }
        CSpxConversationParticipant(ConversationParticipant&& part)
            : ConversationParticipant(move(part))
        {
        }

        virtual void SetPreferredLanguage(std::string && preferredLanguage) override
        {
            (void)preferredLanguage;
            Impl::ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR);
        }

        virtual void SetVoiceSignature(std::string && voiceSignature) override
        {
            (void)voiceSignature;
            Impl::ThrowWithCallstack(SPXERR_UNSUPPORTED_API_ERROR);
        }

        virtual std::string GetPreferredLanguage() const override { return Locale; }
        virtual std::string GetVoiceSignature() const override { return string(); }
        virtual std::string GetId() const override { return ParticipantId; }
        virtual std::string GetAvatar() const override { return Avatar; }
        virtual std::string GetDisplayName() const override { return Nickname; }
        virtual bool IsUsingTts() const override { return UseTTS; }
        virtual bool IsMuted() const override { return ConversationParticipant::IsMuted; }
        virtual bool IsHost() const override { return ConversationParticipant::IsHost; }
    };

    class ConversationRecognitionResult :
        public virtual ISpxConversationTranslationResult,
        public ISpxPropertyBagImpl
    {
    protected:
        wstring resultId;
        wstring text;
        wstring originalLang;
        ResultReason reason;
        wstring participantId;
        map<wstring, wstring> translations;
        NoMatchReason noMatchReason;
        uint64_t offset;
        uint64_t duration;

    public:
        ConversationRecognitionResult() = default;

        ConversationRecognitionResult(
            const string& id,
            const string& text,
            const string& originalLang,
            ResultReason reason,
            const string& participantId,
            uint64_t offset = 0,
            uint64_t duration = 0
        ) :
            resultId(PAL::ToWString(id)),
            text(PAL::ToWString(text)),
            originalLang(PAL::ToWString(originalLang)),
            reason(reason),
            participantId(PAL::ToWString(participantId)),
            translations(),
            noMatchReason(),
            offset(offset),
            duration(duration)
        {
        }

        ConversationRecognitionResult(const string& id, NoMatchReason reason, const string& participantId) :
            ConversationRecognitionResult(id, "", "", ResultReason::NoMatch, participantId)
        {
            noMatchReason = reason;
        }

        SPX_INTERFACE_MAP_BEGIN()
            SPX_INTERFACE_MAP_ENTRY(ISpxRecognitionResult)
            SPX_INTERFACE_MAP_ENTRY(ISpxTranslationRecognitionResult)
            SPX_INTERFACE_MAP_ENTRY(ISpxConversationTranscriptionResult)
            SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_END()

        void AddTranslation(const string& lang, const string& translation)
        {
            translations[PAL::ToWString(lang)] = PAL::ToWString(translation);
        }

        void AddTranslation(const wstring& lang, const wstring& translation)
        {
            translations[lang] = translation;
        }

        // Inherited via ISpxConversationTranslationResult
        virtual std::wstring GetResultId() override { return resultId; }
        virtual std::wstring GetText() override { return text; }
        virtual ResultReason GetReason() override { return reason; }
        virtual std::wstring GetUserId() override { return participantId; }
        virtual std::wstring GetOriginalLanguage() const override { return originalLang; }
        virtual const std::map<std::wstring, std::wstring>& GetTranslationText() override { return translations; }
        virtual uint64_t GetOffset() const override { return offset; }
        virtual void SetOffset(uint64_t o) override { offset = o; }
        virtual uint64_t GetDuration() const override { return duration; }
        virtual NoMatchReason GetNoMatchReason() override { return noMatchReason; }
        virtual void SetLatency(uint64_t) override {}
        virtual CancellationReason GetCancellationReason() override { return static_cast<CancellationReason>(0); }
        virtual CancellationErrorCode GetCancellationErrorCode() override { return CancellationErrorCode::NoError; }
    };

    class ConversationCancellationResult : public ConversationRecognitionResult
    {
    protected:
        CancellationReason cancelReason;
        CancellationErrorCode cancelError;

    public:
        ConversationCancellationResult() = default;
        ConversationCancellationResult(const string& participantId, CancellationReason reason, CancellationErrorCode error) :
            ConversationRecognitionResult("", "", "", ResultReason::Canceled, participantId),
            cancelReason(reason),
            cancelError(error)
        {
        }

        virtual CancellationReason GetCancellationReason() override { return cancelReason; }
        virtual CancellationErrorCode GetCancellationErrorCode() override { return cancelError; }

        virtual void SetCancellationErrorDetails(const std::string& details)
        {
            return SetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), details.c_str());
        }
        virtual std::string GetCancellationErrorDetails()
        {
            return GetStringValue(GetPropertyName(PropertyId::SpeechServiceResponse_JsonErrorDetails), "");
        }
    };

    class ConversationSessionEventArgs : public virtual ISpxConversationSessionEventArgs
    {
    protected:
        wstring sessionId;

    public:
        ConversationSessionEventArgs() = default;

        ConversationSessionEventArgs(ConversationSessionEventArgs& other)
            : ISpxConversationSessionEventArgs(), sessionId(other.sessionId)
        {}

        ConversationSessionEventArgs(const string& sessionId) : sessionId(PAL::ToWString(sessionId))
        { }

        // Inherited via ISpxSessionEventArgs
        virtual const wstring & GetSessionId() override { return sessionId; }
    };

    class ConversationConnectionEventArgs : public ConversationSessionEventArgs, public ISpxConversationConnectionEventArgs
    {
    public:
        ConversationConnectionEventArgs() = default;
        ConversationConnectionEventArgs(const string& sessionId) : ConversationSessionEventArgs(sessionId)
        {
        }

        // Inherited via ISpxConversationConnectionEventArgs
        virtual const wstring & GetSessionId() override { return sessionId; }
    };

    class ConversationExpirationEventArgs : public ConversationSessionEventArgs, public ISpxConversationExpirationEventArgs
    {
    protected:
        int32_t minutesLeft;

    public:
        ConversationExpirationEventArgs() = default;
        ConversationExpirationEventArgs(const std::string& sessionId, int32_t minutes)
            : ConversationSessionEventArgs(sessionId), minutesLeft(minutes)
        { }

        // Inherited via ISpxConversationExpirationEventArgs
        virtual int32_t GetMinutesLeft() const override { return minutesLeft; }
    };

    class ConversationParticipantChangedEventArgs : public ConversationSessionEventArgs, public ISpxConversationParticipantChangedEventArgs
    {
    protected:
        ParticipantChangedReason reason;
        vector<shared_ptr<ISpxConversationParticipant>> participants;

    public:
        ConversationParticipantChangedEventArgs() = default;
        ConversationParticipantChangedEventArgs(const std::string& sessionId, ParticipantChangedReason reason)
            : ConversationSessionEventArgs(sessionId), reason(reason)
        { }

        void AddParticipant(shared_ptr<ISpxConversationParticipant> participant)
        {
            participants.push_back(participant);
        }

        // Inherited via ISpxConversationParticipantChangedEventArgs
        virtual ParticipantChangedReason GetReason() const override { return reason; }
        virtual const vector<shared_ptr<ISpxConversationParticipant>>& GetParticipants() const override { return participants; }
    };

    class ConversationTranslationEventArgs : public ConversationSessionEventArgs, public ISpxConversationTranslationEventArgs
    {
    protected:
        std::shared_ptr<ConversationRecognitionResult> result;
        uint64_t offset;

    public:
        ConversationTranslationEventArgs() = default;

        ConversationTranslationEventArgs(const std::string& sessionId, std::shared_ptr<ConversationRecognitionResult> result) :
            ConversationSessionEventArgs(sessionId), result(result), offset()
        {
        }

        // Inherited via ISpxConversationTranslationEventArgs
        virtual const wstring & GetSessionId() override { return sessionId; }
        virtual const uint64_t & GetOffset() override
        {
            if (result != nullptr)
            {
                offset = result->GetOffset();
            }

            return offset;
        }
        virtual std::shared_ptr<ISpxRecognitionResult> GetResult() override { return result; }
        virtual std::shared_ptr<ISpxConversationTranslationResult> GetConversationTranslationResult() const override { return result; }
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation

#ifdef _MSC_VER
#pragma warning( pop )
#endif
