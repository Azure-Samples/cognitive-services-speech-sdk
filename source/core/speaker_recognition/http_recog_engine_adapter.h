// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include "ispxinterfaces.h"
#include "service_helpers.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "http_response.h"
#include "http_utils.h"
#include "http_client.h"
#include <object_with_site_init_impl.h>
#include <json.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


using ResponsePtr = std::unique_ptr<HttpResponse>;

class CSpxHttpRecoEngineAdapter :
    public ISpxGenericSite,
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxHttpRecoEngineAdapter,
    public ISpxRecoResultFactory,
    public ISpxPropertyBagImpl
{
private:
    enum class SpeakerRecognitionOperationType
    {
        Unknown,
        EnrollProfile,
        DeleteProfile,
        ResetProfile,
        Identify,
        Verify,
    };

public:
    CSpxHttpRecoEngineAdapter() = default;
    virtual ~CSpxHttpRecoEngineAdapter() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxHttpRecoEngineAdapter)
        SPX_INTERFACE_MAP_ENTRY(ISpxRecoResultFactory)
    SPX_INTERFACE_MAP_END()

        // --- IServiceProvider ---
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxObject ---
    void Init() override;
    void Term() override;

    // --- ISpxHttpRecoEngineAdapter ---
    void SetFormat(const SPXWAVEFORMATEX* pformat, VoiceProfileType type, std::vector<std::string>&& profileIds, bool enroll) override;
    void ProcessAudio(const DataChunkPtr& audioChunk) override;
    void FlushAudio() override;
    RecognitionResultPtr GetResult() override;
    RecognitionResultPtr ModifyVoiceProfile(ModifyOperation operation, VoiceProfileType type, std::string&& id) override;
    VoiceProfilePtr CreateVoiceProfile(VoiceProfileType type, std::string&& locale) const override;
    std::vector<VoiceProfilePtr> GetVoiceProfiles(VoiceProfileType type) const override;
    VoiceProfilePtr GetVoiceProfileStatus(VoiceProfileType type, std::string&& voiceProfileId) const override;

    RecognitionResultPtr GetResult(SpeakerRecognitionOperationType operationType);

    // --- ISpxRecoResultFactory
    virtual RecognitionResultPtr CreateIntermediateResult(const wchar_t* text, uint64_t offset, uint64_t duration) override;
    virtual RecognitionResultPtr CreateFinalResult(ResultReason reason, NoMatchReason noMatchReason, const wchar_t* text, uint64_t offset, uint64_t duration, const wchar_t* userId = nullptr) override;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason, std::shared_ptr<ISpxAudioDataStream> stream) override;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateErrorResult(const std::shared_ptr<ISpxErrorInformation>& error) override;
    virtual std::shared_ptr<ISpxRecognitionResult> CreateEndOfStreamResult() override;

private:
    DISABLE_COPY_AND_MOVE(CSpxHttpRecoEngineAdapter);
    using SitePtr = std::shared_ptr<ISpxHttpRecoEngineAdapter>;

    const std::string GetSubscriptionKey() const;
    std::unique_ptr<HttpResponse> SendRequest(const HttpEndpointInfo& endPoint, HTTPAPI_REQUEST_TYPE requestType, const void *content = nullptr, size_t contentSize = 0) const;
    HttpEndpointInfo CreateEndpoint(VoiceProfileType type) const;
    HttpEndpointInfo CreateEndpoint(std::string&& fullPath) const;
    std::string GetDefaultEndpoint();

    // override this in ISpxPropertyBagImpl.
    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;

    std::string m_speakerRecognitionHostName;
    // calculate a hash for an enum class to make gcc happy.
    struct EnumClassHash
    {
        template <typename T>
        std::size_t operator()(T t) const
        {
            return static_cast<std::size_t>(t);
        }
    };
    std::unordered_map<VoiceProfileType, std::string, EnumClassHash> m_speakerIdPaths;
    VoiceProfileType m_voiceProfileType;
    bool m_enroll;
    std::string m_profileIdForVerification;
    bool m_audioFlushed = false;

    inline SpeakerRecognitionOperationType GetCurrentOperationType()
    {
        return m_enroll ? SpeakerRecognitionOperationType::EnrollProfile
            : m_voiceProfileType == VoiceProfileType::TextIndependentIdentification ? SpeakerRecognitionOperationType::Identify
            : SpeakerRecognitionOperationType::Verify;
    }
    ResultReason GetResultReasonForResponse(SpeakerRecognitionOperationType operationType);

    std::shared_ptr<ISpxErrorInformation> TryPopulateResultFromResponse(RecognitionResultPtr& result);

    std::unique_ptr<ISpxHttpDataClient> m_httpData;
    std::unique_ptr<HttpResponse> m_response;

    RecognitionResultPtr m_result;
    UriScheme m_uriScheme;
    bool m_devEndpoint;
};
}}}}
