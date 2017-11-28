//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {

class Microphone
{
public:
    static std::shared_ptr<ISpxAudioPump> Create();
private:
    Microphone() = delete;
};

}; // CARBON_IMPL_NAMESPACE()
