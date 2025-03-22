//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <string>
#include <map>
#include <algorithm>

#include "intent_match_result.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

void CSpxIntentMatchResult::InitIntentMatchResult(
    std::string intentId,
    std::string pattern,
    const std::map<std::string, EntityResult>& entities,
    unsigned int priority,
    unsigned int bytesMatched)
{
    m_intentId = intentId;
    m_pattern = pattern;
    m_entities = entities;
    m_priority = priority;
    m_bytesMatched = bytesMatched;
}

int64_t SpxIntentMatchResultCompare::CountEntityType(const std::map<std::string, EntityResult>& entities, Intent::EntityType type)
{
    return std::count_if(entities.begin(), entities.end(), [&](auto& e)
        {
            return (e.second.Type == type);
        });
}

// returns true if one should be higher priority.
bool SpxIntentMatchResultCompare::operator()(std::shared_ptr<CSpxIntentMatchResult> one, std::shared_ptr<CSpxIntentMatchResult> two) const
{
    if (one->GetPriority() < two->GetPriority())
    {
        return true;
    }

    if (one->GetPriority() == two->GetPriority())
    {
        // Compare based on number of entities. 0 entities takes priority because it means it is an exact pattern.
        auto oneSize = one->GetEntities().size();
        auto twoSize = two->GetEntities().size();
        if ((oneSize == 0) && (twoSize != 0))
        {
            return true;
        }

        if (oneSize > twoSize && twoSize != 0)
        {
            return true;
        }

        // Compare based on entity types starting with PrebuiltIntegers.
        if (oneSize == twoSize)
        {
            auto thisPrebuiltIntegerCount = CountEntityType(one->GetEntities(), Intent::EntityType::PrebuiltInteger);
            auto thatPrebuiltIntergerCount = CountEntityType(two->GetEntities(), Intent::EntityType::PrebuiltInteger);
            if (thisPrebuiltIntegerCount > thatPrebuiltIntergerCount)
            {
                return true;
            }

            if (thisPrebuiltIntegerCount == thatPrebuiltIntergerCount)
            {
                auto thisListEntityCount = CountEntityType(one->GetEntities(), Intent::EntityType::List);
                auto thatListEntityCount = CountEntityType(two->GetEntities(), Intent::EntityType::List);
                if (thisListEntityCount > thatListEntityCount)
                {
                    return true;
                }

                if (thisListEntityCount == thatListEntityCount)
                {
                    // Don't need to compare Any types since they are the base and are equal.

                    //Break ties with bytesMatched
                    // Compare based on number of bytes matched.
                    if (one->GetBytesMatched() > two->GetBytesMatched())
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

}}}}}
