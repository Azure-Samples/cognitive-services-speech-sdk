//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "memory_utils.h"

using namespace Microsoft::CognitiveServices::Speech::Impl;

namespace Memory
{
    
void CheckObjectCount(SPXHANDLE hspeechconfig)
{
    auto config_handles = CSpxSharedPtrHandleTableManager::Get<ISpxSpeechConfig, SPXSPEECHCONFIGHANDLE>();

    auto config = (*config_handles)[hspeechconfig];
    auto config_property_bag = SpxQueryInterface<ISpxNamedProperties>(config);

    // Get the max object count to verify we're not over.
    size_t errorThreshold = stoul(config_property_bag->GetStringValue("SPEECH-ObjectCountErrorThreshold", "0"));
    if (0 == errorThreshold)
    {
        errorThreshold = SIZE_MAX;
    }

    size_t currentObjCount = CSpxSharedPtrHandleTableManager::GetTotalTrackedObjectCount();

    if (currentObjCount >= errorThreshold)
    {
        std::string errorMessage = "The maximum object count of " + std::to_string(errorThreshold) + " has been exceeded.\r\n";
        errorMessage += "The threshold can be adjusted by setting the SPEECH-ObjectCountErrorThreshold property on the SpeechConfig object.\r\n";
        errorMessage += "See http://https://aka.ms/csspeech/object-tracking for more detailed information.\r\n";
        errorMessage += "Handle table dump by ojbect type:\r\n";
        errorMessage += CSpxSharedPtrHandleTableManager::GetHandleCountByType();
        ThrowRuntimeError(errorMessage, 0);
    }

    size_t warnThreshold = stoul(config_property_bag->GetStringValue("SPEECH-ObjectCountWarnThreshold", "10000"));

    if (currentObjCount >= warnThreshold)
    {
        std::string errorMessage = "Object count exceeded the warning threshold" + std::to_string(warnThreshold) + ".\r\n";
        errorMessage += "Handle table dump by ojbect type:\\n";
        errorMessage += CSpxSharedPtrHandleTableManager::GetHandleCountByType();
        SPX_TRACE_WARNING("%s", errorMessage.c_str());
    }
}
}
