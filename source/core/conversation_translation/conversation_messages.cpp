//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "common.h"
#include <time_utils.h>
#include "spxdebug.h"
#include "conversation_messages.h"
#include "string_utils.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
namespace ConversationTranslation {

    using StringUtils = PAL::StringUtils;

    DEFINE_ENUM_STRINGS(MessageType, _MESSAGE_TYPE_ENUM_VALUES)
    DEFINE_ENUM_STRINGS(CommandType, _COMMAND_TYPE_ENUM_VALUES)
    DEFINE_ENUM_STRINGS(ParticipantCommandType, _PARTICIPANT_COMMAND_TYPE_ENUM_VALUES)

    const char* KEY_TYPE = "type";
    const char* KEY_ROOM_ID = "roomid";
    const char* KEY_NICKNAME = "nickname";
    const char* KEY_PARTICIPANT_ID = "participantId";
    const char* KEY_AVATAR = "avatar";
    const char* KEY_COMMAND = "command";
    const char* KEY_IS_HOST = "ishost";
    const char* KEY_VALUE = "value";
    const char* KEY_TOKEN = "token";
    const char* KEY_PARTICIPANTS = "participants";
    const char* KEY_LOCALE = "locale";
    const char* KEY_USE_TTS = "usetts";
    const char* KEY_IS_MUTED = "ismuted";
    const char* KEY_ID = "id";
    const char* KEY_TIMESTAMP = "timestamp";
    const char* KEY_LANGUAGE = "language";
    const char* KEY_TRANSLATIONS = "translations";
    const char* KEY_TRANSLATION_LANG = "lang";
    const char* KEY_TRANSLATION_TRANSLATION = "translation";
    const char* KEY_RECOGNITION = "recognition";
    const char* KEY_TEXT = "text";
    const char* KEY_REGION = "region";
    const char* KEY_VALID_TO = "validTo";

    /// <summary>
    /// Reads a string value from the specified key in JSON without throwing exceptions on nulls.
    /// </summary>
    /// <param name="json">The JSON object</param>
    /// <param name="key">The key to read</param>
    /// <param name="val">The string to set</param>
    /// <returns>True if there was a value of the correct type at that key, false otherwise</returns>
    static inline bool TryReadString(json& json, const char *key, string& val)
    {
        if (key == nullptr)
        {
            return false;
        }

        auto prop = json[key];
        if (prop.is_string())
        {
            val = prop.get<string>();
            return true;
        }
        else
        {
            return false;
        }
    }

    /// <summary>
    /// Reads a boolean value from the specified key in JSON without throwing exceptions on nulls.
    /// </summary>
    /// <param name="json">The JSON object</param>
    /// <param name="key">The key to read</param>
    /// <param name="val">The boolean to set</param>
    /// <returns>True if there was a value of the correct type at that key, false otherwise</returns>
    static inline bool TryReadBool(json& json, const char *key, bool& b)
    {
        auto prop = json[key];
        if (prop.is_boolean())
        {
            b = prop.get<bool>();
            return true;
        }
        else
        {
            return false;
        }
    }

    /// <summary>
    /// Reads a UTC time string from the specified key in JSON, and parses the string value into
    /// a local time
    /// </summary>
    /// <param name="json">The JSON object</param>
    /// <param name="key">The key to read</param>
    /// <aparm name="format">The expected time format string. Please refer to the std::get_time documentation
    //  for possible values</param>
    /// <param name="time">The local time struct to set</param>
    /// <returns>True if the value existed and we were able to successfully parse it into a local
    /// time, false otherwise</returns>
    static bool TryReadUtcTime(json& json, const char * key, const char* format, std::chrono::system_clock::time_point& time)
    {
        std::string timeString;
        if (!TryReadString(json, key, timeString))
        {
            return false;
        }

        // Attempt to parse the date time string
        return PAL::TryParseUtcTimeString(timeString, format, time);
    }

    ParticipantCommandValue::ParticipantCommandValue()
        : m_type(ValueType::Unknown), m_val()
    {
    }

    ParticipantCommandValue::ParticipantCommandValue(bool val)
        : m_type(ValueType::Boolean), m_val(val ? 1 : 0)
    {
    }

    ParticipantCommandValue::ParticipantCommandValue(int32_t val)
        : m_type(ValueType::Integer), m_val(static_cast<size_t>(val))
    {
    }

