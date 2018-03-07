//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// interactive_microphone.cpp: Implementation definitions for CSpxInteractiveMicrophone C++ class
//

#include "stdafx.h"
#include "microphone.h"
#include "interactive_microphone.h"


namespace CARBON_IMPL_NAMESPACE() {


CSpxInteractiveMicrophone::CSpxInteractiveMicrophone() :
    ISpxDelegateAudioPumpImpl(Microphone::Create())
{
}


} // CARBON_IMPL_NAMESPACE
