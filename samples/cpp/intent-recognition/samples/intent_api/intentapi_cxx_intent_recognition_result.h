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

    IntentRecognitionResult(std::string& intentId, std::string& jsonResult, std::string& detailedJsonResult);

    const std::map<std::string, std::string>& GetEntities() const;

    const std::string& GetDetailedResult() const;

    ~IntentRecognitionResult();

    const std::string& IntentId;

private:

    void PopulateIntentFields(std::string& jsonResult);

    std::string m_intentId;
    std::map<std::string, std::string> m_entities;
    std::string m_detailedJsonResult;
};

}}}}
