//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <iostream>
#include <random>
#include <signal.h>
#include <fstream>

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"
#define __SPX_DO_TRACE_IMPL SpxTraceMessage

#include "string_utils.h"
#include "debug_utils.h"

#ifdef _MSC_VER
#pragma warning( push )
// disable: (9754,94): error 6330:  : 'const char' passed as _Param_(1) when 'unsigned char' is required in call to 'isalnum'.
#pragma warning( disable : 6330 )
#include "catch.hpp"
#pragma warning( pop )
#else
#include "catch.hpp"
#endif

#if !defined(CATCH_CONFIG_RUNNER)
#define EXTERN extern
#else
#define EXTERN
#endif

namespace Keys
{
    EXTERN std::string Speech;
    EXTERN std::string CRIS;
    EXTERN std::string LUIS;
    EXTERN std::string Skyman;
}

namespace Config
{
    EXTERN std::string Endpoint;
    EXTERN std::string Region;
    EXTERN std::string LuisAppId;
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
        | Opt(Keys::LUIS, "LuisSubscriptionKey")
        ["--keyLUIS"]
    ("The subscription key for LanguageUnderstanding")
        | Opt(Keys::Skyman, "SkymanSubscriptionKey")
        ["--keySkyman"]
    ("The subscription key for skyman")
        | Opt(Config::Endpoint, "endpoint")
        ["--endpoint"]
    ("The endpoint url to test against.")
        | Opt(Config::Region, "RegionId")
        ["--regionId"]
    ("The region id to be used for subscription and authorization requests")
        | Opt(Config::LuisAppId, "LuisAppId")
        ["--luisAppId"]
    ("The language understanding app id to be used intent recognition tests")
    ;

    // Now pass the new composite back to Catch so it uses that
    session.cli(cli);

    // Let Catch (using Clara) parse the command line
    return session.applyCommandLine(argc, argv);
}
#endif

#define SPXTEST_SECTION(msg) SECTION(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_SECTION('%s') %s(%d)", msg, __FILE__, __LINE__);  \
    return 1; }())

#define SPXTEST_GIVEN(msg) GIVEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_GIVEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_WHEN(msg) WHEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_WHEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_REQUIRE(expr) \
    SPX_TRACE_INFO("SPXTEST_REQUIRE('%s'): %s(%d):", __SPX_EXPR_AS_STRING(expr), __FILE__, __LINE__); \
    SPX_TRACE_ERROR_IF(!(expr), "SPXTEST_REQUIRE('%s') FAILED: %s(%d):", __SPX_EXPR_AS_STRING(expr), __FILE__, __LINE__); \
    REQUIRE(expr)

#define SPXTEST_REQUIRE_RESULT_RECOGNIZED_SPEECH(result) do {           \
    SPXTEST_REQUIRE(result != nullptr);                                 \
    SPXTEST_REQUIRE(result->Reason == ResultReason::RecognizedSpeech);  \
    SPXTEST_REQUIRE(!result->Text.empty()); } while (0)
