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

class CSpxIntentTrigger : public ISpxTrigger
{
public:

    // --- ISpxTrigger ---
    void InitPhraseTrigger(const std::string& phrase) override;
    void InitPatternMatchingModelTrigger(const std::shared_ptr<CSpxPatternMatchingModel>& model) override;

    const std::string GetPhrase() const override
    {
        return m_phrase;
    }

    std::shared_ptr<CSpxPatternMatchingModel> GetModel() const override
    {
        return m_model;
    }

    static std::string NormalizeInput(const std::string& inputText);

private:

    // this is the phrase as given when creating the intent trigger.
    std::string m_phrase;

    // this is the pattern used for regex searches of incoming phrases.
    std::string m_pattern;

    std::shared_ptr<CSpxPatternMatchingModel> m_model;
    std::string m_intentName;
    std::list<std::string> m_entities;
};

}}}}}
