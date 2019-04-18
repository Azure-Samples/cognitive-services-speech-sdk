//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// rest_tts_engine_adapter.h: Implementation declarations for CSpxRestTtsEngineAdapter C++ class
//

#pragma once
#include <memory>
#include <queue>
#include "asyncop.h"
#include "spxcore_common.h"
#include "authenticator.h"
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"

#define HTTPS_URL_PREFIX "https://"
#define ISSUE_TOKEN_HOST_SUFFIX ".api.cognitive.microsoft.com"
#define ISSUE_TOKEN_URL_PATH "/sts/v1.0/issueToken"
#define TTS_COGNITIVE_SERVICE_HOST_SUFFIX ".tts.speech.microsoft.com"
#define TTS_COGNITIVE_SERVICE_URL_PATH "/cognitiveservices/v1"
#define TTS_CUSTOM_VOICE_HOST_SUFFIX ".voice.speech.microsoft.com"
#define USER_AGENT "SpeechSDK"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


typedef struct RestTtsRequest_Tag
{
    std::wstring requestId;
    std::string endpoint;
    std::string postContent;
    bool isSsml;
    std::string accessToken;
    std::string outputFormatString;
    SpxWAVEFORMATEX_Type outputFormat;
    bool outputHasHeader;
    ISpxTtsEngineAdapter* adapter;
    std::shared_ptr<ISpxTtsEngineAdapterSite> site;
} RestTtsRequest;


class CSpxRestTtsEngineAdapter :
    public ISpxTtsEngineAdapter,
    public ISpxPropertyBagImpl
{
public:

    CSpxRestTtsEngineAdapter();
    virtual ~CSpxRestTtsEngineAdapter();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxTtsEngineAdapter)
    SPX_INTERFACE_MAP_END()

    // --- ISpxObjectInit
    void Init() override;

    // --- ISpxTtsEngineAdapter
    void SetOutput(std::shared_ptr<ISpxAudioOutput> output) override;
    std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml, const std::wstring& requestId) override;


protected:

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;


private:

    using SitePtr = std::shared_ptr<ISpxTtsEngineAdapterSite>;

    CSpxRestTtsEngineAdapter(const CSpxRestTtsEngineAdapter&) = delete;
    CSpxRestTtsEngineAdapter(const CSpxRestTtsEngineAdapter&&) = delete;

    CSpxRestTtsEngineAdapter& operator=(const CSpxRestTtsEngineAdapter&) = delete;

    SpxWAVEFORMATEX_Type GetOutputFormat(std::shared_ptr<ISpxAudioOutput> output, bool* hasHeader);
    std::string GetOutputFormatString(std::shared_ptr<ISpxAudioOutput> output);

    static std::string ParseRegionFromCognitiveServiceEndpoint(const std::string& endpoint);
    static bool IsCustomVoiceEndpoint(const std::string& endpoint);
    static bool IsStandardVoiceEndpoint(const std::string& endpoint);
    static void PostTtsRequest(RestTtsRequest& request, std::shared_ptr<ISpxSynthesisResultInit> result_init);
    static void OnChunkReceived(void* context, const unsigned char* buffer, size_t size);


private:

    std::string m_endpoint;
    std::shared_ptr<ISpxAudioOutput> m_audioOutput;
    std::shared_ptr<CSpxRestTtsAuthenticator> m_authenticator;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
