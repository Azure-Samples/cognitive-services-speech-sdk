//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// dynamic_module.h: Implementation declarations for CSpxDynamicModule C++ class
//

#pragma once
#include "spxcore_common.h"
#include "interface_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxDynamicModule : public std::enable_shared_from_this<CSpxDynamicModule>
{
private:

    // Prevents the constructor being called outside this class. 
    class NoObject{};
public:
    typedef void* SPX_MODULE_FUNC;

    static std::shared_ptr<CSpxDynamicModule> Get(const std::string& filename);
    CSpxDynamicModule(const std::string& filename, NoObject);

    SPX_MODULE_FUNC GetModuleProcAddress(const std::string& procname);

private:

    CSpxDynamicModule() = delete;
    CSpxDynamicModule(const CSpxDynamicModule&) = delete;
    CSpxDynamicModule(const CSpxDynamicModule&&) = delete;

    CSpxDynamicModule& operator =(const CSpxDynamicModule&) = delete;
    CSpxDynamicModule&& operator =(const CSpxDynamicModule&&) = delete;

    SPX_MODULE_FUNC GetModuleFunctionPointer(const std::string& filename, const std::string& procname);

    std::string m_filename;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
