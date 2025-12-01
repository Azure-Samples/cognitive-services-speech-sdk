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
    IntentRecognitionResult(const IntentRecognitionResult& other);
    IntentRecognitionResult& operator=(const IntentRecognitionResult& other);
    IntentRecognitionResult(IntentRecognitionResult&&) = delete;
    IntentRecognitionResult& operator=(IntentRecognitionResult&&) = delete;

    const std::map<std::string, std::string>& GetEntities() const;

    const std::string& GetDetailedResult() const;

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
