//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// logging_helpers.h: Implementation declarations/definitions for shared pointer helper methods
//

#pragma once
#include "spxcore_common.h"
#include "speechapi_c_diagnostics.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

inline void SpxDiagLogSetProperties(const std::shared_ptr<ISpxNamedProperties>& namedProperties)
{
    auto ptr = namedProperties.get();
    SPX_THROW_ON_FAIL(diagnostics_log_apply_properties(SPXHANDLE_INVALID, ptr));
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
