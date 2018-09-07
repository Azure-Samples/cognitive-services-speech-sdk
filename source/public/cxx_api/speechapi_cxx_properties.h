
#pragma once
#include <string>
#include <speechapi_c_common.h>
#include <spxdebug.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

class SpeechRecognizer;
class ISpeechFactory;
class Session;
class RecognitionResult;
class SpeechFactory;

// Forward declaration for friends.
namespace Intent { class IntentRecognizer; }
namespace Translation { class TranslationRecognizer;   }
namespace Impl { class MockParameterValue; }

enum class SpeechPropertyId
{
    SpeechServiceConnection_Key = 1000,
    SpeechServiceConnection_Endpoint = 1001,
    SpeechServiceConnection_Region = 1002,
    SpeechServiceAuthorization_Token = 1003,
    SpeechServiceAuthorization_Type = 1004,
    SpeechServiceRps_Token = 1005,
    SpeechServiceConnection_DeploymentId = 1006,
    
    SpeechServiceConnection_TranslationFromLanguage = 2000,
    SpeechServiceConnection_TranslationToLanguages = 2001,
    SpeechServiceConnection_TranslationVoice = 2002,
    SpeechServiceConnection_TranslationFeatures = 2003,
    SpeechServiceConnection_IntentRegion = 2004,

    SpeechServiceConnection_RecoMode = 3000,
    SpeechServiceConnection_RecoMode_Interactive = 3001,
    SpeechServiceConnection_RecoMode_Conversation = 3002,
    SpeechServiceConnection_RecoMode_Dictation = 3004,
    SpeechServiceConnection_RecoLanguage = 3005,
    Speech_SessionId = 3006,

    SpeechServiceResponse_OutputFormat = 4000,
    SpeechServiceResponse_OutputFormat_Simple = 4001,
    SpeechServiceResponse_OutputFormat_Detailed = 4002,
    SpeechServiceResponse_RequestProfanityFilterTrueFalse = 4003,

    SpeechServiceResponse_Json = 5000,
    SpeechServiceResponse_JsonResult = 5001,
    SpeechServiceResponse_JsonErrorDetails = 5002,
    
    CancellationDetails_ReasonCanceled = 6000,
    CancellationDetails_ReasonText = 6001,
    CancellationDetails_ReasonDetailedText = 6002
};

enum class HandleType { MOCK, RESULT, SESSION, RECOGNIZER, FACTORY};

template <typename Handle >
class PropertyCollection
{
private:
    PropertyCollection() : m_propbag(nullptr) {}

    explicit PropertyCollection(Handle h, HandleType type)
    {
        SPX_INIT_HR(hr);

        switch (type)
        {
        case HandleType::MOCK:
            m_propbag = h;
            break;
        case HandleType::RESULT:
            SPX_THROW_ON_FAIL(hr = result_get_property_bag(h, &m_propbag));
            break;
        case HandleType::SESSION:
            SPX_THROW_ON_FAIL(hr = session_get_property_bag(h, &m_propbag));
            break;
        case HandleType::FACTORY:
            SPX_THROW_ON_FAIL(hr = speech_factory_get_property_bag(h, &m_propbag));
            break;
        case HandleType::RECOGNIZER:
            SPX_THROW_ON_FAIL(hr = recognizer_get_property_bag(h, &m_propbag));
            break;
        default:
           SPX_TRACE_ERROR("Unknown handle type, %d", static_cast<int>(type));
           SPX_THROW_ON_FAIL(SPXERR_INVALID_ARG);
        }

        if (type != HandleType::MOCK)
        {
            SPX_IFTRUE_THROW_HR(m_propbag == nullptr, SPXERR_INVALID_ARG);
            SPX_IFFALSE_THROW_HR(property_bag_is_valid(m_propbag), SPXERR_INVALID_ARG);
        }
    }

public:

    ~PropertyCollection()
    {
        property_bag_close(m_propbag);
        m_propbag = SPXHANDLE_INVALID;
    }

public:
    void SetProperty(SpeechPropertyId propertyID, const std::string& value)
    {
        property_bag_set_string(m_propbag, (int)propertyID, NULL, value.c_str());
    }

    void SetProperty(const std::string& propertyName, const std::string& value)
    {
        property_bag_set_string(m_propbag, -1, propertyName.c_str(), value.c_str());
    }

    std::string GetProperty(SpeechPropertyId propertyID, const std::string& defaultValue = "")
    {
        return property_bag_get_string(m_propbag, static_cast<int>(propertyID), nullptr, defaultValue.c_str());
    }

    std::string GetProperty(const std::string& propertyName, const std::string& defaultValue = "")
    {
        return property_bag_get_string(m_propbag, -1, propertyName.c_str(), defaultValue.c_str());
    }

private:

    DISABLE_COPY_AND_MOVE(PropertyCollection);

    friend class Microsoft::CognitiveServices::Speech::SpeechRecognizer;
    friend class Microsoft::CognitiveServices::Speech::ISpeechFactory;
    friend class Microsoft::CognitiveServices::Speech::SpeechFactory;
    friend class Microsoft::CognitiveServices::Speech::Session;
    friend class Microsoft::CognitiveServices::Speech::RecognitionResult;
    friend class Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer;
    friend class Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer;
    friend class Microsoft::CognitiveServices::Speech::Impl::MockParameterValue;

private:
    SPXPROPERTYBAGHANDLE m_propbag;
};

} } } // Microsoft::CognitiveServices::Speech
