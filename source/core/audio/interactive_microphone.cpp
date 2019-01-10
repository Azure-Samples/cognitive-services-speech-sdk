//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interactive_microphone.cpp: Implementation definitions for CSpxInteractiveMicrophone C++ class
//

#include "stdafx.h"
#include "interactive_microphone.h"
#include "site_helpers.h"
#include "create_object_helpers.h"
#include "microphone_pump.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {
    
CSpxInteractiveMicrophone::CSpxInteractiveMicrophone() :
    ISpxDelegateAudioPumpImpl()
{
}

void CSpxInteractiveMicrophone::Init()
{
    if (m_delegateToAudioPump == nullptr)
    {
        auto site = SpxSiteFromThis(this);
        m_delegateToAudioPump = SpxCreateObjectWithSite<ISpxAudioPump>("CSpxMicrophonePump", site);
    }
}

// relay the property query to its site, which is CSpxAudioStreamSession
std::shared_ptr<ISpxNamedProperties> CSpxInteractiveMicrophone::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
