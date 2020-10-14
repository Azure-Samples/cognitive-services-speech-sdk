//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// voice_profile_client.h: Implementation declarations for CSpxVoiceProfileClient C++ class
//

#pragma once
#include "ispxinterfaces.h"
#include "interface_helpers.h"
#include "service_helpers.h"
#include "property_bag_impl.h"
#include <object_with_site_init_impl.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxVoiceProfileClient :
    public ISpxObjectWithSiteInitImpl<ISpxSpeakerRecognition>,
    public ISpxGenericSite,
    public ISpxServiceProvider,
    public ISpxVoiceProfileClient,
    public ISpxPropertyBagImpl
{
public:

    CSpxVoiceProfileClient();
    virtual ~CSpxVoiceProfileClient();

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectWithSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxObjectInit)
        SPX_INTERFACE_MAP_ENTRY(ISpxServiceProvider)
        SPX_INTERFACE_MAP_ENTRY(ISpxGenericSite)
        SPX_INTERFACE_MAP_ENTRY(ISpxNamedProperties)
        SPX_INTERFACE_MAP_ENTRY(ISpxVoiceProfileClient)
   SPX_INTERFACE_MAP_END()

    // --- ISpxObjectWithSiteInit
    void Init() override;
    void Term() override;

    // --- IServiceProvider
    SPX_SERVICE_MAP_BEGIN()
        SPX_SERVICE_MAP_ENTRY(ISpxNamedProperties)
        SPX_SERVICE_MAP_ENTRY_FUNC(InternalQueryService)
        SPX_SERVICE_MAP_ENTRY_SITE(GetSite())
    SPX_SERVICE_MAP_END()

    // --- ISpxVoiceProfileClient
    VoiceProfilePtr CreateVoiceProfile(VoiceProfileType voiceProfileType, std::string&& locale) const override;
    RecognitionResultPtr ModifyVoiceProfile(ModifyOperation operation, VoiceProfileType type, std::string&& id) override;
    std::vector<VoiceProfilePtr> GetVoiceProfiles(VoiceProfileType type) const override;
    RecognitionResultPtr Identify(std::vector<std::shared_ptr<ISpxVoiceProfile>>&& profiles) override;
    RecognitionResultPtr Verify(VoiceProfileType type, std::string&& profileId) override;
    RecognitionResultPtr Enroll(VoiceProfileType type, std::string&& profileId)  override;
    VoiceProfilePtr GetVoiceProfileStatus(VoiceProfileType type, std::string&& voiceProfileId) const override;

private:
    using SitePtr = std::shared_ptr<ISpxSpeakerRecognition>;

    DISABLE_COPY_AND_MOVE(CSpxVoiceProfileClient);

    std::shared_ptr<ISpxSpeakerRecognition> InternalQueryService(const char* serviceName);

    // enable logging.
    void CheckLogFilename();
    // --- provide a way to get parent's site's properties.
    std::shared_ptr<ISpxNamedProperties> GetParentProperties() const override;
    std::shared_ptr<ISpxSpeakerRecognition> m_keepSiteAlive;
};

 }}}}
