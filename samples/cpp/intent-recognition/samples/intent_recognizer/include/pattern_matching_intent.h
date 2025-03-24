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

struct IntentPattern
{
    std::string Pattern;
    std::vector<std::string> Entities;
    std::string Phrase;
};

class CSpxPatternMatchingIntent : public ISpxTrigger
{
public:

    void Init(const std::string& id, unsigned int priority, const std::string& orthographyPrefix);
    void AddPhrase(const std::string& phrase);

    inline const std::string& GetId() const { return m_id; }
    inline const std::vector<IntentPattern>& GetPatterns() const { return m_patterns; }
    inline unsigned int GetPriority() const { return m_priority; }
    inline void AddPatterns(const std::vector<IntentPattern>& patterns) { m_patterns.insert(m_patterns.end(), patterns.begin(), patterns.end()); }

    // --- ISpxTrigger
    inline void InitPhraseTrigger(const std::string& phrase) override { (void)(phrase); }
    inline void InitPatternMatchingModelTrigger(const std::shared_ptr<CSpxPatternMatchingModel>& model) override { (void)(model); }
    inline const std::string GetPhrase() const override { return ""; }
    inline std::shared_ptr<CSpxPatternMatchingModel> GetModel() const override { return nullptr; }

private:
    static std::vector<std::string> ExtractEntities(const std::string& input);

    unsigned int m_priority = 0;
    std::string m_id;
    std::string m_orthographyPrefix;
    std::vector<IntentPattern> m_patterns;

};

}}}}}
