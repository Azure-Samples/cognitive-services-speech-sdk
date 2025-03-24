//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "pattern_any_entity.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

void CSpxPatternAnyEntity::Init(const std::string& name, const OrthographyInformation&)
{
    m_name = name;
}

void CSpxPatternAnyEntity::SetMode(Intent::EntityMatchMode)
{
    // this is a no-op for "any" entities
}

void CSpxPatternAnyEntity::AddPhrase(const std::string&)
{
    // this is a no-op for "any" entities
}

const std::string& CSpxPatternAnyEntity::GetName() const
{
    return m_name;
}

Intent::EntityMatchMode CSpxPatternAnyEntity::GetMode() const
{
    return Intent::EntityMatchMode::Basic;
}

Intent::EntityType CSpxPatternAnyEntity::GetType() const
{
    return Intent::EntityType::Any;
}

Maybe<std::string> CSpxPatternAnyEntity::Parse(const std::string& input) const
{
    if (input.empty())
    {
        return Maybe<std::string>();
    }
    return input;
}

}}}}}
