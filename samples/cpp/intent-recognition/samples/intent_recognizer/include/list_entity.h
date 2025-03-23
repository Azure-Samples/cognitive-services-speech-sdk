//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once
#include <string>

#include "intent_interfaces.h"
#include "locale_information.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class CSpxListEntity : public ISpxEntity
{
public:

    // --- ISpxEntity ---
    virtual void Init(const std::string& name, const OrthographyInformation& orthography) override;
    virtual void SetMode(Intent::EntityMatchMode mode) override;
    virtual void AddPhrase(const std::string& phrase) override;

    const std::string& GetName() const override;
    Intent::EntityMatchMode GetMode() const override;
    Intent::EntityType GetType() const override;
    unsigned int GetGreed() const override { return m_greed; }
    bool IsRequired() const override { return (m_matchMode == Intent::EntityMatchMode::Strict); }
    Maybe<std::string> Parse(const std::string& input) const override;

private:
    static unsigned int CalculateGreed(const OrthographyInformation& orthography, const std::string& phrase);

    unsigned int m_greed;
    std::string m_name;
    Intent::EntityMatchMode m_matchMode;
    std::vector<std::string> m_phrases;
    const OrthographyInformation* m_orthography;
};

}}}}}
