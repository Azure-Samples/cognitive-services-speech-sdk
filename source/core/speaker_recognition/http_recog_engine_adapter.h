// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// participant.h: Private implementation declarations for participant
//
#pragma once

#include "ispxinterfaces.h"
#include "service_helpers.h"
#include "interface_helpers.h"
#include "property_bag_impl.h"
#include "http_response.h"
#include "http_utils.h"
#include "http_client.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


using ResponsePtr = std::unique_ptr<HttpResponse>;
using CreateFinalResultFuncPtr = std::function<RecognitionResultPtr(ResultReason, CancellationReason, NoMatchReason, CancellationErrorCode, const wchar_t*)>;

class ISpxSpeakerResultFactory : public ISpxInterfaceBaseFor<ISpxSpeakerResultFactory>
{
public:
    virtual RecognitionResultPtr CreateResult(CreateFinalResultFuncPtr func) = 0;

protected:
    virtual ResultReason GetResultReason() { return ResultReason::Canceled; };
    virtual CancellationReason GetCancellationReason(bool isSuccess);
    virtual NoMatchReason GetNoMatchReason();
    virtual CancellationErrorCode GetCancellationErrorCode(int httpStatusCode);

    RecognitionResultPtr CreateErrorResult(CreateFinalResultFuncPtr func, const std::string& error);
    void PopulateJsonResult(RecognitionResultPtr& result, const std::string& jsonString);
    std::string GetErrorMessageInJson(const std::string& str, int statusCode);
    void SetInPropertyBag(ISpxNamedProperties* properties, std::string&& name, float valueInSeconds);
};

class DeleteOrResetResultFactory : public ISpxSpeakerResultFactory
{
public:
    DeleteOrResetResultFactory(std::unique_ptr<HttpResponse> response, bool reset) :
        m_response(std::move(response)), m_reset(reset)
    {
    }
    virtual ~DeleteOrResetResultFactory() = default;
    virtual RecognitionResultPtr CreateResult(CreateFinalResultFuncPtr func) override;

private:
    ResponsePtr m_response;
    bool m_reset;
};

class EnrollmentResultFactory : public ISpxSpeakerResultFactory
{
public:

    EnrollmentResultFactory(std::unique_ptr<HttpResponse> response) :
        m_response(std::move(response))
    {
    }

    virtual ~EnrollmentResultFactory() = default;

    virtual RecognitionResultPtr CreateResult(CreateFinalResultFuncPtr func) override;

private:
    RecognitionResultPtr PopulateEnrollmentDurations(CreateFinalResultFuncPtr func, RecognitionResultPtr result, const nlohmann::json& json);

    ResponsePtr m_response;
};

class IdentifyResultFactory : public ISpxSpeakerResultFactory
{
public:
    IdentifyResultFactory(ResponsePtr response) :
        m_response(std::move(response))
    {
    }

    virtual ~IdentifyResultFactory() = default;

    virtual RecognitionResultPtr CreateResult(CreateFinalResultFuncPtr func) override;

private:
    ResponsePtr m_response;
};

class VerifyResultFactory : public ISpxSpeakerResultFactory
{
public:
    VerifyResultFactory(ResponsePtr response, std::string&& profileid) :
        m_response(std::move(response)), m_profileId(std::move(profileid))
    {
    }

    virtual ~VerifyResultFactory() = default;

    virtual RecognitionResultPtr CreateResult(CreateFinalResultFuncPtr func) override;

private:
    bool GetReasons(ResultReason& resultReason, CancellationReason& cancelReason);
    std::string GetErrorMesssage(bool rejected);

    ResponsePtr m_response;
    std::string m_profileId;
};

class CSpxHttpRecoEngineAdapter :
    public ISpxGenericSite,
    public ISpxObjectWithSiteInitImpl<ISpxGenericSite>,
    public ISpxServiceProvider,
    public ISpxHttpRecoEngineAdapter,
    public ISpxPropertyBagImpl
{
public:
    CSpxHttpRecoEngineAdapter() = default;
    virtual ~CSpxHttpRecoEngineAdapter() = default;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxHttpRecoEngineAdapter)
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
    RecognitionResultPtr ModifyVoiceProfile(bool reset, VoiceProfileType type, std::string&& id) const override;
    std::string CreateVoiceProfile(VoiceProfileType type, std::string&& locale) const override;

private:
    DISABLE_COPY_AND_MOVE(CSpxHttpRecoEngineAdapter);
    using SitePtr = std::shared_ptr<ISpxHttpRecoEngineAdapter>;

    RecognitionResultPtr CreateRecoResult(ResultReason reason, CancellationReason canReason, NoMatchReason noMatchReason, CancellationErrorCode errorCode, const wchar_t* text) const;
    RecognitionResultPtr CreateErrorResult(const std::string& error) const;
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

    std::unique_ptr<ISpxHttpDataClient> m_httpData;
    std::unique_ptr<HttpResponse> m_response;

    RecognitionResultPtr m_result;
    UriScheme m_uriScheme;
    bool m_devEndpoint;
};
}}}}
