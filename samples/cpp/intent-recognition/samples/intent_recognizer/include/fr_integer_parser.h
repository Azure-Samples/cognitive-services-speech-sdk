//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once

#include <string>

#include "intent_interfaces.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

class CSpxFRIntegerParser : public ISpxIntegerParser
{
public:

    // --- ISpxIntegerParser ---
    Maybe<std::string> Parse(const std::string& input) const override;
};

}}}}}
