//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <regex>
#include <stack>
#include <cmath>
#include <cstring>
#include <sstream>
#include <stdlib.h>

#include "en_integer_parser.h"
#include "es_integer_parser.h"
#include "fr_integer_parser.h"
#include "ja_integer_parser.h"
#include "zh_integer_parser.h"
#include "locale_information.h"
#include "integer_entity.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class NoOpIntegerParser : public ISpxIntegerParser
{
public:
    virtual Maybe<std::string> Parse(const std::string&) const override
    {
        return Maybe<std::string>();
    }
};

void CSpxIntegerEntity::SetMode(Intent::EntityMatchMode)
{
    // this is a no-op for integer entities
}

void CSpxIntegerEntity::AddPhrase(const std::string&)
{
    // this is a no-op for integer entities
}

const std::string& CSpxIntegerEntity::GetName() const
{
    return m_name;
}

Intent::EntityMatchMode CSpxIntegerEntity::GetMode() const
{
    return Intent::EntityMatchMode::Basic;
}

Intent::EntityType CSpxIntegerEntity::GetType() const
{
    return Intent::EntityType::PrebuiltInteger;
}

void CSpxIntegerEntity::Init(const std::string& name, const OrthographyInformation& orthography)
{
    m_name = name;
    if (orthography.Name == "en")
    {
        m_integerParser = std::static_pointer_cast<ISpxIntegerParser>(std::make_shared<CSpxENIntegerParser>());
    }
    else if (orthography.Name == "es")
    {
        m_integerParser = std::static_pointer_cast<ISpxIntegerParser>(std::make_shared<CSpxESIntegerParser>());
    }
    else if (orthography.Name == "ja")
    {
        m_integerParser = std::static_pointer_cast<ISpxIntegerParser>(std::make_shared<CSpxJPIntegerParser>());
    }
    else if (orthography.Name == "fr")
    {
        m_integerParser = std::static_pointer_cast<ISpxIntegerParser>(std::make_shared<CSpxFRIntegerParser>());
    }
    else if (orthography.Name == "zh")
    {
        m_integerParser = std::static_pointer_cast<ISpxIntegerParser>(std::make_shared<CSpxZHIntegerParser>());
    }
    else
    {
        // fall back to default and use English
        SPX_TRACE_ERROR(
            "No explicit integer parser for '%s' language. Disabling integer parsing",
            orthography.Name.c_str());
        m_integerParser = std::static_pointer_cast<ISpxIntegerParser>(std::make_shared<NoOpIntegerParser>());
    }
}

Maybe<std::string> CSpxIntegerEntity::Parse(const std::string& input) const
{
    return m_integerParser->Parse(input);
}

}}}}}
