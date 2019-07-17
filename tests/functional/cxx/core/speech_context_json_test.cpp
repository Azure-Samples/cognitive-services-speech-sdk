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

    void SetToLanguages(string toLangs)
    {
        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        properties->SetStringValue(GetPropertyName(PropertyId::SpeechServiceConnection_TranslationToLanguages), toLangs.c_str());
    }

    void SetInsertLeftRight(string left, string right)
    {
        auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
        properties->SetStringValue("DictationInsertionPointLeft", left.c_str());
        properties->SetStringValue("DictationInsertionPointRight", right.c_str());
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


    string getSpeechContextJson()
    {
        return m_adapter.GetSpeechContextJson();
    }

    shared_ptr<TestCSpxAudioStreamSession> GetSession()
    {
        return m_session;
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

        auto contextJsonStr = adapterTest.getSpeechContextJson();
        REQUIRE(json::parse(contextJsonStr) == expectedJson);
    }

    SPXTEST_SECTION("Test DGI Json")
    {
        string intentName = "intent1";
        string grammar1 = "grammar1";
        string grammar2 = "grammar2";
        string id = "randomkey2-xyz";
        list<string> listenFors
        {
            "{luis:" + id + "-PRODUCTION#" + intentName + "}",
            grammar1,
            grammar2
        };
        session->SetListenForList(listenFors);

        expectedJson["dgi"]["ReferenceGrammars"] = json::array(
            {
                "luis/" + id + "-PRODUCTION#" + intentName
            });
        json grammarJson;
        grammarJson["Type"] = "Generic";
        grammarJson["Items"] = json::array();
        for (auto grammar : { grammar1, grammar2 })
        {
            grammarJson["Items"].push_back({ {"Text", grammar} });
        }
        expectedJson["dgi"]["Groups"] = json::array({ grammarJson });

        auto contextJsonStr = adapterTest.getSpeechContextJson();
        REQUIRE(json::parse(contextJsonStr) == expectedJson);
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

        auto contextJsonStr = adapterTest.getSpeechContextJson();
        REQUIRE(json::parse(contextJsonStr) == expectedJson);
    }

    SPXTEST_SECTION("Test Dictation and Translationcontext Json")
    {
        vector<string> toLangs{ "en-us", "Fr-fr" };
        session->SetToLanguages(toLangs[0] + "," + toLangs[1]);
        session->SetInsertLeftRight("left", "right");

        expectedJson["translationcontext"]["to"] = json(toLangs);
        expectedJson["dictation"]["insertionPoint"]["left"] = "left";
        expectedJson["dictation"]["insertionPoint"]["right"] = "right";

        auto contextJsonStr = adapterTest.getSpeechContextJson();
        REQUIRE(json::parse(contextJsonStr) == expectedJson);
    }
}
