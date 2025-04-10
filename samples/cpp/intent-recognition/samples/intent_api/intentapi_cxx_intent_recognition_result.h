//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_intent_recognition_result.h: Intent recognition API declarations for IntentRecognitionResult C++ class
//
#pragma once

#include <string>
#include <map>

#include <ajv.h>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// Represents the result of an intent recognition.
/// </summary>
class IntentRecognitionResult final
{
public:

    /// <summary>
    /// Constructor. Creates a new instance using the provided parameters.
    /// </summary>
    /// <param name="intentId">Result intent id.</param>
    /// <param name="jsonResult">Simple result JSON string.</param>
    /// <param name="detailedJsonResult">Detailed result JSON string.</param>
    IntentRecognitionResult(std::string& intentId, std::string& jsonResult, std::string& detailedJsonResult) :
        m_intentId(intentId),
        IntentId(m_intentId),
        m_detailedJsonResult(detailedJsonResult)
    {
        PopulateIntentFields(jsonResult);
    }

    /// <summary>
    /// A call to return a map of the entities found in the utterance.
    /// </summary>
    /// <returns>
    /// A map with the entity name as a key and containing the value of the entity found in the utterance.
    /// </returns>
    const std::map<std::string, std::string>& GetEntities() const
    {
        return m_entities;
    }

    /// <summary>
    /// A call to return the detailed results.
    /// </summary>
    /// <returns>
    /// The detailed results as a JSON string.
    /// </returns>
    const std::string& GetDetailedResult() const
    {
        return m_detailedJsonResult;
    }

    /// <summary>
    /// Destructor.
    /// </summary>
    ~IntentRecognitionResult()
    {
    }

    /// <summary>
    /// Unique intent id.
    /// </summary>
    const std::string& IntentId;

private:

    void PopulateIntentFields(std::string& jsonResult)
    {
        auto parser = ajv::json::Parse(jsonResult);
        auto reader = parser.Reader();
        for (auto name = reader.FirstName(); name.IsOk(); name++)
        {
            auto key = name.AsString(false); // Do not convert Unicode escape sequences in the name,
            auto item = reader[key.c_str()]; // otherwise the corresponding value will not be found.
            auto value = item.AsString();
            if (!value.empty())
            {
                key = name.AsString();
                m_entities[key] = value;
            }
        }
    }

    std::string m_intentId;
    std::map<std::string, std::string> m_entities;
    std::string m_detailedJsonResult;
};

}}}}
