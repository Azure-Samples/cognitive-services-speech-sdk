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
#include <intentapi_cxx_exports.h>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// Represents the result of an intent recognition.
/// </summary>
class INTENT_API IntentRecognitionResult final
{
public:

    /// <summary>
    /// Constructor. Creates a new instance using the provided parameters.
    /// </summary>
    /// <param name="intentId">Result intent id.</param>
    /// <param name="jsonResult">Simple result JSON string.</param>
    /// <param name="detailedJsonResult">Detailed result JSON string.</param>
    IntentRecognitionResult(std::string& intentId, std::string& jsonResult, std::string& detailedJsonResult);
    IntentRecognitionResult(const IntentRecognitionResult& other);
    IntentRecognitionResult& operator=(const IntentRecognitionResult& other);
    IntentRecognitionResult(IntentRecognitionResult&&) = delete;
    IntentRecognitionResult& operator=(IntentRecognitionResult&&) = delete;

    /// <summary>
    /// A call to return a map of the entities found in the utterance.
    /// </summary>
    /// <returns>
    /// A map with the entity name as a key and containing the value of the entity found in the utterance.
    /// </returns>
    const std::map<std::string, std::string>& GetEntities() const;

    /// <summary>
    /// A call to return the detailed results.
    /// </summary>
    /// <returns>
    /// The detailed results as a JSON string.
    /// </returns>
    const std::string& GetDetailedResult() const;

    /// <summary>
    /// Destructor.
    /// </summary>
    ~IntentRecognitionResult();

private:

    void PopulateIntentFields(std::string& jsonResult);

    struct Storage
    {
        std::string intentId;
        std::map<std::string, std::string> entities;
        std::string detailedJsonResult;
    };

    Storage* m_storage;

public:
    const std::string& IntentId;
};

}}}}
