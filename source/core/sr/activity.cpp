//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// activity.cpp: Implementation definitions for Activity related classes.
//
#include "stdafx.h"
#include "activity.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

nlohmann::json& CSpxActivityJSONAccessor::Get()
{
    return m_accessor();
}

void CSpxActivityJSONAccessor::SetAccessor(accessor_fn accessor)
{
    m_accessor = accessor;
}

nlohmann::json& CSpxActivity::GetJSON()
{
    return m_json;
}

void CSpxActivity::LoadJSON(const nlohmann::json& json)
{
    m_json = json;
}

} } } }
