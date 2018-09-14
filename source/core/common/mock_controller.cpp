//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_controller.cpp: Implementation definitions for global Mock parameters
//

#include "stdafx.h"
#include "mock_controller.h"
#include "speechapi_c_factory.h"
#include "speechapi_cxx_properties.h"
#include "string_utils.h"

using namespace Microsoft::CognitiveServices::Speech;


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class MockParameterValue : public PropertyCollection
{
public:

    MockParameterValue() : PropertyCollection(SPXFACTORYHANDLE_ROOTSITEPARAMETERS_MOCK) { }

private:

    DISABLE_COPY_AND_MOVE(MockParameterValue);
};

void SpxSetMockParameterString(const char* name, const char * value)
{
    MockParameterValue().SetProperty(name, value);
}

std::string SpxGetMockParameterString(const char* name, const char* defaultValue)
{
    return MockParameterValue().GetProperty(name, defaultValue);
}

void SpxSetMockParameterBool(const char* name, bool value)
{
    MockParameterValue().SetProperty(name, PAL::BoolToString(value));
}

bool SpxGetMockParameterBool(const char* name, bool defaultValue)
{
    return PAL::ToBool(MockParameterValue().GetProperty(name, PAL::BoolToString(defaultValue)).c_str());
}

void SpxSetMockParameterNumber(const char* name, int32_t value)
{
    MockParameterValue().SetProperty(name, std::to_string(value).c_str());
}

int32_t SpxGetMockParameterNumber(const char* name, int32_t defaultValue)
{
    return std::stoi(MockParameterValue().GetProperty(name, std::to_string(defaultValue)));
}
} } } } // Microsoft::CognitiveServices::Speech::Impl
