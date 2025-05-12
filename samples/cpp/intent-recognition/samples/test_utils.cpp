//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <ajv.h>
#include <intentapi_cxx.h>

#include "catch2/catch_amalgamated.hpp"
#include "test_utils.h"

//#define INTENT_TEST_VERBOSE_RESULTS // Use this flag to make RequireXyz functions output the verified intent results to stdout

using namespace Microsoft::SpeechSDK::Standalone::Intent;


void RequireIntentId(std::shared_ptr<IntentRecognitionResult> result, std::string expectedIntentId)
{
    REQUIRE(result != nullptr);
    REQUIRE(result->IntentId == expectedIntentId);
#ifdef INTENT_TEST_VERBOSE_RESULTS
    std::cout << "*** Intent Id: \"" << expectedIntentId << "\"\n";
#endif
}

void RequireEntity(std::shared_ptr<IntentRecognitionResult> result, std::string expectedEntityId, std::string expectedEntityValue)
{
    REQUIRE(result != nullptr);
    auto entities = result->GetEntities();
    REQUIRE(entities[expectedEntityId] == expectedEntityValue);
#ifdef INTENT_TEST_VERBOSE_RESULTS
    INFO("*** Entity Id: \"" << expectedEntityId << "\" Value: \"" << entities[expectedEntityId] << "\"");
#endif
}

void RequireNoEntity(std::shared_ptr<IntentRecognitionResult> result, std::string expectedEntityId)
{
    REQUIRE(result != nullptr);
    auto entities = result->GetEntities();
    REQUIRE(entities.find(expectedEntityId) == entities.end());
#ifdef INTENT_TEST_VERBOSE_RESULTS
    std::cout << "*** No Entity\n";
#endif
}

void RequireAlternateIntentId(std::shared_ptr<IntentRecognitionResult> result, std::string expectedIntentId)
{
    REQUIRE(result != nullptr);

    auto detailedOutputJson = result->GetDetailedResult();
    auto detailedParser = ajv::JsonParser::Parse(detailedOutputJson);

    bool found = false;
    int index = 0;
    while (!detailedParser[index].IsEmpty())
    {
        auto intentJson = detailedParser[index];
        if (intentJson["intentId"].AsString() == expectedIntentId)
        {
            found = true;
            break;
        }
    }
    REQUIRE(found);
#ifdef INTENT_TEST_VERBOSE_RESULTS
    std::cout << "*** Alternate Intent Id: \"" << expectedIntentId << "\"\n";
#endif
}

void RequireAlternateCount(std::shared_ptr<IntentRecognitionResult> result, int expectedCount)
{
    REQUIRE(result != nullptr);

    auto detailedOutputJson = result->GetDetailedResult();
    auto detailedParser = ajv::JsonParser::Parse(detailedOutputJson);
    auto actualCount = detailedParser.ValueCount();
    REQUIRE(actualCount == expectedCount);
#ifdef INTENT_TEST_VERBOSE_RESULTS
    std::cout << "*** Intent Count: " << actualCount << "\n";
#endif
}
