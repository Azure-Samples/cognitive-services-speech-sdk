#pragma once

#include <string>
#include <spxdebug.h>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_enums.h>
#include <speechapi_c_factory.h>
#include <speechapi_c_result.h>
#include <speechapi_c_recognizer.h>
#include <speechapi_c_session.h>
#include <speechapi_c_property_bag.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {

class SpeechRecognizer;
class Session;
class RecognitionResult;

// Forward declaration for friends.
namespace Intent { class IntentRecognizer; }
namespace Translation { class TranslationRecognizer; }
namespace Impl { class MockParameterValue; }

enum class HandleType { MOCK, RESULT, SESSION, RECOGNIZER };

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
        property_bag_release(m_propbag);
        m_propbag = SPXHANDLE_INVALID;
    }

public:
    void SetProperty(PropertyId propertyID, const std::string& value)
    {
        property_bag_set_string(m_propbag, (int)propertyID, NULL, value.c_str());
    }

    void SetProperty(const std::string& propertyName, const std::string& value)
    {
        property_bag_set_string(m_propbag, -1, propertyName.c_str(), value.c_str());
    }

    std::string GetProperty(PropertyId propertyID, const std::string& defaultValue = "")
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
    friend class Microsoft::CognitiveServices::Speech::Session;
    friend class Microsoft::CognitiveServices::Speech::RecognitionResult;
    friend class Microsoft::CognitiveServices::Speech::Intent::IntentRecognizer;
    friend class Microsoft::CognitiveServices::Speech::Translation::TranslationRecognizer;
    friend class Microsoft::CognitiveServices::Speech::Impl::MockParameterValue;

private:
    SPXPROPERTYBAGHANDLE m_propbag;
};

} } } // Microsoft::CognitiveServices::Speech
