//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once
#include <string>

#include "intent_interfaces.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class CSpxIntegerEntity : public ISpxEntity
{
public:

    // --- ISpxEntity ---
    virtual void Init(const std::string& name, const OrthographyInformation& orthography) override;
    virtual void SetMode(Intent::EntityMatchMode mode) override;
    virtual void AddPhrase(const std::string& phrase) override;

    // --- ISpxEntity ---
    const std::string& GetName() const override;
    Intent::EntityMatchMode GetMode() const override;
    Intent::EntityType GetType() const override;
    unsigned int GetGreed() const override { return m_greed; }
    bool IsRequired() const override { return true; }
    Maybe<std::string> Parse(const std::string& input) const override;

private:

    std::shared_ptr<ISpxIntegerParser> m_integerParser;

    unsigned int m_greed = 0;
    std::string m_name;
};

}}}}}
