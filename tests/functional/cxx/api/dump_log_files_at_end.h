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
        LogToFileAndDumpAtEnd__(const std::string& logFile) : LogToFileAndDumpAtEnd__(logFile, {})
        { }

        LogToFileAndDumpAtEnd__(const std::string& logFile, initializer_list<string> additionalFilesToDump)
            : m_logFiles()
        {
            if (!logFile.empty())
            {
                m_logFiles.push_back(logFile);
            }

            for (const auto& additionalFile : additionalFilesToDump)
            {
                m_logFiles.push_back(additionalFile);
            }

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
                cout << "ERROR: Failed to stop logging with HR 0x" << hex << hr << endl;
            }

            string line;
            for (const auto& filename : m_logFiles)
            {
                cout << ">>********   " << filename << "   *************************************************" << endl
                     << ">>**********************************************************************************" << endl;

                try
                {
                    ifstream logFile(filename);
                    if (logFile.is_open())
                    {
                        while (getline(logFile, line))
                        {
                            cout << line << endl;
                        }

                        logFile.close();
                    }
                    else
                    {
                        cout << "ERROR: Failed to open '" << filename << "' for reading" << endl;
                    }
                }
                catch (const exception& ex)
                {
                    cout << "ERROR: Failed to open '" << filename << "' for reading. Cause: " << ex.what() << endl;
                }
                catch (...)
                {
                    cout << "ERROR: Failed to open '" << filename << "' for reading with unknown throwable" << endl;
                }

                cout << ">>**********************************************************************************"
                     << endl << endl;
            }
        }

        private:
            static inline void ThrowOnFail(SPXHR hr)
            {
                if (SPX_FAILED(hr)) throw hr;
            }

            SPXHR StartLogging(const std::string& logFile)
            {
                SPXSPEECHCONFIGHANDLE speechConfig = SPXHANDLE_INVALID;
                SPXPROPERTYBAGHANDLE propertyBag = SPXHANDLE_INVALID;

                SPXHR hr = SPX_NOERROR;
                try
                {
                    // create a fake speech config instance to set the logging properties we need
                    ThrowOnFail(speech_config_from_subscription(&speechConfig, "not_real", "not_real"));
                    ThrowOnFail(speech_config_get_property_bag(speechConfig, &propertyBag));
                    ThrowOnFail(property_bag_set_string(propertyBag, (int)PropertyId::Speech_LogFilename, nullptr, logFile.c_str()));

                    // call the C API to enable file logging
                    ThrowOnFail(diagnostics_log_start_logging(speechConfig, nullptr));
                }
                catch (SPXHR error)
                {
                    hr = error;
                }
                catch (...)
                {
                    hr = SPXERR_UNHANDLED_EXCEPTION;
                }

                if (propertyBag != SPXHANDLE_INVALID) property_bag_release(propertyBag);
                if (speechConfig != SPXHANDLE_INVALID) speech_config_release(speechConfig);

                return hr;
            }

        private:
            vector<string> m_logFiles;
    };

}}}}

