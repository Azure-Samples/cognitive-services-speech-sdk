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
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "azure_c_shared_utility_httpapi_wrapper.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


typedef struct RestTtsResponse_Tag
{
    std::vector<uint8_t> body;
    std::mutex mutex;
} RestTtsResponse;


typedef struct RestTtsRequest_Tag
{
    std::string requestId;
    std::string endpoint;
    std::string postContent;
    bool isSsml;
    std::string subscriptionKey;
    std::string accessToken;
    std::string outputFormatString;
    SpxWAVEFORMATEX_Type outputFormat;
    bool outputHasHeader;
    ISpxTtsEngineAdapter* adapter;
    std::shared_ptr<ISpxTtsEngineAdapterSite> site;
    RestTtsResponse response;
} RestTtsRequest;


class CSpxRestTtsEngineAdapter :
    public ISpxObjectWithSiteInitImpl<ISpxTtsEngineAdapterSite>,
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
    void Term() override;

    // --- ISpxTtsEngineAdapter
    void SetOutput(std::shared_ptr<ISpxAudioOutput> output) override;
    std::shared_ptr<ISpxSynthesisResult> Speak(const std::string& text, bool isSsml, const std::string& requestId, bool retry) override;
    void StopSpeaking() override;


protected:

    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;


private:

    using SitePtr = std::shared_ptr<ISpxTtsEngineAdapterSite>;

    CSpxRestTtsEngineAdapter(const CSpxRestTtsEngineAdapter&) = delete;
    CSpxRestTtsEngineAdapter(const CSpxRestTtsEngineAdapter&&) = delete;

    CSpxRestTtsEngineAdapter& operator=(const CSpxRestTtsEngineAdapter&) = delete;

    void GetProxySetting();
    void EnsureHttpConnection(std::shared_ptr<ISpxNamedProperties> properties);
    SpxWAVEFORMATEX_Type GetOutputFormat(std::shared_ptr<ISpxAudioOutput> output, bool* hasHeader);
    std::string GetOutputFormatString(std::shared_ptr<ISpxAudioOutput> output);

    static void PostTtsRequest(HTTP_HANDLE http_connect, RestTtsRequest& request, std::shared_ptr<ISpxSynthesisResultInit> result_init);
    static void OnChunkReceived(void* context, const unsigned char* buffer, size_t size);

    static std::string ConstructUserAgent();


private:

    std::string m_endpoint;
    std::shared_ptr<ISpxAudioOutput> m_audioOutput;
    HTTP_HANDLE m_httpConnect = nullptr;

    std::string m_proxyHost;
    int m_proxyPort { 0 };
    std::string m_proxyUsername;
    std::string m_proxyPassword;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
