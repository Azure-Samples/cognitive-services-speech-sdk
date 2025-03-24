//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <assert.h>
#include <memory>
#include <regex>
#include <sstream>

#include "integer_entity.h"
#include "intent_match_result.h"
#include "list_entity.h"
#include "pattern_any_entity.h"
#include "pattern_matching_intent.h"
#include "pattern_matching_model.h"
#include "pattern_matching_utils.h"
#include "string_utils.h"
#include "utf8_utils.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

void CSpxPatternMatchingFactory::Init(const std::shared_ptr<CSpxPatternMatchingModel>& model, const std::string& language)
{
    if (model == nullptr)
    {
        throw std::invalid_argument("CSpxPatternMatchingFactory: model == nullptr");
    }
    m_model = model;
    m_model->Init(language);
}

std::shared_ptr<ISpxEntity> CSpxPatternMatchingFactory::CreateEntity(Intent::EntityType type) const
{
    std::shared_ptr<ISpxEntity> entity;
    switch (type)
    {
    case Intent::EntityType::Any:
        entity = std::shared_ptr<ISpxEntity>(new CSpxPatternAnyEntity());
        break;

    case Intent::EntityType::List:
        entity = std::shared_ptr<ISpxEntity>(new CSpxListEntity());
        break;

    case Intent::EntityType::PrebuiltInteger:
        entity = std::shared_ptr<ISpxEntity>(new CSpxIntegerEntity());
        break;
    }

    if (entity == nullptr)
    {
        throw std::invalid_argument("CSpxPatternMatchingFactory: entity == nullptr");
    }

    return entity;
}

void CSpxPatternMatchingFactory::AddEntity(const std::shared_ptr<ISpxEntity>& entity)
{
    m_model->m_entityMap[entity->GetName()] = entity;
}

std::shared_ptr<CSpxPatternMatchingIntent> CSpxPatternMatchingFactory::CreateIntent() const
{
    return std::make_shared<CSpxPatternMatchingIntent>();
}

void CSpxPatternMatchingFactory::AddIntent(const std::shared_ptr<CSpxPatternMatchingIntent>& intent)
{
    // Let's use the completely intuitive and not at all confusing C++ insert method to do a
    // "get or add" with one dictionary lookup
    auto result = m_model->m_intentMap.insert(std::make_pair(intent->GetId(), intent));
    bool wasAdded = result.second;
    auto inMap = result.first->second;

    if (!wasAdded)
    {
        // We already had an entry so let's add the phrases to that one
        inMap->AddPatterns(intent->GetPatterns());
    }
}


CSpxPatternMatchingModel::CSpxPatternMatchingModel(const std::string& id) :
    m_id(id)
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxPatternMatchingModel::~CSpxPatternMatchingModel()
{
    SPX_DBG_TRACE_FUNCTION();
}

const std::string& CSpxPatternMatchingModel::GetId() const
{
    return m_id;
}

void CSpxPatternMatchingModel::Init(const std::string& language)
{
    auto speechOrthography = PAL::StringUtils::ToLower(language);
    speechOrthography = speechOrthography.substr(0, speechOrthography.find_first_of('-'));

    m_orthography = Locales::find_orthography(speechOrthography);
    if (m_orthography == nullptr)
    {
        m_orthography = &Locales::default_orthography();
    }

    assert(m_orthography != nullptr);
}

std::vector<std::shared_ptr<CSpxIntentMatchResult>> CSpxPatternMatchingModel::FindMatches(const std::string& phrase)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    std::vector<std::shared_ptr<CSpxIntentMatchResult>> intentResults;

    // Trim the string of sentence ending characters. This is necessary because SR might add characters to the utterance
    // that we don't want to include in greedy entities.
    auto trimmedPhrase = phrase;

    // French is special because they use non-breaking spaces before and after punctuation.
    if (PAL::stricmp(m_orthography->Name.c_str(), "fr") == 0)
    {
        trimmedPhrase = Impl::Locales::Utils::RemoveLeadingPunctuationSpaceFR(trimmedPhrase);
    }

    Utils::TrimUTF8SentenceEndCharacters(trimmedPhrase, *m_orthography);

    for (auto& intent : m_intentMap)
    {
        auto intentPatterns = intent.second->GetPatterns();
        for (auto& intentPattern : intentPatterns)
        {
            // Create the reference for entityResults here so it can be used in all the recursive calls.
            std::map<std::string, Impl::EntityResult> entityResults;
            auto matchResult = CheckPattern(
                trimmedPhrase.c_str(),
                intentPattern.Pattern.c_str(),
                intentPattern,
                intent.first.c_str(),
                intent.second->GetPriority(),
                entityResults,
                0);
            if (matchResult)
            {
                // Add our match to the result set.
                intentResults.push_back(matchResult.Get());
            }
        }
    }

    return intentResults;
}

