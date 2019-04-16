//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// activity.h: Implementation declarations for Activity related classes.
//
#pragma once

#include "ispxinterfaces.h"
#include "interface_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxActivity;

class CSpxActivityJSONAccessor:
    public ISpxActivityJSONAccessor
{
public:
    using accessor_fn = std::function<nlohmann::json&()>;

    CSpxActivityJSONAccessor() = default;
    CSpxActivityJSONAccessor(const CSpxActivityJSONAccessor&) = delete;
    CSpxActivityJSONAccessor(CSpxActivityJSONAccessor&&) = delete;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxActivityJSONAccessor)
    SPX_INTERFACE_MAP_END()

    nlohmann::json& Get() final;

    void SetAccessor(accessor_fn accessor) final;

    friend class CSpxActivity;
private:
    accessor_fn m_accessor;
};

class CSpxActivity:
    public ISpxActivity
{
public:
    CSpxActivity() = default;
    CSpxActivity(const CSpxActivity&) = delete;
    CSpxActivity(CSpxActivity&&) = delete;

    SPX_INTERFACE_MAP_BEGIN()
        SPX_INTERFACE_MAP_ENTRY(ISpxActivity)
    SPX_INTERFACE_MAP_END()

    nlohmann::json& GetJSON() final;
    void LoadJSON(const nlohmann::json& json) final;
private:
    nlohmann::json m_json;
};

} } } }
