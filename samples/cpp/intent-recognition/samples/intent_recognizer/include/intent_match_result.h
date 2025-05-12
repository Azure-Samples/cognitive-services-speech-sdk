//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once
#include <string>
#include <map>

#include "intent_interfaces.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class CSpxIntentMatchResult;

struct SpxIntentMatchResultCompare
{
    bool operator()(std::shared_ptr<CSpxIntentMatchResult> one, std::shared_ptr<CSpxIntentMatchResult> two) const;
    static int64_t CountEntityType(const std::map<std::string, EntityResult>& entities, Intent::EntityType type);
};

class CSpxIntentMatchResult
{

public:
    ~CSpxIntentMatchResult() = default;

    void InitIntentMatchResult(
        std::string intentId,
        std::string pattern,
        const std::map<std::string, EntityResult>& entities,
        unsigned int priority,
        unsigned int bytesMatched);

    const std::map<std::string, EntityResult>& GetEntities() const&
    {
        return m_entities;
    }

    const std::string& GetIntentId() const&
    {
        return m_intentId;
    }

    const std::string& GetPattern() const&
    {
        return m_pattern;
    }

    unsigned int GetPriority() const&
    {
        return m_priority;
    }

    unsigned int GetBytesMatched() const&
    {
        return m_bytesMatched;
    }

private:

    unsigned int m_priority = UINT32_MAX;
    unsigned int m_bytesMatched = 0;
    std::string m_intentId;
    std::map<std::string, EntityResult> m_entities{};
    std::string m_pattern;
};

}}}}}
