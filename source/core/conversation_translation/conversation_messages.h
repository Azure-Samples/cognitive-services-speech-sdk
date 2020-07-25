//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <string>
#include <json.h>
#include <azure_c_shared_utility_macro_utils.h>
#include <exception.h>
#include <speechapi_cxx_common.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    using json = nlohmann::json;
    using namespace std;


    #define _MESSAGE_TYPE_ENUM_VALUES \
        Unknown, \
        Info, \
        Command, \
        ParticipantCommand, \
        Partial, \
        Final, \
        TranslatedMessage, \
        InstantMessage
    /// <summary>
    /// The types of conversation messages
    /// </summary>
    DEFINE_ENUM(MessageType, _MESSAGE_TYPE_ENUM_VALUES)


    #define _COMMAND_TYPE_ENUM_VALUES \
        JoinSession, \
        LeaveSession, \
        DisconnectSession
    /// <summary>
    /// The types of conversation command messages
    /// </summary>
    DEFINE_ENUM(CommandType, _COMMAND_TYPE_ENUM_VALUES)


    #define _PARTICIPANT_COMMAND_TYPE_ENUM_VALUES \
        SetMute, \
        SetMuteAll, \
        SetUseTTS, \
        RoomExpirationWarning, \
        SetLockState, \
        EjectParticipant, \
        Joined, \
        Left, \
        ChangeNickname
    /// <summary>
    /// The types of conversation participant command messages
    /// </summary>
    DEFINE_ENUM(ParticipantCommandType, _PARTICIPANT_COMMAND_TYPE_ENUM_VALUES)

    /// <summary>
    /// The value in a participant command conversation message
    /// </summary>
    struct ParticipantCommandValue
    {
        /// <summary>
        /// Default constructor
        /// </summary>
        ParticipantCommandValue();

        /// <summary>
        /// Creates a boolean participant value
        /// </summary>
        /// <param name="val">The value</param>
        ParticipantCommandValue(bool val);

        /// <summary>
        /// Creates an integer participant value
        /// </summary>
        /// <param name="val">The value</param>
        ParticipantCommandValue(int32_t val);

        /// <summary>
        /// Creates a string participant value
        /// </summary>
        /// <param name="val">The value</param>
        ParticipantCommandValue(const string& val);

        /// <summary>
        /// Creates a string participant value
        /// </summary>
        /// <param name="val">The value</param>
        /// <param name="length">The length of the string</param>
        ParticipantCommandValue(const char * val, const size_t length);

        /// <summary>
        /// Move constructor
        /// </summary>
        /// <param name="other">The other object</param>
        ParticipantCommandValue(ParticipantCommandValue&& other);

        /// <summary>
        /// Destructor
        /// </summary>
        ~ParticipantCommandValue();

        /// <summary>
        /// Move assignment operator
        /// </summary>
        /// <param name="other">The other object</param>
        /// <returns>This</returns>
        ParticipantCommandValue& operator=(ParticipantCommandValue&& other);

        /// <summary>
        /// Gets the boolean value. Will throw an exception if the value is a different type
        /// </summary>
        bool BoolValue() const;

        /// <summary>
        /// Gets the integer value. Will throw an exception if the value is a different type
        /// </summary>
        int32_t IntValue() const;

        /// <summary>
        /// Gets the string value. Will throw an exception if the value is a different type
        /// </summary>
        const char * StringVal() const;

        /// <summary>
        /// Serialises the value
        /// </summary>
        /// <param name="json">The JSON object to serialise to</param>
        /// <param name="key">The key to serialise to in the JSON object</param>
        void Serialize(json & json, const char* key) const;

    private:
        enum class ValueType : uint8_t
        {
            Unknown,
            Boolean,
            Integer,
            String,
        };

        inline size_t ValidateAndReturn(ValueType expected) const
        {
            if (expected != m_type)
            {
                ThrowLogicError("Incorrect type. Expected: "
                    + to_string(static_cast<uint8_t>(expected))
                    + ". Got:"
                    + to_string(static_cast<uint8_t>(m_val)));
            }

            return m_val;
        }

    private:
        // Prevent copying to avoid duplicating memory unnecessarily
        ParticipantCommandValue(const ParticipantCommandValue&) = delete;
        ParticipantCommandValue& operator=(const ParticipantCommandValue&) = delete;

        void Free();

    private:
        ValueType m_type;
        size_t m_val;
    };

    /// <summary>
    /// The base class for all conversation web socket messages
    /// </summary>
    struct ConversationMessageBase
    {
        /// <summary>
        /// Initializes a new instance
        /// </summary>
        /// <param name="conversationId">The identifier for the conversation</param>
        /// <param name="type">The type of the message</param>
        /// <returns></returns>
        ConversationMessageBase(const string& conversationId, const MessageType type);

        /// <summary>
        /// Type destructor
        /// </summary>
        virtual ~ConversationMessageBase() = default;

        /// <summary>
        /// The identifier for the conversation
        /// </summary>
        string ConversationId;

        /// <summary>
        /// The type of message. You can use this to determine what child class to cast this into
        /// </summary>
        MessageType Type;

        /// <summary>
        /// Attempts to parse the JSON string into an instance of one this classes's
        /// children
        /// </summary>
        /// <param name="jsonString">The string to parse</param>
        /// <returns>The parsed object instance, or nullptr in the case of errors</returns>
        static unique_ptr<ConversationMessageBase> ParseJsonString(const string& jsonString);

        /// <summary>
        /// Serialises the current instance into a JSON string
        /// </summary>
        /// <returns>The JSON string. Will return an empty string in the case of errors</returns>
        string ToJsonString() const;

    protected:
        /// <summary>
        /// Default constructor
        /// </summary>
        ConversationMessageBase() = default;

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <param name="ptr">The reference to the unique pointer we are returning. This can be
        /// helpful in cases you determine you need a more specific child instance while parsing</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json, unique_ptr<ConversationMessageBase>& ptr)
        {
            (void)ptr;
            return TryParse(json);
        }

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json);

        /// <summary>
        /// Attempts to serialise the current instance to the intermediate JSON object
        /// </summary>
        /// <param name="json">The JSON instance to serialise to</param>
        virtual void Serialize(json& json) const;
    };

    /// <summary>
    /// The base class for conversation messages that have a nickname, participant ID, and (optionally) an avatar
    /// </summary>
    struct ConversationNicknameMessageBase : public ConversationMessageBase
    {
        /// <summary>
        /// Creates new instance
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="type">The message type</param>
        /// <param name="nickname">The user's nickname</param>
        /// <param name="participantId">The user's unique identifier</param>
        ConversationNicknameMessageBase(const string& conversationId, const MessageType type, const string& nickname, const string& participantId);

        /// <summary>
        /// Creates new instance
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="type">The message type</param>
        /// <param name="nickname">The user's nickname</param>
        /// <param name="participantId">The user's unique identifier</param>
        /// <param name="avatar">The user's avatar colour</param>
        ConversationNicknameMessageBase(const string& conversationId, const MessageType type, const string& nickname, const string& participantId, const string& avatar);

        /// <summary>
        /// The participant's nickname. Please note that there may be more than one participant with
        /// the same nickname. You can use the <see cref="ParticipantId"/> to tell them apart
        /// </summary>
        string Nickname;

        /// <summary>
        /// The unique identifier for the participant. This will be a GUID
        /// </summary>
        string ParticipantId;

        /// <summary>
        /// The colour of the user's avatar as an HTML hex string (e.g. FF0000 for red)
        /// </summary>
        string Avatar;

    protected:
        /// <summary>
        /// Default constructor
        /// </summary>
        ConversationNicknameMessageBase() = default;

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;

        /// <summary>
        /// Attempts to serialise the current instance to the intermediate JSON object
        /// </summary>
        /// <param name="json">The JSON instance to serialise to</param>
        virtual void Serialize(json& json) const override;
    };

    /// <summary>
    /// A conversation command message used to indicate e.g. if a user has been joined the conversation
    /// </summary>
    struct ConversationCommandMessage : public ConversationNicknameMessageBase
    {
        /// <summary>
        /// True if the command is on behalf of the host. False otherwise
        /// </summary>
        bool IsHost;

        /// <summary>
        ///Whether or not the participant is muted
        /// </summary>
        bool IsMuted;

        /// <summary>
        /// The speech or text language code the participant is speaking
        /// </summary>
        string Locale;

        /// <summary>
        /// Whether or not the participant has TTS enabled
        /// </summary>
        bool UseTTS;

        /// <summary>
        /// The type of the command
        /// </summary>
        CommandType Command;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;
    };

    /// <summary>
    /// A conversation participant command message. This can be used by clients to indicate if they
    /// are e.g. muting. A host can also use these messages to e.g. mute all other participants
    /// </summary>
    struct ConversationParticipantCommandMessage : public ConversationNicknameMessageBase
    {
        /// <summary>
        /// Default constructor
        /// </summary>
        ConversationParticipantCommandMessage() = default;

        /// <summary>
        /// Creates a new instance for participant commands with a boolean value (e.g. mute/unmute)
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="nickname">The user's nickname</param>
        /// <param name="participantId">The user's unique identifier</param>
        /// <param name="command">The participant command</param>
        /// <param name="value">The value for the command</param>
        ConversationParticipantCommandMessage(const string& conversationId, const string& nickname, const string& participantId, ParticipantCommandType command, bool value);

        /// <summary>
        /// Creates a new instance for participant commands with an integer value (e.g. room expiration warning)
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="nickname">The user's nickname</param>
        /// <param name="participantId">The user's unique identifier</param>
        /// <param name="command">The participant command</param>
        /// <param name="value">The value for the command</param>
        ConversationParticipantCommandMessage(const string& conversationId, const string& nickname, const string& participantId, ParticipantCommandType command, int32_t value);

        /// <summary>
        /// Creates a new instance for participant commands with a string value (e.g. ejecting a user from a conversation)
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="nickname">The user's nickname</param>
        /// <param name="participantId">The user's unique identifier</param>
        /// <param name="command">The participant command</param>
        /// <param name="value">The value for the command</param>
        ConversationParticipantCommandMessage(const string& conversationId, const string& nickname, const string& participantId, ParticipantCommandType command, const std::string& value);

        /// <summary>
        /// Creates a new instance for participant commands with a string value (e.g. ejecting a user from a conversation)
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="nickname">The user's nickname</param>
        /// <param name="participantId">The user's unique identifier</param>
        /// <param name="command">The participant command</param>
        /// <param name="value">The value for the command</param>
        /// <param name="valueLength">The length of the string value</param>
        ConversationParticipantCommandMessage(const string& conversationId, const string& nickname, const string& participantId, ParticipantCommandType command, const char * value, const size_t valueLength);

        /// <summary>
        /// The type of the participant command we are issuing
        /// </summary>
        ParticipantCommandType Command;

        /// <summary>
        /// The value for the participant command
        /// </summary>
        ParticipantCommandValue Value;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;

        /// <summary>
        /// Attempts to serialise the current instance to the intermediate JSON object
        /// </summary>
        /// <param name="json">The JSON instance to serialise to</param>
        virtual void Serialize(json& json) const override;
    };

    /// <summary>
    /// A conversation informational message
    /// </summary>
    struct ConversationInfoMessage : public ConversationMessageBase
    {
        /// <summary>
        /// Default constructor
        /// </summary>
        ConversationInfoMessage() = default;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="command">The info command</param>
        ConversationInfoMessage(const string& conversationId, const string& command);

        /// <summary>
        /// The informational command
        /// </summary>
        string Command;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <param name="ptr">The reference to the unique pointer we are returning. This can be
        /// helpful in cases you determine you need a more specific child instance while parsing</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json, unique_ptr<ConversationMessageBase>& ptr) override;

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;

        /// <summary>
        /// Attempts to serialise the current instance to the intermediate JSON object
        /// </summary>
        /// <param name="json">The JSON instance to serialise to</param>
        virtual void Serialize(json& json) const override;
    };

    /// <summary>
    /// A informational message that is broadcast to all participants when the Cognitive Speech
    /// authorization token to use changes. This can also be sent from the host to the Conversation
    /// service to set the new authorization token to use.
    /// </summary>
    struct ConversationAuthorizationTokenMessage : public ConversationInfoMessage
    {
        /// <summary>
        /// Default constructor
        /// </summary>
        ConversationAuthorizationTokenMessage() = default;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="authToken">The authentication token to use</param>
        /// <param name="region">The region for this authorization token</param>
        ConversationAuthorizationTokenMessage(const string & authToken, const string & region);

        /// <summary>
        /// The Cognitive Services Speech authorization token
        /// </summary>
        string AuthToken;

        /// <summary>
        /// The Azure region for this authorization token
        /// </summary>
        string Region;

        /// <summary>
        /// The local time at which the authorization token becomes invalid
        /// </summary>
        std::chrono::system_clock::time_point ValidUntil;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <param name="ptr">The reference to the unique pointer we are returning. This can be
        /// helpful in cases you determine you need a more specific child instance while parsing</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json, unique_ptr<ConversationMessageBase>& ptr) override
        {
            (void)ptr;
            return TryParse(json);
        }

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;

        /// <summary>
        /// Attempts to serialise the current instance to the intermediate JSON object
        /// </summary>
        /// <param name="json">The JSON instance to serialise to</param>
        virtual void Serialize(json& json) const override;
    };

    /// <summary>
    /// Information about a conversation participant
    /// </summary>
    struct ConversationParticipant
    {
        virtual ~ConversationParticipant() = default;

        /// <summary>
        /// The participant's nickname. Please note that there may be more than one participant with
        /// the same nickname. You can use the <see cref="ParticipantId"/> to tell them apart
        /// </summary>
        string Nickname;

        /// <summary>
        /// The unique identifier for the participant. This will be a GUID
        /// </summary>
        string ParticipantId;

        /// <summary>
        /// The colour of the user's avatar as an HTML hex string (e.g. FF0000 for red)
        /// </summary>
        string Avatar;

        /// <summary>
        /// The code for the language the participant is speaking (e.g. en-US), OR the
        /// code for the participant's text language (e.g. en)
        /// the participant
        /// </summary>
        string Locale;

        /// <summary>
        /// True if this participant has enabled TTS, false otherwise
        /// </summary>
        bool UseTTS;

        /// <summary>
        /// True if this participant is muted, false otherwise
        /// </summary>
        bool IsMuted;

        /// <summary>
        /// True if this participant is the host, false otherwise
        /// </summary>
        bool IsHost;

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json);
    };

    /// <summary>
    /// The conversation message that lists all the participants in the room (including yourself).
    /// This is always the first message sent at the start of the web socket connection from
    /// the server.
    /// </summary>
    struct ConversationParticipantListMessage : public ConversationInfoMessage
    {
        /// <summary>
        /// The list of participants in the room
        /// </summary>
        vector<ConversationParticipant> Participants;

        /// <summary>
        /// (Optional) The conversation token to use for re-authentication
        /// </summary>
        string Token;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <param name="ptr">The reference to the unique pointer we are returning. This can be
        /// helpful in cases you determine you need a more specific child instance while parsing</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json, unique_ptr<ConversationMessageBase>& ptr) override
        {
            (void)ptr;
            return TryParse(json);
        }

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;
    };

    /// <summary>
    /// Base class for recognitions or translations
    /// </summary>
    struct ConversationRecognitionBase : public ConversationNicknameMessageBase
    {
        /// <summary>
        /// The unique identifier for the recognition. This will be the same across all
        /// partials, and the final for speech recognitions
        /// </summary>
        string Id;

        /// <summary>
        /// The time stamp for the recognition
        /// </summary>
        string Timestamp; // TODO ralphe: use proper time type?

        /// <summary>
        /// The original language of this recognition/text
        /// </summary>
        string OriginalLanguage;

        /// <summary>
        /// The translations for this recognition/text. The key is the language,
        /// the value is the translation
        /// </summary>
        map<string, string> Translations;

    protected:
        /// <summary>
        /// Default constructor
        /// </summary>
        ConversationRecognitionBase() = default;

        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;
    };

    /// <summary>
    /// A conversation speech recognition. This can be a partial (we are still recognising the utterance
    /// and it will change as we go), or a final (we are done recognising the utterance).
    /// </summary>
    struct ConversationSpeechRecognitionMessage : public ConversationRecognitionBase
    {
        /// <summary>
        /// The speech recognition
        /// </summary>
        string Recognition;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;
    };

    /// <summary>
    /// An instant message sent from you or another participant
    /// </summary>
    struct ConversationTranslatedMessage : public ConversationRecognitionBase
    {
        /// <summary>
        /// The instant message text
        /// </summary>
        string Text;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;
    };

    /// <summary>
    /// An instant message that you are sending to the room
    /// </summary>
    struct ConversationInstantMessage : public ConversationNicknameMessageBase
    {
        /// <summary>
        /// Default constructor
        /// </summary>
        ConversationInstantMessage() = default;

        /// <summary>
        /// Creates an instance
        /// </summary>
        /// <param name="conversationId">The conversation identifier</param>
        /// <param name="nickname">The user's nickname</param>
        /// <param name="participantId">The user's unique identifier</param>
        /// <param name="text">The instant message text</param>
        ConversationInstantMessage(const string& conversationId, const string& nickname, const string& participantId, const string& text);

        /// <summary>
        /// The instant message text
        /// </summary>
        string Text;

    protected:
        /// <summary>
        /// Attempts to parse the JSON object passed in to deserialise into the class properties
        /// </summary>
        /// <param name="json">The JSON to deserialise from</param>
        /// <returns>True if we parsed successfully, false otherwise</returns>
        virtual bool TryParse(json& json) override;

        /// <summary>
        /// Attempts to serialise the current instance to the intermediate JSON object
        /// </summary>
        /// <param name="json">The JSON instance to serialise to</param>
        virtual void Serialize(json& json) const override;
    };

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
