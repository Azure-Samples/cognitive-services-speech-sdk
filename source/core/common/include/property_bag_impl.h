//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// property_bag_impl.h: Implementation definitions for ISpxNamedPropertiesImpl C++ class
//

#pragma once
#include <map>
#include "ispxinterfaces.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxPropertyBagImpl : public ISpxNamedProperties
{
public:
    // --- ISpxNamedProperties
    void Copy(ISpxNamedProperties* from) override;

    std::string GetStringValue(const char* name, const char* defaultValue = "") const override;

    void SetStringValue(const char* name, const char* value) override;

    bool HasStringValue(const char* name) const override;

    CSpxStringMap FindPrefix(const char* prefix_) const override;

protected:
    virtual std::shared_ptr<ISpxNamedProperties> GetParentProperties() const
    {
        return nullptr;
    }

private:
    mutable std::mutex m_mutexProperties;
    std::map<std::string, std::string> m_stringPropertyMap;

    void LogPropertyAndValue(std::string name, std::string value) const;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
