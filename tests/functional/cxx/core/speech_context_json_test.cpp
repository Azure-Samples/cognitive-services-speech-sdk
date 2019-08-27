//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <chrono>
#include <thread>
#include <random>
#include <string>

#include "exception.h"
#define __SPX_THROW_HR_IMPL(hr) Microsoft::CognitiveServices::Speech::Impl::ThrowWithCallstack(hr)

#include "test_utils.h"
#include "audio_stream_session.h"
#include "usp_reco_engine_adapter.h"
#include "site_helpers.h"
#include "create_object_helpers.h"
#include "ispxinterfaces.h"
#include "json.h"

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Impl;
using json = nlohmann::json;

class TestCSpxAudioStreamSession : public CSpxAudioStreamSession
{
public:

    void SetIntentInfo(const string& provider, const string& id, const string& key, const string& region)
    {
        m_provider = provider;
        m_id = id;
        m_key = key;
        m_region = region;
    }

    void GetIntentInfo(string& provider, string& id, string& key, string& region) override
    {
        provider = m_provider;
        id = m_id;
        key = m_key;
        region = m_region;
    }

    void SetSpottedKeywordResult(wstring keyword, double confidence, uint64_t offset, uint64_t duration)
    {
        if (m_result == nullptr)
        {
            m_result = make_shared<CSpxRecognitionResult>();
        }
        m_result->InitKeywordResult(confidence, offset, duration, keyword.c_str(), ResultReason::RecognizedIntent);
        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        properties->SetStringValue(KeywordConfig_EnableKeywordVerification, "true");
    }

    shared_ptr<ISpxRecognitionResult> GetSpottedKeywordResult() override
    {
        return m_result;
    }

    void SetListenForList(const list<string>& listners)
    {
        m_listnerFor = listners;
    }

    list<string> GetListenForList() override
    {
        return m_listnerFor;
    }

    shared_ptr<ISpxNamedProperties> GetPropertiesPtr()
    {
        return SpxQueryService<ISpxNamedProperties>(GetSite());
    }

    void SetInsertLeftRight(string left, string right)
    {
        auto properties = GetPropertiesPtr();
        properties->SetStringValue("DictationInsertionPointLeft", left.c_str());
        properties->SetStringValue("DictationInsertionPointRight", right.c_str());
    }

    void SetNoIntent() {
        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        properties->SetStringValue("CARBON-INTERNAL-USP-NoIntentJson", "true");
    }

    void SetTargetLanguages(std::string toLanguages)
    {
        auto properties = GetPropertiesPtr();
        properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), toLanguages.c_str());
    }

    // The data in speech context and speech config that could be set from user is being passed via a recognizer.
    // The speech_context_json_test.cpp does not have a recognizer. So, override it with empty data
    CSpxStringMap GetParametersFromUser(std::string&& path) override
    {
        UNUSED(path);
        return {};
    }

private:

    string m_provider;
    string m_id;
    string m_key;
    string m_region;
    list<string> m_listnerFor;
    shared_ptr<CSpxRecognitionResult> m_result;
};


class CSpxUspRecoEngineAdapterTest
{
public:

    CSpxUspRecoEngineAdapterTest()
    {
        m_session = std::make_shared<TestCSpxAudioStreamSession>();
        m_session->SetSite(SpxGetRootSite());
        auto site = SpxQueryInterface<ISpxGenericSite>(m_session);
        m_adapter.SetSite(site);
    }


    json GetSpeechContextJson()
    {
        return m_adapter.GetSpeechContextJson();
    }

    void SetEndpointType(USP::EndpointType endpointType)
    {
        m_adapter.m_endpointType = endpointType;
    }

    shared_ptr<TestCSpxAudioStreamSession> GetSession()
    {
        return m_session;
    }

    string Join(vector<string> values)
    {
        stringstream ss;
        size_t count = 0;
        for(auto& value: values)
        {
            ss << value;
            if (++count < values.size())
            {
                ss << CommaDelim;
            }
        }
        return ss.str();
    }

private:

    CSpxUspRecoEngineAdapter m_adapter;
    shared_ptr<TestCSpxAudioStreamSession> m_session;
};


