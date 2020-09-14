//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <memory>
#include <string>

#include "interfaces/base.h"
#include "interfaces/types.h"
#include "asyncop.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class ISpxNamedProperties;
class ISpxRecognitionResult2;
class ISpxRecognizer2;

class ISpxSession2 : public ISpxInterfaceBaseFor<ISpxSession2>
{
public:

    virtual const char* GetSessionId() const = 0;

    // virtual void AddRecognizer(std::shared_ptr<ISpxRecognizer2> recognizer) = 0;
    // virtual void RemoveRecognizer(ISpxRecognizer2* recognizer) = 0;

    // virtual CSpxAsyncOp<std::shared_ptr<ISpxRecognitionResult2>> RecognizeOnceAsync(const char* kind = nullptr, const std::shared_ptr<ISpxNamedProperties>& properties = nullptr) = 0;
    // virtual CSpxAsyncOp<void> StartContinuousAsync(const char* kind = nullptr, const std::shared_ptr<ISpxNamedProperties>& properties = nullptr) = 0;
    // virtual CSpxAsyncOp<void> StopAsync(const char* stopModeOrKind = nullptr) = 0; // "once", "continuous", "{kind}", nullptr == "all"

    // virtual CSpxAsyncOp<void> SendTechMessage(const char* tech, const char* message, const char* payload, const std::shared_ptr<ISpxNamedProperties>& properties) = 0;
};

// class ISpxRecognizer2 : public ISpxInterfaceBaseFor<ISpxRecognizer2>
// {
// };

// class ISpxRecognitionResult2 : public ISpxInterfaceBaseFor<ISpxRecognitionResult2>
// {
// };

} } } }
