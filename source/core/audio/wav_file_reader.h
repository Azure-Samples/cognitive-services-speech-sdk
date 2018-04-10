//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// wav_file_reader.h: Implementation declarations for CSpxWavFileReader C++ class
//

#pragma once
#include "spxcore_common.h"


namespace CARBON_IMPL_NAMESPACE() {


class CSpxWavFileReader : public ISpxAudioFile, public ISpxAudioReader, public ISpxAudioReaderRealTime
{
public:

    // --- ctor/dtor

    CSpxWavFileReader();
    ~CSpxWavFileReader();

    // --- ISpxAudioFile

    void Open(const wchar_t* fileName) override;
    void Close() override;

    bool IsOpen() const override;

    void SetContinuousLoop(bool value) override;
    void SetIterativeLoop(bool value) override;
    void SetRealTimePercentage(uint8_t percentage) override;

    // --- ISpxAudioReader

    uint16_t GetFormat(WAVEFORMATEX* pformat, uint16_t cbFormat) override;
    uint32_t Read(uint8_t* pbuffer, uint32_t cbBuffer) override;


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
    SpxWAVEFORMATEX_Type m_waveformat;

    bool m_continuousAudioLoop = false;           // Continuously loop thru the audio from the .WAV file; Essentially, .WAV is a ring buffer for infinite audio data source

    bool m_iterativeAudioLoop = false;            // Iteratively loop thru the audio data from the .WAV file; 3000 byte audio file calling Read, repeatedly, will return 
                                                  // 2000 bytes, then 1000 bytes, then 0 bytes, then 2000 bytes, then 1000 bytes, then 0 bytes ... over and over again

    uint8_t m_simulateRealtimePercentage = 0;     // 0 == as fast as possible; 100 == real time. E.g. If .WAV file is 12 seconds long, it will take 12 seconds to read all 
                                                  // the data when percentage==100; it'll take 1.2 seconds if set to 10; it'll go as fast as possible at 0; and it'll
                                                  // take 24 seconds if set to 200.

    std::streamoff m_firstSeekDataChunkPos;

    uint32_t m_dataChunkBytesLeft;
};


} // CARBON_IMPL_NAMESPACE
