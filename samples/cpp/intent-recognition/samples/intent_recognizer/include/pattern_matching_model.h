//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once
#include <array>
#include <mutex>
#include <string>
#include "intent_interfaces.h"
#include "locale_information.h"
#include "pattern_matching_intent.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class CSpxPatternMatchingModel;

class CSpxPatternMatchingFactory
{
public:

    void Init(const std::shared_ptr<CSpxPatternMatchingModel>& model, const std::string& language);
    std::shared_ptr<ISpxEntity> CreateEntity(Intent::EntityType type) const;
    void AddEntity(const std::shared_ptr<ISpxEntity>& entity);
    std::shared_ptr<CSpxPatternMatchingIntent> CreateIntent() const;
    void AddIntent(const std::shared_ptr<CSpxPatternMatchingIntent>& intent);

private:

    std::shared_ptr<CSpxPatternMatchingModel> m_model;
};

class CSpxPatternMatchingModel
{
public:

    CSpxPatternMatchingModel(const std::string& id);
    ~CSpxPatternMatchingModel();

    void Init(const std::string& language);

    void AddIntent(std::shared_ptr<CSpxPatternMatchingIntent> intent, const std::string& intentId);
    void AddEntity(std::shared_ptr<ISpxEntity> entity);
    virtual const OrthographyInformation& GetOrthographyInfo() const;
    std::vector<std::shared_ptr<CSpxIntentMatchResult>> FindMatches(const std::string& phrase);

    const std::string& GetId() const;

private:
    friend class CSpxPatternMatchingFactory;

    Maybe<std::shared_ptr<CSpxIntentMatchResult>> CheckPattern(
        const char* input,
        const char* patternText,
        const IntentPattern& intentPattern,
        const char* intentId,
        unsigned int intentPriority,
        std::map<std::string, Impl::EntityResult>& entityResults,
        unsigned int bytesPreviouslyMatched);

    void StoreEntityResult(const std::string& entityName, std::string& entityValue, std::map<std::string, EntityResult>& entityResults, bool& requiredEntityPresent);

    /// <summary>
    /// This will parse the input for the optional phrases. It will put all phrases in the vector it returns. The pointer will be moved to the end of the optional phrase including the ']'.
    /// </summary>
    /// <param name="input">This should be a null terminated character array pointer.</param>
    /// <returns>A vector containing all of the optional phrases.</returns>
    std::vector<std::string> ParseGroupedPhrases(const char** input);

    std::mutex m_mutex;
    std::string m_id;
    std::map<std::string, std::shared_ptr<CSpxPatternMatchingIntent>> m_intentMap;
    std::map<std::string, std::shared_ptr<ISpxEntity>> m_entityMap;

    const OrthographyInformation* m_orthography = &Locales::default_orthography();
};

}}}}}
