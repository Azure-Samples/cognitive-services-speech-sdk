//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include "catch.hpp"
#include "test_utils.h"

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "speechapi_cxx.h"


using namespace CARBON_NAMESPACE_ROOT;
using namespace CARBON_NAMESPACE_ROOT::Recognition;
using namespace CARBON_NAMESPACE_ROOT::Recognition::Speech;
using namespace std;



TEST_CASE("Speech Recognizer is thread-safe.", "[api][cxx]")
{
    wstring input_file(L"whatstheweatherlike.wav");
    REQUIRE(exists(input_file));

    mutex mtx;
    condition_variable cv;

    SECTION("Check for race conditions in destructor.")
    {
        bool callback_invoked = false;
        auto recognizer = RecognizerFactory::CreateSpeechRecognizerWithFileInput(input_file);
        recognizer->FinalResult.Connect(
            [&](const SpeechRecognitionEventArgs& args) 
        {
                {
                    UNUSED(args);
                    unique_lock<mutex> lock(mtx);
                    callback_invoked = true;
                    cv.notify_one();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
        });

        auto result = recognizer->RecognizeAsync().get();

        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&callback_invoked] { return callback_invoked; });
        }
        recognizer.reset();
    }
}