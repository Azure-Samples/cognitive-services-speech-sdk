//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <stdexcept>
#include <string>
#include <spxerror.h>
#include <spx_namespace.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ExceptionWithCallStack : public std::runtime_error
{
public:
    ExceptionWithCallStack(SPXHR error, size_t skipLevels = 0);
    const char* GetCallStack() const;
    SPXHR GetErrorCode() const;

private:
    std::string m_callstack;
    SPXHR m_error;
};

void ThrowWithCallstack(SPXHR hr);

SPXHR StoreException(ExceptionWithCallStack&& ex);

} } } } // Microsoft::CognitiveServices::Speech::Impl
