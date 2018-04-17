//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// mock_lu_engine_adapter.cpp: Implementation definitions for CSpxMockLuEngineAdapter C++ class
//

#include "stdafx.h"
#include "mock_lu_engine_adapter.h"
#include "service_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


CSpxMockLuEngineAdapter::CSpxMockLuEngineAdapter()
{
    SPX_DBG_TRACE_FUNCTION();
}

CSpxMockLuEngineAdapter::~CSpxMockLuEngineAdapter()
{
    SPX_DBG_TRACE_FUNCTION();
}

void CSpxMockLuEngineAdapter::Init()
{
    SPX_DBG_TRACE_FUNCTION();
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);
}

void CSpxMockLuEngineAdapter::Term()
{
    SPX_DBG_TRACE_FUNCTION();
    m_triggerMap.clear();
}

void CSpxMockLuEngineAdapter::AddIntentTrigger(const wchar_t* id, std::shared_ptr<ISpxTrigger> trigger)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    m_triggerMap.emplace(id, trigger);
}

std::list<std::string> CSpxMockLuEngineAdapter::GetListenForList()
{
    // TODO: DGI
    return std::list<std::string>();
}

void CSpxMockLuEngineAdapter::GetIntentInfo(std::string& provider, std::string& id, std::string& key)
{
    // TODO: DGI
    UNUSED(provider);
    UNUSED(id);
    UNUSED(key);
}

void CSpxMockLuEngineAdapter::ProcessResult(std::shared_ptr<ISpxRecognitionResult> result)
{
    SPX_IFTRUE_THROW_HR(result == nullptr, SPXERR_INVALID_ARG);
    SPX_IFTRUE_THROW_HR(GetSite() == nullptr, SPXERR_UNINITIALIZED);

    // Get the named property service...
    auto properties = SpxQueryService<ISpxNamedProperties>(GetSite());
    SPX_IFTRUE_THROW_HR(properties == nullptr, SPXERR_NOT_FOUND);

    // Are we supposed to process this result?
    auto mockIntent = properties->HasStringValue(L"CARBON-INTERNAL-MOCK-IntentId");
    if (mockIntent)
    {
        // What's the intentId and json payload?
        auto intentId = properties->GetStringValue(L"CARBON-INTERNAL-MOCK-IntentId");
        auto json = properties->GetStringValue(L"CARBON-INTERNAL-MOCK-IntentJson");

        // OK... Now let's initialize the intent result
        auto initIntentResult = SpxQueryInterface<ISpxIntentRecognitionResultInit>(result);
        initIntentResult->InitIntentResult(intentId.c_str(), json.c_str());
    }
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
