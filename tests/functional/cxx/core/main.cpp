//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#define CATCH_CONFIG_RUNNER
#include "test_utils.h"
#include "trace_message.h"
#include "azure_c_shared_utility/xlogging.h"

#if defined(_MSC_VER) && defined(_DEBUG)
// in case of asserts in debug mode, print the message into stderr and throw exception
int HandleDebugAssert(int,               // reportType  - ignoring reportType, printing message and aborting for all reportTypes
    char *message,     // message     - fully assembled debug user message
    int * returnValue) // returnValue - retVal value of zero continues execution
{
    fprintf(stderr, "C-Runtime: %s\n", message);

    if (returnValue) {
        *returnValue = 0;   // return value of 0 will continue operation and NOT start the debugger
    }

    return 1;            // make sure no message box is displayed
}
#endif

int main(int argc, char* argv[])
{
#if defined(_MSC_VER) && defined(_DEBUG)
    // in case of asserts in debug mode, print the message into stderr and throw exception
    if (_CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, HandleDebugAssert) == -1) {
        fprintf(stderr, "_CrtSetReportHook2 failed.\n");
        return -1;
    }
#endif

    xlogging_set_log_function(_xlogging_log_function_spx_trace_message_wrapper);

    auto logging = PAL::SpxGetEnv("SPEECHSDK_TEST_LOGGING", "memory");
    auto enableMemoryLogging = logging.find("memory") != logging.npos;
    if (enableMemoryLogging) diagnostics_log_memory_start_logging();

    Catch::Session session; // There must be exactly one instance

    // The catch2 test adapter runs a Discovery phase and we shouldn't attemp io during this phase
    if (!checkForDiscovery(argc, argv))
    {
        std::string rootPath(argv[0]);
        std::replace(rootPath.begin(), rootPath.end(), '\\', '/');
        std::string rootPathOnly = rootPath.substr(0, rootPath.find_last_of('/') + 1);

        ConfigSettings::LoadFromJsonFile(rootPathOnly);
    }

    // Let Catch (using Clara) parse the command line
    int returnCode = parse_cli_args(session, argc, argv);
    if (returnCode != 0) // Indicates a command line error
        return returnCode;

    add_signal_handlers();

    return session.run();
}
