//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#pragma once

#include <fstream>

using namespace Microsoft::SpeechSDK::Standalone::Intent;

void RequireIntentId(std::shared_ptr<IntentRecognitionResult> result, std::string expectedIntentId);
void RequireEntity(std::shared_ptr<IntentRecognitionResult> result, std::string expectedEntityId, std::string expectedEntityValue);
void RequireNoEntity(std::shared_ptr<IntentRecognitionResult> result, std::string expectedEntityId);
void RequireAlternateIntentId(std::shared_ptr<IntentRecognitionResult> result, std::string expectedIntentId);
void RequireAlternateCount(std::shared_ptr<IntentRecognitionResult> result, int expectedCount);

inline bool exists(const std::string& name) {
    return std::ifstream(name.c_str()).good();
}
