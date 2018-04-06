//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "test_utils.h"

extern std::string g_keySpeech;
extern std::string g_keyCRIS;
extern std::string g_keyLUIS;
extern std::string g_keySkyman;

int main(int argc, char* argv[])
{
    Catch::Session session; // There must be exactly one instance

                            // Build a new parser on top of Catch's
    using namespace Catch::clara;
    auto cli
        = session.cli() // Get Catch's composite command line parser
        | Opt(g_keySpeech, "SpeechSubscriptionKey") // bind variable to a new option, with a hint string
             ["--keySpeech"]    // the option names it will respond to
             ("The subscription key for speech")
        | Opt(g_keyCRIS, "CRISSubscriptionKey") // bind variable to a new option, with a hint string
             ["--keyCRIS"]    // the option names it will respond to
             ("The subscription key for CRIS")
        | Opt(g_keyLUIS, "LUISSubscriptionKey") // bind variable to a new option, with a hint string
             ["--keyLUIS"]    // the option names it will respond to
             ("The subscription key for LUIS")        // description string for the help output
        | Opt(g_keySkyman, "SkymanSubscriptionKey") // bind variable to a new option, with a hint string
             ["--keySkyman"]    // the option names it will respond to
             ("The subscription key for skyman");

                                                    // Now pass the new composite back to Catch so it uses that
    session.cli(cli);

    // Let Catch (using Clara) parse the command line
    int returnCode = session.applyCommandLine(argc, argv);
    if (returnCode != 0) // Indicates a command line error
        return returnCode;

    add_signal_handlers();

    return session.run();
}