    ParticipantCommandValue::ParticipantCommandValue(const string& val)
        : ParticipantCommandValue(val.c_str(), val.length())
    {
    }

    ParticipantCommandValue::ParticipantCommandValue(const char * val, const size_t length)
        : m_type(ValueType::String), m_val(0)
    {
        char *ptr = static_cast<char *>(malloc(length + 1));
        if (ptr == nullptr)
        {
            throw bad_alloc();
        }

        m_val = reinterpret_cast<size_t>(ptr);
        (void)snprintf(ptr, length + 1, "%s", val);
    }

    ParticipantCommandValue::ParticipantCommandValue(ParticipantCommandValue&& other)
        : m_type(ValueType::Unknown), m_val()
    {
        m_type = other.m_type;
        m_val = other.m_val;

        other.m_type = ValueType::Unknown;
        other.m_val = 0;
    }

    ParticipantCommandValue::~ParticipantCommandValue()
    {
        Free();

        m_type = ValueType::Unknown;
        m_val = 0;
    }

    ParticipantCommandValue& ParticipantCommandValue::operator=(ParticipantCommandValue && other)
    {
        if (this != &other)
        {
            Free();

            m_type = other.m_type;
            m_val = other.m_val;

            other.m_type = ValueType::Unknown;
            other.m_val = 0;
        }

        return *this;
    }

    bool ParticipantCommandValue::BoolValue() const
    {
        return ValidateAndReturn(ValueType::Boolean) != 0;
    }

    int32_t ParticipantCommandValue::IntValue() const
    {
        return static_cast<int32_t>(ValidateAndReturn(ValueType::Integer));
    }

    const char * ParticipantCommandValue::StringVal() const
    {
        return reinterpret_cast<const char *>(ValidateAndReturn(ValueType::String));
    }

    void ParticipantCommandValue::Serialize(json & json, const char* key) const
    {
        switch (m_type)
        {
            case ValueType::Boolean:
                json[key] = BoolValue();
                break;

            case ValueType::Integer:
                json[key] = IntValue();
                break;

            case ValueType::String:
                json[key] = StringVal();
                break;

            case ValueType::Unknown:
            default:
                SPX_TRACE_ERROR("Don't know how to serialize value of type '%d'. Skipping...", m_type);
                break;
        }
    }

    void ParticipantCommandValue::Free()
    {
        if (m_type == ValueType::String)
        {
            char *ptr = reinterpret_cast<char *>(m_val);
            if (ptr != nullptr)
            {
                free(ptr);
                m_val = 0;
            }
        }
    }

    ConversationMessageBase::ConversationMessageBase(const string& conversationId, const MessageType type)
        : ConversationId(conversationId), Type(type)
    {
    }

