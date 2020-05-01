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


class CSpxSpeakerVerificationModel : public ISpxSVModel
{
public:

    CSpxSpeakerVerificationModel()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    ~CSpxSpeakerVerificationModel()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxSVModel)
    SPX_INTERFACE_MAP_END()

    // --- ISpxSVModel ---
    virtual void InitModel(const std::shared_ptr<ISpxVoiceProfile>& profile) override;
    virtual std::shared_ptr<ISpxVoiceProfile> GetProfile() const override;

private:
    std::shared_ptr<ISpxVoiceProfile> m_profile;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
