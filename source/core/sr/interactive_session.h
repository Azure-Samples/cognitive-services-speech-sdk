//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interactive_session.h: Implementation declarations for CSpxInteractiveSession C++ class
//

#pragma once
#include <spxcore_common.h>
#include "mic_session.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxInteractiveSession : public CSpxMicSession
{
public:

    CSpxInteractiveSession() = default;


private:

    CSpxInteractiveSession(const CSpxInteractiveSession&) = delete;
    CSpxInteractiveSession(const CSpxInteractiveSession&&) = delete;

    CSpxInteractiveSession& operator=(const CSpxInteractiveSession&) = delete;
};


}; // CARBON_IMPL_NAMESPACE()
