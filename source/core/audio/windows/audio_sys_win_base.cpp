//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// common file for winRT and win32 audio sys.

#include <windows\audio_sys_win_base.h>

HRESULT GetBufferAndCallBackClient(
    IAudioCaptureClient* pCaptureClient,
    AudioDataBuffer& audioBuff,
    AUDIOINPUT_WRITE audio_write_cb,
    void * user_write_ctx,
    int & audio_result)
{

    uint32_t space_remaining = 0;
    uint32_t adding = 0;
    HRESULT  hr = S_OK;
    BYTE *pData;
    DWORD flags = 0;
    UINT32 numFramesAvailable = 0;
   
    // Get the available data in the shared buffer.
    hr = pCaptureClient->GetBuffer(
        &pData,
        &numFramesAvailable,
        &flags, nullptr, nullptr);
    EXIT_ON_ERROR(hr);

    space_remaining = audioBuff.totalSize - audioBuff.currentSize;

    // How many bytes we're adding to our local audio buff
    adding = min(numFramesAvailable * 2, space_remaining);

    // Copy the audio data buffer into our local audio buff which we pass to our audio write callback.
    // keep filling the buffer up.
    memcpy_s(audioBuff.pAudioData + audioBuff.currentSize,
        space_remaining,
        pData,
        adding);

    audioBuff.currentSize += adding;

    // If there is data remaining in the audio buffer pData:
    // Flush the local audio buff and reset it. Then copy over the remaining data.
    if (adding < (numFramesAvailable * 2))
    {
        audio_result = audio_write_cb(user_write_ctx,
            audioBuff.pAudioData,
            audioBuff.currentSize);

        uint32_t data_remaining = (numFramesAvailable * 2) - adding;

        // Reset local audio buffer.
        audioBuff.currentSize = 0;
        if (data_remaining > 0)
        {
            uint32_t copySize = min(data_remaining, audioBuff.totalSize);
            memcpy_s(audioBuff.pAudioData,
                audioBuff.totalSize,
                pData + adding,
                copySize);
            audioBuff.currentSize += copySize;
        }
    }

    hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
    EXIT_ON_ERROR(hr);

Exit:
    return hr;
}

HRESULT SetAudioStreamCategory(IAudioClient* pAudioClient, AUDIO_STREAM_CATEGORY category)
{
    IAudioClient2 * pAudioClient2 = nullptr;
    HRESULT hr = S_OK;

    hr = pAudioClient->QueryInterface(&pAudioClient2);
    if (SUCCEEDED(hr))
    {
        AudioClientProperties props = { 0 };

        props.cbSize = sizeof(AudioClientProperties);
        props.eCategory = category;
        hr = pAudioClient2->SetClientProperties(&props);
    }

    SAFE_RELEASE(pAudioClient2);

    return hr;
}
