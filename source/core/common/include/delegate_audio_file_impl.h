//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// delegate_audio_file_impl.h: Implementation declarations for ISpxDelegateAudioFileImpl C++ class
//

#pragma once
#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


class ISpxDelegateAudioFileImpl : public ISpxAudioFile
{
    // --- ISpxAudioFile

    void Open(const wchar_t* pszFileName) override { m_delegateToAudioFile->Open(pszFileName); }
    void Close() override { m_delegateToAudioFile->Close(); }

    bool IsOpen() const override { return m_delegateToAudioFile->IsOpen(); }

    void SetContinuousLoop(bool value) override { m_delegateToAudioFile->SetContinuousLoop(value); }
    void SetIterativeLoop(bool value) override { m_delegateToAudioFile->SetIterativeLoop(value); }
    void SetRealTimePercentage(uint8_t percentage) override { m_delegateToAudioFile->SetRealTimePercentage(percentage); }


protected:

    ISpxDelegateAudioFileImpl() { }
    ISpxDelegateAudioFileImpl(std::shared_ptr<ISpxAudioFile> delegateTo) : m_delegateToAudioFile(delegateTo) { }

    ISpxDelegateAudioFileImpl(ISpxDelegateAudioFileImpl&&) = delete;
    ISpxDelegateAudioFileImpl(const ISpxDelegateAudioFileImpl&) = delete;
    ISpxDelegateAudioFileImpl& operator=(ISpxDelegateAudioFileImpl&&) = delete;
    ISpxDelegateAudioFileImpl& operator=(const ISpxDelegateAudioFileImpl&) = delete;
    
    ~ISpxDelegateAudioFileImpl() { m_delegateToAudioFile = nullptr; }

    std::shared_ptr<ISpxAudioFile> m_delegateToAudioFile;
};


} // CARBON_IMPL_NAMESPACE
