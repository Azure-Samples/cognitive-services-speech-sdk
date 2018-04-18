//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include <iomanip>
#include <sstream>
#include "exception.h"
#include "debug_utils.h"
#include "handle_table.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    static std::string hexify(SPXHR hr)
    {
        std::stringstream sstream;
        sstream << "0x" << std::hex << hr;
        return sstream.str();
    }

    ExceptionWithCallStack::ExceptionWithCallStack(SPXHR error, size_t skipLevels/* = 0*/) :
        std::runtime_error("Exception with an error code: " + hexify(error)),
        m_callstack(Debug::GetCallStack(skipLevels + 1, /*makeFunctionNamesStandOut=*/true)),
        m_error(error)
    {
    }

    ExceptionWithCallStack::ExceptionWithCallStack(const std::string& message, SPXHR error/* = SPXERR_UNHANDLED_EXCEPTION*/, size_t skipLevels /*= 0*/) :
        std::runtime_error(message),
        m_callstack(Debug::GetCallStack(skipLevels + 1, /*makeFunctionNamesStandOut=*/true)),
        m_error(error)
    {
    }

    const char* ExceptionWithCallStack::GetCallStack() const
    {
        return m_callstack.c_str();
    }

    SPXHR ExceptionWithCallStack::GetErrorCode() const
    {
        return m_error;
    }

    void ThrowWithCallstack(SPXHR hr, size_t skipLevels/* = 0*/)
    {
        ExceptionWithCallStack ex(hr, skipLevels + 1);
        SPX_DBG_TRACE_VERBOSE("About to throw %s %s", ex.what(), ex.GetCallStack());
        throw ex;
    }

    void ThrowRuntimeError(const std::string& msg, size_t skipLevels/* = 0*/)
    {
        ExceptionWithCallStack ex("Runtime error: " + std::string(msg), SPXERR_INVALID_ARG, skipLevels + 1);
        SPX_DBG_TRACE_VERBOSE("About to throw %s %s", ex.what(), ex.GetCallStack());
        throw ex;
    }

    void ThrowInvalidArgumentException(const std::string& msg, size_t skipLevels/* = 0*/)
    {
        ExceptionWithCallStack ex("Invalid argument exception: " + std::string(msg), SPXERR_INVALID_ARG, skipLevels + 1);
        SPX_DBG_TRACE_VERBOSE("About to throw %s %s", ex.what(), ex.GetCallStack());
        throw ex;
    }

    void ThrowLogicError(const std::string& msg, size_t skipLevels/* = 0*/)
    {
        ExceptionWithCallStack ex("Logic error: " + std::string(msg), SPXERR_INVALID_ARG, skipLevels + 1);
        SPX_DBG_TRACE_VERBOSE("About to throw %s %s", ex.what(), ex.GetCallStack());
        throw ex;
    }

    SPXHR StoreException(ExceptionWithCallStack&& ex)
    {
        auto errorHandles = CSpxSharedPtrHandleTableManager::Get<ExceptionWithCallStack, SPXERRORHANDLE>();
        std::shared_ptr<ExceptionWithCallStack> handle(new ExceptionWithCallStack(std::move(ex)));
        return reinterpret_cast<SPXHR>(errorHandles->TrackHandle(handle));
    }

} } } } // Microsoft::CognitiveServices::Speech::Impl
