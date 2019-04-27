//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// user.cpp: Private implementation declarations for user
//
#include "stdafx.h"
#include "user.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxUser::CSpxUser()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

CSpxUser::~CSpxUser()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
}

void CSpxUser::InitFromUserId(const char* pszUserId)
{
    m_userId = pszUserId;
}

std::string CSpxUser::GetId() const
{
    return m_userId;
}

std::shared_ptr<ISpxNamedProperties> CSpxUser::GetParentProperties() const
{
    return SpxQueryService<ISpxNamedProperties>(GetSite());
}
}}}}