    unique_ptr<ConversationMessageBase> ConversationMessageBase::ParseJsonString(const string & jsonString)
    {
        try
        {
            if (jsonString.length() == 0)
            {
                return nullptr;
            }

            json json = json::parse(jsonString, nullptr, false);
            if (json.is_discarded() || json.is_object() != true)
            {
                SPX_TRACE_ERROR("Could not parse the JSON string OR unexpected JSON value: '%s'", jsonString.c_str());
                return nullptr;
            }

            // inspect the type property in the parsed JSON to determine what kind of message we have
            string msgTypeStr;
            if (!TryReadString(json, KEY_TYPE, msgTypeStr))
            {
                SPX_TRACE_ERROR("JSON does not contain the '%s' key: '%s'", KEY_TYPE, jsonString.c_str());
                return nullptr;
            }

            msgTypeStr = StringUtils::SnakeCaseToPascalCase(msgTypeStr);
            MessageType msgType(MessageType::Unknown);

            if (MessageType_FromString(msgTypeStr.c_str(), &msgType))
            {
                // unknown message type
                SPX_TRACE_ERROR("Unknown message type '%s' in JSON: '%s'", msgTypeStr.c_str(), jsonString.c_str());
                return nullptr;
            }

            unique_ptr<ConversationMessageBase> ptr(nullptr);
            switch (msgType)
            {
                case MessageType::Command:
                    ptr.reset(new ConversationCommandMessage());
                    break;

                case MessageType::Final:
                case MessageType::Partial:
                    ptr.reset(new ConversationSpeechRecognitionMessage());
                    break;

                case MessageType::Info:
                    ptr.reset(new ConversationInfoMessage());
                    break;

                case MessageType::InstantMessage:
                    ptr.reset(new ConversationInstantMessage());
                    break;

                case MessageType::ParticipantCommand:
                    ptr.reset(new ConversationParticipantCommandMessage());
                    break;

                case MessageType::TranslatedMessage:
                    ptr.reset(new ConversationTranslatedMessage());
                    break;

                case MessageType::Unknown:
                default:
                    // don't know how to parse this
                    SPX_TRACE_ERROR("Don't know how to parse '%s' type in JSON: '%s'", msgTypeStr.c_str(), jsonString.c_str());
                    return nullptr;
            }

            SPX_DBG_ASSERT(ptr != nullptr);

            if (ptr != nullptr)
            {
                unique_ptr<ConversationMessageBase> newType(nullptr);
                bool success = ptr->TryParse(json, newType);
                if (!success && newType != nullptr)
                {
                    ptr.reset();
                    ptr.swap(newType);

                    success = ptr->TryParse(json, newType);
                }

                if (success)
                {
                    ptr->Type = msgType;
                    return ptr;
                }
                else
                {
                    SPX_TRACE_ERROR("Could not deserialize the '%s' JSON message into a type: '%s'", msgTypeStr.c_str(), jsonString.c_str());
                }
            }
        }
        catch (exception& ex)
        {
            SPX_TRACE_ERROR(
                "Exception while parsing JSON string. Cause: '%s'. JSON: '%s'",
                ex.what(),
                jsonString.c_str());
            UNUSED(ex); // unused in release builds
        }
        catch (...)
        {
            SPX_TRACE_ERROR(
                "Exception while parsing JSON string. JSON: '%s'",
                jsonString.c_str());
        }

        return nullptr;
    }

    string ConversationMessageBase::ToJsonString() const
    {
        try
        {
            json json;
            Serialize(json);

            return json.dump();
        }
        catch (exception& ex)
        {
            SPX_TRACE_ERROR(
                "Failed to serialize the current '%s' conversation message instance. Reason: %s",
                MessageTypeStrings(Type),
                ex.what());
            UNUSED(ex); // unused in release builds

            throw;
        }
        catch (...)
        {
            SPX_TRACE_ERROR(
                "Failed to serialize the current '%s' conversation message instance.",
                MessageTypeStrings(Type));

            throw;
        }
    }

    bool ConversationMessageBase::TryParse(json& json)
    {
        bool success = true;

        // Type is set in the ConversationMessageBase::ParseJsonString
        success &= TryReadString(json, KEY_ROOM_ID, ConversationId);

        return ConversationId.empty() == false;
    }

    void ConversationMessageBase::Serialize(json & json) const
    {
        json[KEY_TYPE] = StringUtils::PascalCaseToSnakeCase(MessageTypeStrings(Type));

        if (!ConversationId.empty())
        {
            json[KEY_ROOM_ID] = ConversationId;
        }
    }

    ConversationNicknameMessageBase::ConversationNicknameMessageBase(const string& conversationId, const MessageType type, const string& nickname, const string& participantId)
        : ConversationMessageBase(conversationId, type), Nickname(nickname), ParticipantId(participantId), Avatar()
    {
    }

    ConversationNicknameMessageBase::ConversationNicknameMessageBase(const string& conversationId, const MessageType type, const string& nickname, const string& participantId, const string& avatar)
        : ConversationMessageBase(conversationId, type), Nickname(nickname), ParticipantId(participantId), Avatar(avatar)
    {
    }

    bool ConversationNicknameMessageBase::TryParse(json & json)
    {
        bool success = ConversationMessageBase::TryParse(json);

        success &= TryReadString(json, KEY_NICKNAME, Nickname);
        success &= TryReadString(json, KEY_PARTICIPANT_ID, ParticipantId);
        TryReadString(json, KEY_AVATAR, Avatar); // optional

        return success;
    }

    void ConversationNicknameMessageBase::Serialize(json & json) const
    {
        ConversationMessageBase::Serialize(json);

        json[KEY_NICKNAME] = Nickname;
        json[KEY_PARTICIPANT_ID] = ParticipantId;
        if (!Avatar.empty())
        {
            json[KEY_AVATAR] = Avatar;
        }
    }

