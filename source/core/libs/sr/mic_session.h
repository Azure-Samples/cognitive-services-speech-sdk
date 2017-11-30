//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <spxcore_common.h>
#include "session.h"
#include "microphone.h"


namespace CARBON_IMPL_NAMESPACE() {

class CSpxMicSession : public CSpxAudioSession
{
public:

    CSpxMicSession() : CSpxAudioSession()
    {
        m_audioPump = Microphone::Create();
    }

private:
    CSpxMicSession(const CSpxMicSession&) = delete;
    CSpxMicSession(const CSpxMicSession&&) = delete;

    CSpxMicSession& operator=(const CSpxMicSession&) = delete;
};

}; // CARBON_IMPL_NAMESPACE()
