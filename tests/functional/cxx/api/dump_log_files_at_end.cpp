//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "dump_log_files_at_end.h"
#include <iostream>
#include <fstream>
#include <speechapi_cxx_enums.h>
#include <exception.h>
#include <speechapi_c_diagnostics.h>
#include <speechapi_c_property_bag.h>
#include <spxdebug.h>
#include <try_catch_helpers.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace IntegrationTests {

    using namespace std;
    using Microsoft::CognitiveServices::Speech::Impl::ExceptionWithCallStack;

    const SPXPROPERTYBAGHANDLE ROOT_SITE_PROPERTIES = reinterpret_cast<SPXPROPERTYBAGHANDLE>(1);

    static void HandleFailure(SPXHR hr, const string& description, const string& logFile)
    {
        SPXERRORHANDLE handle = SPXHANDLE_INVALID;

        try
        {
            if (SPX_SUCCEEDED(hr))
            {
                return;
            }

            cerr << "ERROR: Failed to " << description << " for '" << logFile << "'. ";

            handle = reinterpret_cast<SPXERRORHANDLE>(hr);
            if (error_get_error_code(handle) != SPX_NOERROR)
            {
                const char * what = error_get_message(handle);
                const char * callstack = error_get_call_stack(handle);

                cerr << (what ? what : "") << ". Call stack: " << (callstack ? callstack : "");
            }
            else
            {
                cerr << " HR " << Impl::stringify(hr) << " (0x" << hex << hr << ")";
            }

            cerr << endl;
        }
        catch (...) { }

        if (handle != SPXHANDLE_INVALID) error_release(handle);
    }

    static SPXHR StartLogging(const string& logFile)
    {
        SPX_IFTRUE_RETURN_HR(logFile.empty(), SPXERR_INVALID_ARG);

        SPXAPI_INIT_HR_TRY(hr)
        {
            // set the log file on the root site so it applies everywhere
            SPX_THROW_ON_FAIL(property_bag_set_string(ROOT_SITE_PROPERTIES, static_cast<int>(PropertyId::Speech_LogFilename), nullptr, logFile.c_str()));

            // start logging
            SPX_THROW_ON_FAIL(diagnostics_log_start_logging(ROOT_SITE_PROPERTIES, nullptr));
        }
        SPXAPI_CATCH_AND_RETURN_HR(hr);
    }

    static void DumpLogFile(const string& logFile)
    {
        try
        {
            string line;
            ifstream fileStream(logFile);
            if (fileStream.is_open())
            {
                while (getline(fileStream, line))
                {
                    cerr << line << endl;
                }

                fileStream.close();
            }
            else
            {
                cerr << "ERROR: Failed to open '" << logFile << "' for reading" << endl;
            }
        }
        catch (const exception& ex)
        {
            cerr << "ERROR: Failed to open '" << logFile << "' for reading. Cause: " << ex.what() << endl;
        }
        catch (...)
        {
            cerr << "ERROR: Failed to open '" << logFile << "' for reading with unknown throwable" << endl;
        }

        cerr << "************************************************************************************"
             << endl << endl;
    }

    LogToFileAndDumpAtEnd__::LogToFileAndDumpAtEnd__(const string& logFile)
        : m_logFile(logFile)
    {
        HandleFailure(StartLogging(logFile), "start logging", logFile);
    }

    LogToFileAndDumpAtEnd__::~LogToFileAndDumpAtEnd__()
    {
        try
        {
            // stop logging
            HandleFailure(diagnostics_log_stop_logging(), "stop logging", m_logFile);

            DumpLogFile(m_logFile);
            remove(m_logFile.c_str());
        }
        catch (...) {}
    }

}}}}
