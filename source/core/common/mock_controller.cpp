//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_controller.cpp: Implementation definitions for global Mock parameters
//

#include "stdafx.h"
#include "mock_controller.h"
#include "speechapi_c_factory.h"
#include "speechapi_cxx_factory_parameter.h"
#include "string_utils.h"


using namespace Microsoft::CognitiveServices::Speech;


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class MockParameterValue : public FactoryParameterValue
{
public:

    MockParameterValue(const char* psz) : FactoryParameterValue(SPXFACTORYHANDLE_ROOTSITEPARAMETERS_HACK, psz) { };

private:

    DISABLE_COPY_AND_MOVE(MockParameterValue);
};


void SpxSetMockParameterString(const char* name, const char* value)
{
    MockParameterValue(name).SetString(value);
}

void SpxSetMockParameterNumber(const char* name, int32_t value)
{
    MockParameterValue(name).SetNumber(value);
}

void SpxSetMockParameterBool(const char* name, bool value)
{
    MockParameterValue(name).SetBool(value);
}

std::string SpxGetMockParameterString(const char* name, const char* defaultValue)
{
    return MockParameterValue(name).GetString(defaultValue);
}

int32_t SpxGetMockParameterNumber(const char* name, int32_t defaultValue)
{
    return MockParameterValue(name).GetNumber(defaultValue);
}

bool SpxGetMockParameterBool(const char* name, bool defaultValue)
{
    return MockParameterValue(name).GetBool(defaultValue);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
