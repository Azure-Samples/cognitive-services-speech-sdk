//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "microphone.h"
#include "ispxinterfaces.h"

namespace CARBON_IMPL_NAMESPACE() {

using namespace std;

shared_ptr<ISpxAudioPump> Microphone::Create() 
{
    // TODO (alrezni)
    SPX_THROW_HR(SPXERR_NOT_IMPL);
}

}; // CARBON_IMPL_NAMESPACE()


