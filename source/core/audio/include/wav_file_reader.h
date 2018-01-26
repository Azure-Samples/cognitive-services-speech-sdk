//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// wav_file_reader.h: Implementation declarations for CSpxWavFileReader C++ class
//

#pragma once
#include <spxcore_common.h>
#include "ispxinterfaces.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxWavFileReader : public ISpxAudioFile, public ISpxAudioReader
{
public:

    // --- ctor/dtor

    CSpxWavFileReader();
    ~CSpxWavFileReader();

    // --- ISpxAudioFile

    void Open(const wchar_t* fileName);

    void Close();

    bool IsOpen() const;

    // --- ISpxAudioReader

    uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat);
    
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer);


private:

    void EnsureGetFormat();
    void FindFormatAndDataChunks();

    bool ReadChunkTypeAndSize(uint8_t* pchunkType, uint32_t* pchunkSize);

    void ReadFormatChunk(uint32_t chunkSize);

    void EnsureDataChunk();
    uint32_t ReadFromDataChunk(uint8_t** ppbuffer, uint32_t* pcbBuffer);

    using WavFile_Type = std::fstream;

    static const uint16_t cbTag = 4;
    static const uint16_t cbChunkType = 4;
    static const uint16_t cbChunkSize = 4;
    
    std::wstring m_fileName;
    std::unique_ptr<WavFile_Type> m_file;
    std::unique_ptr<WAVEFORMATEX> m_waveformat;

    uint32_t m_dataChunkBytesLeft;
};


} // CARBON_IMPL_NAMESPACE()