    bool ConversationCommandMessage::TryParse(json & json)
    {
        bool success = ConversationNicknameMessageBase::TryParse(json);

        string commandTypeStr;

        success &= TryReadString(json, KEY_COMMAND, commandTypeStr);
        if (CommandType_FromString(commandTypeStr.c_str(), &Command))
        {
            success = false;
        }

        success &= TryReadBool(json, KEY_IS_HOST, IsHost);

        if (Command == CommandType::JoinSession)
        {
            success &= TryReadBool(json, KEY_IS_MUTED, IsMuted);
            success &= TryReadString(json, KEY_LOCALE, Locale);
            success &= TryReadBool(json, KEY_USE_TTS, UseTTS);
        }

        return success;
    }

    ConversationParticipantCommandMessage::ConversationParticipantCommandMessage(const string & conversationId, const string & nickname, const string & participantId, ParticipantCommandType command, bool value)
        : ConversationNicknameMessageBase(conversationId, MessageType::ParticipantCommand, nickname, participantId),
        Command(command), Value(value)
    {
    }

    ConversationParticipantCommandMessage::ConversationParticipantCommandMessage(const string & conversationId, const string & nickname, const string & participantId, ParticipantCommandType command, int32_t value)
        : ConversationNicknameMessageBase(conversationId, MessageType::ParticipantCommand, nickname, participantId),
        Command(command), Value(value)
    {
    }

    ConversationParticipantCommandMessage::ConversationParticipantCommandMessage(const string & conversationId, const string & nickname, const string & participantId, ParticipantCommandType command, const std::string & value)
        : ConversationNicknameMessageBase(conversationId, MessageType::ParticipantCommand, nickname, participantId),
        Command(command), Value(value)
    {
    }

    ConversationParticipantCommandMessage::ConversationParticipantCommandMessage(const string & conversationId, const string & nickname, const string & participantId, ParticipantCommandType command, const char * value, const size_t valueLength)
        : ConversationNicknameMessageBase(conversationId, MessageType::ParticipantCommand, nickname, participantId),
        Command(command), Value(value, valueLength)
    {
    }

    bool ConversationParticipantCommandMessage::TryParse(json & json)
    {
        bool success = ConversationNicknameMessageBase::TryParse(json);

        // Room ID is optional for info messages but mandatory for the participant list
        // messages. Let's validate the value has been set here
        success &= !ConversationId.empty();

        string commandStr;

        success &= TryReadString(json, KEY_COMMAND, commandStr);
        if (success)
        {
            success &= ParticipantCommandType_FromString(commandStr.c_str(), &Command) == 0;
        }

        auto value = json[KEY_VALUE];
        if (value.is_boolean())
        {
            Value = ParticipantCommandValue(value.get<bool>());
        }
        else if (value.is_number_integer() || value.is_number_unsigned())
        {
            Value = ParticipantCommandValue(value.get<int32_t>());
        }
        else if (value.is_string())
        {
            Value = ParticipantCommandValue(value.get<string>());
        }
        else
        {
            success = false;
        }

        return success;
    }

    void ConversationParticipantCommandMessage::Serialize(json & json) const
    {
        ConversationNicknameMessageBase::Serialize(json);

        json[KEY_COMMAND] = ParticipantCommandTypeStrings(Command);
        Value.Serialize(json, KEY_VALUE);
    }

    ConversationInfoMessage::ConversationInfoMessage(const string & conversationId, const string & command)
        : ConversationMessageBase(conversationId, MessageType::Info), Command(command)
    {
    }

    bool ConversationInfoMessage::TryParse(json& json, unique_ptr<ConversationMessageBase>& ptr)
    {
        bool success = TryParse(json);

        if (success)
        {
            if (Command == "token")
            {
                ptr.reset(new ConversationAuthorizationTokenMessage());
                return false;
            }
            else
            {
                ptr.reset(new ConversationParticipantListMessage());
                return false;
            }
        }

        return success;
    }

    bool ConversationInfoMessage::TryParse(json& json)
    {
        // the room ID is optional for some info messages
        ConversationMessageBase::TryParse(json);

        std::string command;
        bool success = TryReadString(json, KEY_COMMAND, command);
        Command = StringUtils::ToLower(command);

        return success;
    }

    void ConversationInfoMessage::Serialize(json& json) const
    {
        ConversationMessageBase::Serialize(json);

        json[KEY_COMMAND] = Command;
    }

    ConversationAuthorizationTokenMessage::ConversationAuthorizationTokenMessage(const string& authToken, const string& region)
        : ConversationInfoMessage({}, "token"), AuthToken(authToken), Region(region)
    {
    }

