//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <ajv.h>
#include <intentapi_cxx.h>
#include <string>
#include <sstream>
#include <iomanip>

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


std::string stringToHex(const std::string& str) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (unsigned char c : str) {
        oss << std::setw(2) << static_cast<int>(c) << " ";
    }

    return oss.str();
}

std::string dumpStringToUTF8(const std::string& str, bool escapeAscii) {
    std::ostringstream oss;

    for (size_t i = 0; i < str.size(); ) {
        unsigned char c = str[i];
        uint32_t codepoint = 0;
        int numBytes = 0;

        // Determine number of bytes in this UTF-8 character
        if ((c & 0x80) == 0x00) {
            // 1-byte character (ASCII)
            codepoint = c;
            numBytes = 1;
        }
        else if ((c & 0xE0) == 0xC0) {
            // 2-byte character
            codepoint = c & 0x1F;
            numBytes = 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            // 3-byte character
            codepoint = c & 0x0F;
            numBytes = 3;
        }
        else if ((c & 0xF8) == 0xF0) {
            // 4-byte character
            codepoint = c & 0x07;
            numBytes = 4;
        }
        else {
            // Invalid UTF-8, skip this byte
            oss << "\\x" << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(c);
            i++;
            continue;
        }

        // Read continuation bytes
        for (int j = 1; j < numBytes && (i + j) < str.size(); j++) {
            unsigned char cont = str[i + j];
            if ((cont & 0xC0) != 0x80) {
                // Invalid continuation byte
                break;
            }
            codepoint = (codepoint << 6) | (cont & 0x3F);
        }

        // Output the character
        if (numBytes == 1 && !escapeAscii && std::isprint(c)) {
            // Keep printable ASCII as-is
            oss << static_cast<char>(c);
        }
        else if (codepoint <= 0xFFFF) {
            // Use \uHHHH for BMP characters
            oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                << codepoint;
        }
        else {
            // Use \UHHHHHHHH for characters outside BMP
            oss << "\\U" << std::hex << std::setw(8) << std::setfill('0')
                << codepoint;
        }

        i += numBytes;
    }

    return oss.str();
}