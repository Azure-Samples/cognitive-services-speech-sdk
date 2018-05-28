//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define CATCH_CONFIG_RUNNER
#include "test_utils.h"

int main(int argc, char* argv[])
{
    Catch::Session session; // There must be exactly one instance

    // Let Catch (using Clara) parse the command line
    int returnCode = parse_cli_args(session, argc, argv);
    if (returnCode != 0) // Indicates a command line error
        return returnCode;

    add_signal_handlers();

    return session.run();
}
