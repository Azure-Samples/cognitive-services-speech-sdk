//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_controller.h: Implementation declarations for global Mock parameters
//

#pragma once
#include <stdint.h>
#include <string>
#include <speechapi_c_common.h>


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


#define SPXFACTORYHANDLE_ROOTSITEPARAMETERS_HACK ((SPXFACTORYHANDLE)+1)


void SpxSetMockParameterString(const char* name, const char* value);
void SpxSetMockParameterNumber(const char* name, int32_t value);
void SpxSetMockParameterBool(const char* name, bool value);

std::string SpxGetMockParameterString(const char* name, const char* defaultValue = "");
int32_t SpxGetMockParameterNumber(const char* name, int32_t defaultValue = 0);
bool SpxGetMockParameterBool(const char* name, bool defaultValue = false);


} } } } // Microsoft::CognitiveServices::Speech::Impl
