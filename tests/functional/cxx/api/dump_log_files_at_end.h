//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <vector>
#include <fstream>
#include <speechapi_c_speech_config.h>
#include <try_catch_helpers.h>


#ifdef INTEGRATION_TEST_LOG_AND_DUMP
#define LogToFileAndDumpAtEnd(...) Microsoft::CognitiveServices::Speech::IntegrationTests::LogToFileAndDumpAtEnd__ __dump_at_end___( __VA_ARGS__ )
#else
#define LogToFileAndDumpAtEnd(...) 
#endif

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace IntegrationTests {

    using namespace std;
    using namespace Microsoft::CognitiveServices::Speech::Impl;

    /// <summary>
    /// Helper class to enable logging to a file, and then dumping the file contents out at the end
    /// to the output stream. This is handy in release builds where there is no usable logging to
    /// help detect and fix issues
    /// </summary>
    class LogToFileAndDumpAtEnd__
    {
    public:
        LogToFileAndDumpAtEnd__(const std::string& logFile)
            : m_logFile(logFile)
        {
            // try to start logging. Since we are dynamically linking to the Microsoft.CognitiveServices.Speech.core
            // library here, we need to call the undocumented exported C functions to turn on logging
            if (!logFile.empty())
            {
                StartLogging(logFile);
            }
        }

        ~LogToFileAndDumpAtEnd__()
        {
            // first try and stop logging since on Windows log files are not opened with file share read permissions
            SPXHR hr = diagnostics_log_stop_logging();
            if (hr != SPX_NOERROR)
            {
                cerr << "ERROR: Failed to stop logging with HR 0x" << hex << hr << endl;
                return;
            }

            try
            {
                string line;

                ifstream logFile(m_logFile);
                if (logFile.is_open())
                {
                    while (getline(logFile, line))
                    {
                        cerr << line << endl;
                    }

                    logFile.close();
                }
                else
                {
                    cerr << "ERROR: Failed to open '" << m_logFile << "' for reading" << endl;
                }
            }
            catch (const exception& ex)
            {
                cerr << "ERROR: Failed to read from '" << m_logFile << "'. Cause: " << ex.what() << endl;
            }
            catch (...)
            {
                cerr << "ERROR: Failed to read from '" << m_logFile << "' due to an unknown throwable" << endl;
            }

            cerr << ">>**********************************************************************************"
                 << endl << endl;
        }

        private:
            SPXHR StartLogging(const std::string& logFile)
            {
                try
                {
                    // Set on the root site so everyone inherits the settings
                    SPX_RETURN_ON_FAIL(property_bag_set_string(SPXFACTORYHANDLE_ROOTSITEPARAMETERS_MOCK, (int)PropertyId::Speech_LogFilename, nullptr, logFile.c_str()));

                    SPX_RETURN_ON_FAIL(diagnostics_log_start_logging(SPXFACTORYHANDLE_ROOTSITEPARAMETERS_MOCK, nullptr));
                    return SPX_NOERROR;
                }
                catch (const std::exception& ex)
                {
                    SPX_TRACE_ERROR("Failed to start logging. Reason: %s", ex.what());
                }
                catch (...)
                {
                    SPX_TRACE_ERROR("Failed to start logging due to unknown throwable");
                }

                return SPXERR_UNHANDLED_EXCEPTION;
            }

        private:
            std::string m_logFile;
    };

}}}}

