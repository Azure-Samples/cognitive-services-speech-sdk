//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_source_wrapper.cpp: Implementation definitions for CSpxAudioSourceWrapper
//

// ROBCH: Introduced in AUDIO.V3

#include "stdafx.h"
#include "audio_source_wrapper.h"
#include "create_object_helpers.h"
#include "property_id_2_name_map.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

CSpxAudioSourceWrapper::~CSpxAudioSourceWrapper()
{
    SPX_DBG_ASSERT(GetSite() == nullptr);
    SPX_DBG_ASSERT(ISpxAudioSourceInitDelegateImpl::IsClear());
    SPX_DBG_ASSERT(ISpxAudioSourceControlDelegateImpl::IsClear());
    SPX_DBG_ASSERT(ISpxAudioSourceBufferDataDelegateImpl::IsClear());
    SPX_DBG_ASSERT(ISpxAudioSourceBufferDataWriterDelegateImpl::IsClear());
}

void CSpxAudioSourceWrapper::Term()
{
    TermAudioSourceDelegate();
}

void CSpxAudioSourceWrapper::InitFromMicrophone()
{
    InitAudioSourceClassName("CSpxMicrophoneAudioSourceAdapter");
    ISpxAudioSourceInitDelegateImpl::InitFromMicrophone();
}

void CSpxAudioSourceWrapper::InitFromFile(const wchar_t* pszFileName)
{
    InitAudioSourceClassName("CSpxFileAudioSourceAdapter");
    ISpxAudioSourceInitDelegateImpl::InitFromFile(pszFileName);
}

void CSpxAudioSourceWrapper::InitFromStream(std::shared_ptr<ISpxAudioStream> stream)
{
    InitAudioSourceClassName("CSpxStreamAudioSourceAdapter");
    ISpxAudioSourceInitDelegateImpl::InitFromStream(stream);
}

std::shared_ptr<ISpxAudioSourceInit> CSpxAudioSourceWrapper::InitAudioSourceDelegate()
{
    return SpxCreateObjectWithSite<ISpxAudioSourceInit>(m_audioSourceClassName.c_str(), this);
}

void CSpxAudioSourceWrapper::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceInit>& ptr)
{
    SPX_DBG_ASSERT(!m_audioSourceClassName.empty());
    ptr = InitAudioSourceDelegate();
}

template<typename I>
std::shared_ptr<I> InitDelegatePtrHelper(ISpxAudioSourceInitDelegateImpl<>& helper)
{
    return SpxQueryInterfaceFromDelegate<I>(helper);
}

void CSpxAudioSourceWrapper::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceControl>& ptr)
{
    ptr = InitDelegatePtrHelper<ISpxAudioSourceControl>(*this);
}

void CSpxAudioSourceWrapper::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferData>& ptr)
{
    ptr = InitDelegatePtrHelper<ISpxAudioSourceBufferData>(*this);
}

void CSpxAudioSourceWrapper::InitDelegatePtr(std::shared_ptr<ISpxAudioSourceBufferDataWriter>& ptr)
{
    ptr = InitDelegatePtrHelper<ISpxAudioSourceBufferDataWriter>(*this);
}

void CSpxAudioSourceWrapper::TermAudioSourceDelegate()
{
    using SourceInitDelegate = ISpxAudioSourceInitDelegateImpl;
    SPX_IFTRUE_RETURN(SourceInitDelegate::IsClear());

    using ControlDelegate = ISpxAudioSourceControlDelegateImpl;
    ControlDelegate::Zombie(true);
    ControlDelegate::Clear();
    SPX_DBG_ASSERT(ControlDelegate::IsClear());

    using DataDelegate = ISpxAudioSourceBufferDataDelegateImpl;
    DataDelegate::Zombie(true);
    DataDelegate::Clear();
    SPX_DBG_ASSERT(DataDelegate::IsClear());

    using WriterDelegate = ISpxAudioSourceBufferDataWriterDelegateImpl;
    WriterDelegate::Zombie(true);
    WriterDelegate::Clear();
    SPX_DBG_ASSERT(WriterDelegate::IsClear());

    SpxTermAndClearDelegate(static_cast<SourceInitDelegate&>(*this));
    SPX_DBG_ASSERT(ISpxAudioSourceInitDelegateImpl::IsClear());
}

void CSpxAudioSourceWrapper::InitAudioSourceClassName(const char* className)
{
    SPX_DBG_ASSERT(m_audioSourceClassName.empty());
    m_audioSourceClassName = className;
}

ISpxAudioSource::State CSpxAudioSourceWrapper::GetState() const
{
    using SourceInitDelegate = ISpxAudioSourceInitDelegateImpl;
    auto ptr = SpxQueryInterfaceFromDelegate<ISpxAudioSource>(static_cast<const SourceInitDelegate&>(*this));
    if (ptr == nullptr)
    {
        return ISpxAudioSource::State::Idle;
    }
    return ptr->GetState();
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
