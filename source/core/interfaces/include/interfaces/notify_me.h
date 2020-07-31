//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <memory>

#include <interfaces/base.h>
#include <interfaces/audio.h>
#include <interfaces/data.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <typename... Ts>
class ISpxNotifyMe : public ISpxInterfaceBaseFor<ISpxNotifyMe<Ts...>>
{
public:

    virtual void NotifyMe(Ts...) = 0;
};

template <class I, class ...Is>
using ISpxNotifyMeSP = ISpxNotifyMe<const std::shared_ptr<I>&, const std::shared_ptr<Is>&...>;

using ISpxAudioProcessorNotifyMe = ISpxNotifyMeSP<ISpxAudioProcessor>;

using ISpxAudioSourceNotifyMe = ISpxNotifyMeSP<ISpxAudioSource, ISpxBufferData>;

} } } }