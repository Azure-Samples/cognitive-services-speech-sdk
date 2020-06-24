//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>

#ifdef INTEGRATION_TEST_LOG_AND_DUMP
#define LogToFileAndDumpAtEnd(...) Microsoft::CognitiveServices::Speech::IntegrationTests::LogToFileAndDumpAtEnd__ __dump_at_end___( __VA_ARGS__ )
#else
#define LogToFileAndDumpAtEnd(...) 
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace IntegrationTests {

    /// Helper class to enable logging to a file, and then dumping the file contents out at the end
    /// to the output stream. This is handy in release builds where there is no usable logging to
    /// help detect and fix issues
    /// </summary>
    class LogToFileAndDumpAtEnd__
    {
    public:
        LogToFileAndDumpAtEnd__(const std::string& logFile);
        virtual ~LogToFileAndDumpAtEnd__();

        private:
            const std::string m_logFile;
    };

}}}}
