//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_notify_me_track_delegate_impl.h: Implementation declarations for ISpxAudioSourceNotifyMeTrackDelegateImpl C++ class
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include "spxcore_common.h"
#include "audio_source_notify_me_tracker.h"
#include "interface_delegate_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename DelegateToHelperT = CSpxDelegateToSharedPtrHelper<ISpxAudioSourceNotifyMe>>
class ISpxAudioSourceNotifyMeTrackDelegateImpl :
    public DelegateToHelperT,
    public CSpxAudioSourceNotifyMeTracker,
    public ISpxAudioSourceNotifyMe
{
private:
    using I = ISpxAudioSourceNotifyMe;
    using C = ISpxAudioSourceNotifyMeTrackDelegateImpl;
public:

    void NotifyMe(const std::shared_ptr<ISpxAudioSource>& source, const std::shared_ptr<ISpxBufferData>& data)
    {
        TrackNotifyMe(source, data);
        InvokeOnDelegate(C::GetDelegate(), &I::NotifyMe, source, data);
    }
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
