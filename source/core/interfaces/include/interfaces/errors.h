//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <memory>
#include <string>

#include <speechapi_cxx_enums.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxErrorInformation : public ISpxInterfaceBaseFor<ISpxErrorInformation>
{
public:
    enum class RetryMode { Allowed, NotAllowed };
    virtual const std::string GetDetails() = 0;
    virtual CancellationReason GetCancellationReason() = 0;
    virtual CancellationErrorCode GetCancellationCode() = 0;
    virtual int GetCategoryCode() = 0;
    virtual int GetStatusCode() = 0;
    virtual RetryMode GetRetryMode() = 0;
};

} } } }