Maybe<std::shared_ptr<CSpxIntentMatchResult>> CSpxPatternMatchingModel::CheckPattern(
    const char* input,
    const char* patternText,
    const IntentPattern& intentPattern,
    const char* intentId,
    unsigned int intentPriority,
    std::map<std::string, Impl::EntityResult>& entityResults,
    unsigned int bytesPreviouslyMatched)
{

    const char* inputLocation = input;
    const char* patternLocation = patternText;
    bool requiredEntityPresent = true;
    std::string entityValue = "";
    unsigned int entityGreedLevel = 0;
    unsigned int entityWords = 0;
    unsigned int bytesMatched = bytesPreviouslyMatched;

    // Move past any pattern punctuation
    Utils::SkipPatternPunctuationAndWhitespace(patternLocation, *m_orthography);

    // If the pattern is empty and input is empty, then by default we match!
    if ((patternLocation == nullptr || patternLocation[0] == '\0') &&
        (input == nullptr || input[0] == '\0'))
    {
        auto intentMatchResult = std::make_shared<CSpxIntentMatchResult>();

        intentMatchResult->InitIntentMatchResult(intentId, intentPattern.Phrase, entityResults, intentPriority, bytesMatched);
        return Maybe<std::shared_ptr<CSpxIntentMatchResult>>(intentMatchResult);
    }

    // If the pattern isn't empty but the input is, we don't match.
    if (input == nullptr ||
        input[0] == '\0')
    {
        return Maybe<std::shared_ptr<CSpxIntentMatchResult>>();
    }

    while ((inputLocation != nullptr && patternLocation != nullptr) &&
        (*inputLocation != '\0' || *patternLocation != '\0'))
    {
        // Special case for space after optional pattern before an any entity.
        if (*patternLocation == 'S')
        {
            // This means the input needs a space or word boundary here.
            if (!Utils::IsWordBoundary(inputLocation, *m_orthography))
            {
                break;
            }
            else
            {
                auto count = Utils::CountNumCharacters(m_orthography->WordBoundary);
                patternLocation += count;
                bytesMatched += (unsigned int)count;
                continue;
            }
        }

        // Check if we should skip something in the baseInput.
        Utils::SkipInputPunctuationAndWhitespace(inputLocation, *m_orthography);

        // Check if we should skip something in the patternInput.
        Utils::SkipPatternPunctuationAndWhitespace(patternLocation, *m_orthography);

        // Let's check if we are expecting an entity.
        if (*patternLocation == '{')
        {
            // Initialize our entity values;
            entityGreedLevel = 0;
            entityValue = "";
            entityWords = 0;

            // Advance past the '{'
            patternLocation++;

            std::string entityName = "";
            // Copy the entityName.
            while (*patternLocation != '}' && *patternLocation != '\0')
            {
                entityName += *patternLocation;
                patternLocation++;
            }
            if (*patternLocation == '\0')
            {
                // We should never get here, but if we do, trace the error.
                std::ostringstream msg;
                msg << "Unmatched '{' in pattern: " << intentPattern.Phrase;
                SPX_TRACE_ERROR(msg.str().c_str());
                throw std::invalid_argument(msg.str());
            }

            // Skip past the '}'.
            patternLocation++;

            // Grab the case-sensitive entity name from the intent
            auto intentEntityEntry = std::find_if(intentPattern.Entities.begin(), intentPattern.Entities.end(), [entityName](auto& value)
                {
                    if (PAL::stricmp(value.c_str(), entityName.c_str()) == 0)
                    {
                        return true;
                    }
                    return false;
                });

            if (intentEntityEntry != intentPattern.Entities.end())
            {
                entityName = *intentEntityEntry;
            }
            else
            {
                // This should never happen since we pulled the name from the pattern, but if it does, throw an exception.
                std::ostringstream msg;
                msg << "Unknown entity in pattern: " << intentPattern.Phrase;
                SPX_TRACE_ERROR(msg.str().c_str());
                throw std::invalid_argument(msg.str());
            }

            // Find the greed level for our entity.
            auto entityClassName = entityName.substr(0, entityName.find_first_of(":"));
            auto entityInMap = m_entityMap.find(entityClassName);
            if (entityInMap != m_entityMap.end())
            {
                entityGreedLevel = entityInMap->second->GetGreed();
            }
            else
            {
                entityGreedLevel = 0;
            }

            std::string nextBasePhrase = "";
            // First make sure we skip whitespace before any next phrase.
            Utils::SkipPatternPunctuationAndWhitespace(patternLocation, *m_orthography);

            // Find out the next whole word from the base input.
            const char* inputLocation2 = inputLocation;
            nextBasePhrase = Utils::GrabNextWord(&inputLocation2, *m_orthography);

            // Grab at least one word for the entity.
            if (entityValue.empty())
            {
                entityValue += nextBasePhrase;
                entityWords++;
                // Move inputLocation up.
                inputLocation = inputLocation2;

                // If we are at the end, store the entity.
                if (*inputLocation == '\0')
                {
                    // Store the entity result.
                    StoreEntityResult(entityName, entityValue, entityResults, requiredEntityPresent);
                    if (!requiredEntityPresent)
                    {
                        break;
                    }

                    // Advance past unimportant characters. This is to account for unimportant characters in the pattern
                    // that occur after the match.
                    Utils::SkipPatternPunctuationAndWhitespace(patternLocation, *m_orthography);

                    // There might be optional phrases after the entity in the pattern, so check if this matches without them.
                    if (*patternLocation == '[')
                    {
                        auto patternLocation2 = patternLocation;
                        // Grab the optional phrases but only move patternLocation2.
                        auto possiblePhrases = ParseGroupedPhrases(&patternLocation2);
                        // If there is more left at the end of the pattern, this doesn't match and we should cleanup the entityResults.
                        if (*patternLocation2 != '\0')
                        {
                            if (entityResults.find(entityName) != entityResults.end())
                            {
                                entityResults.erase(entityName);
                            }
                            break;
                        }

                        // Only optional phrases are left. Update patternLocation and continue loop.
                        patternLocation = patternLocation2;
                        continue;
                    }

                    // If there is more left at the end of the pattern, this doesn't match and we should cleanup the entityResults.
                    if (*patternLocation != '\0')
                    {
                        if (entityResults.find(entityName) != entityResults.end())
                        {
                            entityResults.erase(entityName);
                        }
                        break;
                    }
                    continue;
                }

                if (entityGreedLevel >= 1)
                {
                    // If we know our greed, check if our entity is valid.
                    while (entityWords <= entityGreedLevel)
                    {
                        // This is a no-op on the first pass but will ensure input doesn't get skipped when the number of words exceeds the greed.
                        inputLocation = inputLocation2;

                        StoreEntityResult(entityName, entityValue, entityResults, requiredEntityPresent);

                        if (requiredEntityPresent)
                        {
                            // If this was a valid entity check the rest of the pattern to make sure we didn't grab too many words.
                            auto result = CheckPattern(inputLocation, patternLocation, intentPattern, intentId, intentPriority, entityResults, bytesMatched);
                            if (result)
                            {
                                // Woah! It all worked out and we have a match!
                                return result;
                            }
                        }
                        else
                        {
                            if (entityWords != entityGreedLevel)
                            {
                                // This just means our entity may need more words. So let the loop go through again.
                                requiredEntityPresent = true;
                            }
                            else
                            {
                                // We grabbed all the words allowed and still the required entity isn't there.
                                break;
                            }
                            if (entityResults.find(entityName) != entityResults.end())
                            {
                                entityResults.erase(entityName);
                            }
                        }
                        // Find out the next whole word from the base input.
                        nextBasePhrase = Utils::GrabNextWord(&inputLocation2, *m_orthography);

                        entityValue += m_orthography->WordBoundary.data() + nextBasePhrase;
                        entityWords++;

                        continue;
                    }

                    // This means we broke out of the loop because a required entity didn't match. Break out of the main loop too.
                    if (!requiredEntityPresent)
                    {
                        break;
                    }
                }
                else if (entityGreedLevel == 0)
                {
                    // We are greedy, grab as much as possible. But if our match fails, walk it back to make sure we didn't grab too much.
                    while (*inputLocation != '\0')
                    {
                        entityValue += *inputLocation++;
                    }

                    // Can't use entityWords here since we grabbed everything and didn't count.
                    while (!entityValue.empty())
                    {
                        StoreEntityResult(entityName, entityValue, entityResults, requiredEntityPresent);
                        // No need to check requiredEntity here since we might have grabbed too much.

                        // Check to see if the rest of the pattern matches.
                        auto result = CheckPattern(inputLocation, patternLocation, intentPattern, intentId, intentPriority, entityResults, bytesMatched);

                        // Now check if everything is good.
                        if (result && entityResults.find(entityName) != entityResults.end() && requiredEntityPresent)
                        {
                            // Woah! It all worked out and we have a match!
                            return result;
                        }
                        else if (!m_orthography->Whitespace.empty() &&
                            entityValue.find(m_orthography->WordBoundary.data(), 0, Utils::CountNumCharacters(m_orthography->WordBoundary)) == std::string::npos)
                        {
                            // This means with even just one word, the result doesn't match so we need to exit since this isn't a match.
                            return Maybe<std::shared_ptr<CSpxIntentMatchResult>>();
                        }
                        else
                        {
                            // Cleanup the results
                            entityResults.erase(entityName);
                            requiredEntityPresent = true;

                            // Remove the last "token" of the entity value and move the pattern back.
                            auto strippedSize = Utils::RemoveLastToken(entityValue, *m_orthography);
                            inputLocation -= strippedSize;
                        }
                    }
                    continue;
                }

                continue;
            }

            // If we ever have more words than the greed, we know it's too many.
            if (entityGreedLevel != 0 && entityWords > entityGreedLevel)
            {
                break;
            }
        }

        // Let's check if we are entering an possiblePhrase block.
        if (*patternLocation == '[')
        {
            // Grab the optional phrases.
            auto possiblePhrases = ParseGroupedPhrases(&patternLocation);

            for (auto& possiblePhrase : possiblePhrases)
            {
                // Let's treat each possiblePhrase as a separate possible pattern.
                std::string newPattern;
                if (*possiblePhrase.begin() != '{' &&
                    *patternLocation != '\0' &&
                    Utils::IsWordBoundary(patternLocation, *m_orthography) &&
                    strlen(patternLocation) > 1 &&
                    *(patternLocation + 1) != '[')
                {
                    // pattern: "Click [on] {application}"
                    // input:   "Click onedrive"
                    // This is a very special case where the space after the possible phrase should be matched.
                    // NOTE: This anchor may need to change if 'S' conflicts with a lower case character in a
                    // supported locale.
                    newPattern = possiblePhrase + "S" + patternLocation;
                }
                else
                {
                    newPattern = possiblePhrase + patternLocation;
                }
                auto result = CheckPattern(inputLocation, newPattern.c_str(), intentPattern, intentId, intentPriority, entityResults, bytesMatched);
                if (result)
                {
                    // Woah! It all worked out and we have a match!
                    return result;
                }
            }
            // If we don't have a match yet, just continue since it was an possiblePhrase set and not required.
            continue;
        }

        // Let's check for a required group.
        if (*patternLocation == '(')
        {
            // Grab the required phrases.
            auto possiblePhrases = ParseGroupedPhrases(&patternLocation);

            for (auto& possiblePhrase : possiblePhrases)
            {
                // Let's treat each possiblePhrase as a separate possible pattern.
                std::string newPattern = possiblePhrase + patternLocation;
                auto result = CheckPattern(inputLocation, newPattern.c_str(), intentPattern, intentId, intentPriority, entityResults, bytesMatched);
                if (result)
                {
                    // Woah! It all worked out and we have a match!
                    return result;
                }
            }
            // If we have gotten here then no required phrases are present and this isn't a match.
            break;
        }

        // Check if we are matching. If so, move both pointers.
        // We need to check in this order, because only input or pattern location could be null, but not both.
        if ((*inputLocation == *patternLocation) && *inputLocation != '\0')
        {
            inputLocation++;
            patternLocation++;
            bytesMatched++;
            continue;
        }
        else
        {
            // We don't match and so this pattern does not match.
            break;
        }
    }

    // If we are still matching good! Then this is a match!
    if ((inputLocation != nullptr && patternLocation != nullptr) &&
        (*inputLocation == *patternLocation) && requiredEntityPresent)
    {
        auto intentMatchResult = std::make_shared<CSpxIntentMatchResult>();

        if (PAL::stricmp(m_orthography->Name.c_str(), "fr") == 0)
        {
            for (auto& entityResult : entityResults)
            {
                entityResult.second.Value = Locales::Utils::AddLeadingPunctuationSpaceFR(entityResult.second.Value);
            }
        }

        intentMatchResult->InitIntentMatchResult(intentId, intentPattern.Phrase.c_str(), entityResults, intentPriority, bytesMatched);
        return Maybe<std::shared_ptr<CSpxIntentMatchResult>>(intentMatchResult);
    }
    else
    {
        return Maybe<std::shared_ptr<CSpxIntentMatchResult>>();
    }
}

