//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <stdexcept>
#include <string>
#include <spxerror.h>
#include <spx_namespace.h>
#include <exception.h>

#ifndef _Analysis_noreturn_
#define _Analysis_noreturn_
#endif // ! _Analysis_noreturn_

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ExceptionWithCallStack : public std::runtime_error
{
public:
    ExceptionWithCallStack(SPXHR error, size_t skipLevels = 0);
    ExceptionWithCallStack(const std::string& message, SPXHR error = SPXERR_UNHANDLED_EXCEPTION, size_t skipLevels = 0);
    const char* GetCallStack() const;
    SPXHR GetErrorCode() const;

private:
    std::string m_callstack;
    SPXHR m_error;
};

_Analysis_noreturn_ void ThrowWithCallstack(SPXHR hr, size_t skipLevels = 0);
_Analysis_noreturn_ void ThrowRuntimeError(const std::string& msg, size_t skipLevels = 0);
_Analysis_noreturn_ void ThrowInvalidArgumentException(const std::string& msg, size_t skipLevels = 0);
_Analysis_noreturn_ void ThrowLogicError(const std::string& msg, size_t skipLevels = 0);

SPXHR StoreException(ExceptionWithCallStack&& ex);
SPXHR StoreException(const std::exception& ex);
std::string stringify(SPXHR hr);

} } } } // Microsoft::CognitiveServices::Speech::Impl
