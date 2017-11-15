//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognizer.h: Public API declarations for Recognizer C++ base class
//

#pragma once
#include <future>
#include <memory>
#include <speechapi_cxx_common.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


class Recognizer
{
public:

    virtual ~Recognizer() {};

    RecognizerParameters& Parameters;

    virtual bool IsEnabled() = 0;
    virtual void Enable() = 0;
    virtual void Disable() = 0;


protected:

    Recognizer(RecognizerParameters &parameters) : Parameters(parameters) {};


private:

    Recognizer(const Recognizer&) = delete;
    Recognizer(const Recognizer&&) = delete;

    Recognizer& operator=(const Recognizer&) = delete;
};


} }; // CARBON_NAMESPACE_ROOT :: Recognition