    bool ConversationAuthorizationTokenMessage::TryParse(json& json)
    {
        bool success = ConversationInfoMessage::TryParse(json);

        if (!json.is_object())
        {
            return false;
        }

        success &= TryReadString(json, KEY_TOKEN, AuthToken);
        success &= TryReadString(json, KEY_REGION, Region);

        // optionally try to parse the validTo
        TryReadUtcTime(json, KEY_VALID_TO, "%Y-%m-%dT%H:%M:%SZ", ValidUntil);

        success &= Command == "token";

        return success;
    }

    void ConversationAuthorizationTokenMessage::Serialize(json& json) const
    {
        ConversationInfoMessage::Serialize(json);

        json[KEY_TOKEN] = AuthToken;
        json[KEY_REGION] = Region;
    }

    bool ConversationParticipant::TryParse(json & json)
    {
        if (!json.is_object())
        {
            return false;
        }

        bool success = true;

        success &= TryReadString(json, KEY_NICKNAME, Nickname);
        success &= TryReadString(json, KEY_PARTICIPANT_ID, ParticipantId);
        success &= TryReadString(json, KEY_AVATAR, Avatar);
        success &= TryReadString(json, KEY_LOCALE, Locale);
        success &= TryReadBool(json, KEY_USE_TTS, UseTTS);
        success &= TryReadBool(json, KEY_IS_MUTED, IsMuted);
        success &= TryReadBool(json, KEY_IS_HOST, IsHost);

        return success;
    }

    bool ConversationParticipantListMessage::TryParse(json & json)
    {
        bool success = ConversationInfoMessage::TryParse(json);

        auto participants = json[KEY_PARTICIPANTS];
        if (participants.is_array())
        {
            for (json::iterator it = participants.begin(); it != participants.end() && success; ++it)
            {
                ConversationParticipant part;
                success &= part.TryParse(*it);
                if (success)
                {
                    Participants.push_back(part);
                }
            }
        }
        else
        {
            success = false;
        }

        TryReadString(json, KEY_TOKEN, Token); // optional

        return success;
    }

    bool ConversationRecognitionBase::TryParse(json & json)
    {
        bool success = ConversationNicknameMessageBase::TryParse(json);

        success &= TryReadString(json, KEY_ID, Id);
        success &= TryReadString(json, KEY_TIMESTAMP, Timestamp);
        success &= TryReadString(json, KEY_LANGUAGE, OriginalLanguage);

        auto translationsArr = json[KEY_TRANSLATIONS];
        if (translationsArr.is_array())
        {
            for (json::iterator it = translationsArr.begin(); it != translationsArr.end() && success; ++it)
            {
                if (!it->is_object())
                {
                    success = false;
                }
                else
                {
                    string lang;
                    string tran;

                    success &= TryReadString(*it, KEY_TRANSLATION_LANG, lang);
                    success &= TryReadString(*it, KEY_TRANSLATION_TRANSLATION, tran);

                    if (success)
                    {
                        Translations[lang] = tran;
                    }
                }
            }
        }
        else
        {
            success = false;
        }

        return success;
    }

    bool ConversationSpeechRecognitionMessage::TryParse(json & json)
    {
        bool success = ConversationRecognitionBase::TryParse(json);
        success &= TryReadString(json, KEY_RECOGNITION, Recognition);
        return success;
    }

    bool ConversationTranslatedMessage::TryParse(json & json)
    {
        bool success = ConversationRecognitionBase::TryParse(json);
        success &= TryReadString(json, "originalText", Text);
        return success;
    }

    ConversationInstantMessage::ConversationInstantMessage(const string & conversationId, const string & nickname, const string & participantId, const string & text)
        : ConversationNicknameMessageBase(conversationId, MessageType::InstantMessage, nickname, participantId), Text(text)
    {
    }

    bool ConversationInstantMessage::TryParse(json & json)
    {
        bool success = ConversationNicknameMessageBase::TryParse(json);
        success &= TryReadString(json, KEY_TEXT, Text);
        return success;
    }

    void ConversationInstantMessage::Serialize(json & json) const
    {
        ConversationNicknameMessageBase::Serialize(json);
        json[KEY_TEXT] = Text;
    }

}}}}} // Microsoft::CognitiveServices::Speech::Impl::Conversation