TEST_CASE("Test JSON Generation", "[context_json]")
{
    CSpxUspRecoEngineAdapterTest adapterTest;
    auto session = adapterTest.GetSession();
    json expectedJson;

    SPXTEST_SECTION("Test Intent Json")
    {
        string provider = "LUIS";
        string id = "abcd-efghigk";
        string key = "randomkey1-xyz";
        string region = "WEUS";
        session->SetIntentInfo(provider, id, key, region);

        expectedJson["intent"]["provider"] = provider;
        expectedJson["intent"]["id"] = id;
        expectedJson["intent"]["key"] = key;

        auto contextJson = adapterTest.GetSpeechContextJson();
        REQUIRE(contextJson == expectedJson);

        session->SetNoIntent();
        contextJson = adapterTest.GetSpeechContextJson();
        json emptyJson;
        REQUIRE(contextJson == emptyJson);
    }

    SPXTEST_SECTION("Test DGI Json")
    {
        string intentName = "intent1";
        vector<string> grammars
        {
            "g\\rammar1",
            R"("gra\mmar2")",
            "grammar3"
        };

        string id = "randomkey2-xyz";
        list<string> listenFors(grammars.begin(), grammars.end());
        listenFors.push_back("{luis:" + id + "-PRODUCTION#" + intentName + "}");
        session->SetListenForList(listenFors);

        expectedJson["dgi"]["ReferenceGrammars"] = json::array(
            {
                "luis/" + id + "-PRODUCTION#" + intentName
            });
        json grammarJson;
        grammarJson["Type"] = "Generic";
        grammarJson["Items"] = json::array();
        for (auto grammar : grammars)
        {
            grammarJson["Items"].push_back({ {"Text", grammar} });
        }
        expectedJson["dgi"]["Groups"] = json::array({ grammarJson });

        auto contextJson = adapterTest.GetSpeechContextJson();
        REQUIRE(contextJson == expectedJson);
    }

    SPXTEST_SECTION("Test KeywordResult Json")
    {
        wstring keyWord = L"Cortana(微软小娜)";
        double confidence = 0.8;
        uint64_t offSet = 1000;
        uint64_t duration = 20000;
        session->SetSpottedKeywordResult(keyWord, confidence, offSet, duration);

        expectedJson["invocationSource"] = "VoiceActivationWithKeyword";
        expectedJson["keywordDetection"] = { {
                   {"type", "startTrigger"},
                   {"clientDetectedKeywords", {{
                       {"text", PAL::ToString(keyWord)},
                       {"confidence", confidence},
                       {"startOffset", offSet},
                       {"duration", duration}
                   }}},
                   {"onReject", {
                       {"action", "EndOfTurn"}
                   }}
               } };

        auto contextJson = adapterTest.GetSpeechContextJson();
        REQUIRE(contextJson == expectedJson);
    }

    SPXTEST_SECTION("Test Dictation Json")
    {
        session->SetInsertLeftRight("left", "right");
        expectedJson["dictation"]["insertionPoint"]["left"] = "left";
        expectedJson["dictation"]["insertionPoint"]["right"] = "right";
        auto contextJson = adapterTest.GetSpeechContextJson();
        REQUIRE(contextJson == expectedJson);
        // clean the data
        session->SetInsertLeftRight("", "");

    }

    SPXTEST_SECTION("Test Translationcontext Json")
    {
        vector<string> toLangs{ "en-us", "Fr-fr" };
        session->SetTargetLanguages(adapterTest.Join(toLangs));
        expectedJson["translationcontext"]["to"] = json(toLangs);
        auto contextJson = adapterTest.GetSpeechContextJson();
        auto contextJsonStr = contextJson.dump();
        auto expectedJsonStr = expectedJson.dump();
        REQUIRE(contextJson == expectedJson);
        // clean the data
        session->SetTargetLanguages("");
    }

    SPXTEST_SECTION("Test Translation NewEndpoint Json")
    {
        adapterTest.SetEndpointType(USP::EndpointType::Translation);
        auto properties = session->GetPropertiesPtr();
        auto recoMode = g_recoModeConversation;
        properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_RecoMode), recoMode);

        vector<string> autoDetectSourceLangs{ "en-us", "zh-CN" };
        // Added some spaces and tabs in source languages, to verify our code can remove them correctly
        properties->SetStringValue("Auto-Detect-Source-Languages", "    en-  us    ,    zh- CN    ");
        json languageIdJson;
        languageIdJson["languages"] = json(autoDetectSourceLangs);
        languageIdJson["onUnknown"]["action"] = "None";
        languageIdJson["onSuccess"]["action"] = "Recognize";
        expectedJson["languageId"] = languageIdJson;

        unordered_map<string, string> voiceNameMap
        {
               {"de-DE", "Microsoft Server Speech Text to Speech Voice(de - DE, Hedda)" },
               {"fr-FR", "Microsoft Server Speech Text to Speech Voice (fr-FR, Julie, Apollo)"}
        };
        vector<string> toLangs;
        for (auto& pair : voiceNameMap)
        {
            toLangs.push_back(pair.first);
        }
        toLangs.push_back("en-US");
        session->SetTargetLanguages(adapterTest.Join(toLangs));

        json phraseDetectionJson;
        phraseDetectionJson["mode"] = recoMode;
        phraseDetectionJson["onSuccess"]["action"] = "Translate";
        phraseDetectionJson["onInterim"]["action"] = "Translate";
        expectedJson["phraseDetection"] = phraseDetectionJson;
        expectedJson["phraseOutput"]["interimResults"][recoMode]["resultType"] = "None";
        expectedJson["phraseOutput"]["phraseResults"][recoMode]["resultType"] = "None";

        json translationJson;
        translationJson["targetLanguages"] = json(toLangs);
        translationJson["output"]["interimResults"]["mode"] = "Always";
        translationJson["onSuccess"]["action"] = "None";
        expectedJson["translation"] = translationJson;
        expectedJson["translationcontext"]["to"] = json(toLangs);

        auto contextJson = adapterTest.GetSpeechContextJson();
        REQUIRE(contextJson == expectedJson);

        for (auto& pair : voiceNameMap)
        {
            string voiceNameProperty = pair.first + GetPropertyName(PropertyId::SpeechServiceConnection_TranslationVoice);
            properties->SetStringValue(voiceNameProperty.c_str(), pair.second.c_str());
        }
        expectedJson["translation"]["onSuccess"]["action"] = "Synthesize";
        expectedJson["synthesis"]["defaultVoices"] = json(std::move(voiceNameMap));
        contextJson = adapterTest.GetSpeechContextJson();
        REQUIRE(contextJson == expectedJson);
    }
}
