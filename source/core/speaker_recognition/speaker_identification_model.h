//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSpeakerIdentificationModel : public ISpxSIModel
{
public:

    CSpxSpeakerIdentificationModel()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    ~CSpxSpeakerIdentificationModel()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxSIModel)
    SPX_INTERFACE_MAP_END()

        // --- ISpxSIModel ---
    virtual void AddProfile(const std::shared_ptr<ISpxVoiceProfile>& profile) override;
    virtual std::vector<std::shared_ptr<ISpxVoiceProfile>> GetProfiles() const override;

private:

    DISABLE_COPY_AND_MOVE(CSpxSpeakerIdentificationModel);

    std::vector<std::shared_ptr<ISpxVoiceProfile>> m_profiles;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