void CSpxPatternMatchingModel::StoreEntityResult(const std::string& entityName, std::string& entityValue, std::map<std::string, EntityResult>& entityResults, bool& requiredEntityPresent)
{
    // Find the entity in the entity map if it exists.
    auto entityClassName = entityName.substr(0, entityName.find_first_of(":"));
    auto entityMapEntry = m_entityMap.find(entityClassName);
    if (entityMapEntry != m_entityMap.end())
    {
        // Emplace the entities found inside the matchResults map. Use the entity Id from the intent trigger.
        auto entity = entityMapEntry->second->Parse(entityValue);
        if (entity)
        {
            entityResults[entityName] = { entity.Get(), entityMapEntry->second->GetType() };
        }
        // If the entity is required and not present, then this intent is not a match.
        else if (entityMapEntry->second->IsRequired())
        {
            // Set our bool and break from the loop. This pattern doesn't match.
            requiredEntityPresent = false;
            return;
        }
    }
    else
    {
        // Treat this as an pattern.Any type if not specified.
        // Emplace the entities found inside the matchResults map.
        entityResults[entityName] = { entityValue , Intent::EntityType::Any };
    }
}

std::vector<std::string> CSpxPatternMatchingModel::ParseGroupedPhrases(const char** input)
{
    std::vector<std::string> phrases;
    std::string phrase = "";

    // Make sure we are at the start of an possiblePhrase.
    if (**input == '[' || **input == '(')
    {
        // Skip the bracket.
        (*input)++;
    }
    else
    {
        return phrases;
    }

    Utils::SkipPatternPunctuationAndWhitespace(*input, *m_orthography);

    while (**input != '\0' && **input != ']' && **input != ')')
    {
        // If we reach an or, store the phrase and reset.
        if (**input == '|')
        {
            Utils::TrimUTF8Whitespace(phrase, *m_orthography);
            // Save the phrase to our list.
            phrases.push_back(phrase);
            phrase = "";
            (*input)++;

            // Skip any whitespace before a word if necessary.
            Utils::SkipPatternPunctuationAndWhitespace(*input, *m_orthography);
            continue;
        }
        else
        {
            phrase += **input;
            (*input)++;
        }
    }
    if (**input == ']' || **input == ')')
    {
        Utils::TrimUTF8Whitespace(phrase, *m_orthography);
        // Save the phrase to our list.
        phrases.push_back(phrase);

        // Move past the bracket.
        (*input)++;
    }
    return phrases;
}

void CSpxPatternMatchingModel::AddIntent(std::shared_ptr<CSpxPatternMatchingIntent> intent, const std::string& intentId)
{
    if (!intent)
    {
        SPX_TRACE_ERROR("AddIntent called with invalid Intent", SPXERR_INVALID_ARG);
        return;
    }
    if (!intentId.empty())
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_intentMap.find(intentId) != m_intentMap.end())
        {
            // We need to append the patterns
            m_intentMap[intentId]->AddPatterns(intent->GetPatterns());
        }
        else
        {
            m_intentMap[intentId] = intent;
        }
    }
    else
    {
        SPX_TRACE_ERROR("AddIntent called with empty Intent ID", SPXERR_INVALID_ARG);
        return;
    }
}

void CSpxPatternMatchingModel::AddEntity(std::shared_ptr<ISpxEntity> entity)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    // Note if an entity with the same name is added, it will be overridden.
    m_entityMap[entity->GetName()] = entity;
}

const OrthographyInformation& CSpxPatternMatchingModel::GetOrthographyInfo() const
{
    return *m_orthography;
}

}}}}}
