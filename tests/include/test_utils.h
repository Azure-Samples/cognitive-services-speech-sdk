//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <iostream>
#include <random>
#include <signal.h>
#include <fstream>
#include "string_utils.h"
#include "debug_utils.h"
#include "catch.hpp"

#if !defined(CATCH_CONFIG_RUNNER)
#define EXTERN extern
#else
#define EXTERN
#endif

namespace Keys 
{
    EXTERN std::string Speech;
    EXTERN std::string CRIS;
    EXTERN std::string LanguageUnderstanding;
    EXTERN std::string Skyman;
}

namespace Config
{
    EXTERN std::string Endpoint;
    EXTERN std::string Region;
}



inline bool exists(const std::wstring& name) {
    return std::ifstream(PAL::ToString(name).c_str()).good();
}

inline std::ifstream get_stream(const std::wstring& name) {
    return std::ifstream(PAL::ToString(name).c_str(), std::ifstream::binary);
}

typedef std::linear_congruential_engine<uint_fast32_t, 1664525, 1013904223, UINT_FAST32_MAX> random_engine;

inline void add_signal_handlers()
{
    Debug::HookSignalHandlers();
}

#if defined(CATCH_CONFIG_RUNNER)
inline int parse_cli_args(Catch::Session& session, int argc, char* argv[])
{
    // Build a new parser on top of Catch's
    using namespace Catch::clara;
    auto cli
        = session.cli() // Get Catch's composite command line parser
        | Opt(Keys::Speech, "SpeechSubscriptionKey") // bind variable to a new option, with a hint string
        ["--keySpeech"]    // the option names it will respond to
    ("The subscription key for speech")
        | Opt(Keys::CRIS, "CRISSubscriptionKey")
        ["--keyCRIS"]
    ("The subscription key for CRIS")
        | Opt(Keys::LanguageUnderstanding, "LanguageUnderstandingSubscriptionKey")
        ["--keyLU"]
    ("The subscription key for LanguageUnderstanding")
        | Opt(Keys::Skyman, "SkymanSubscriptionKey")
        ["--keySkyman"]
    ("The subscription key for skyman")
        | Opt(Config::Endpoint, "endpoint")
        ["--endpoint"]
    ("The endpoint url to test against.")
        | Opt(Config::Region, "RegionId")
        ["--regionId"]
    ("The region id to be used for subscription and authorization requests");

    // Now pass the new composite back to Catch so it uses that
    session.cli(cli);

    // Let Catch (using Clara) parse the command line
    return session.applyCommandLine(argc, argv);
}
#endif