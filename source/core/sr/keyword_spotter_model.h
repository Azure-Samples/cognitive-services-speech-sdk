//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once
#include <string>
#include "ispxinterfaces.h"
#include "interface_helpers.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxKwsModel : public ISpxKwsModel
{
public:

    CSpxKwsModel()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }

    ~CSpxKwsModel()
    {
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    }
    
    SPX_INTERFACE_MAP_BEGIN()
        SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
        SPX_INTERFACE_MAP_ENTRY(ISpxKwsModel)
    SPX_INTERFACE_MAP_END()

    // --- ISpxKwsModel ---
    void InitFromFile(const wchar_t* fileName) override;
    std::wstring GetFileName() const override { return m_fileName; }

private: 

    DISABLE_COPY_AND_MOVE(CSpxKwsModel);

    std::wstring m_fileName;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
