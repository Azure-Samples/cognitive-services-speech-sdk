//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// save_to_wav.h: Implementation declarations for CSpxSaveToWavFile helper class
//

#pragma once
#include <chrono>
#include <string>
#include "spxcore_common.h"
#include "file_utils.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {


class CSpxSaveToWavFile
{
public:

    static void SaveToWav(std::string filename, WAVEFORMATEX* format, const uint8_t* buffer, uint32_t size)
    {
        CSpxSaveToWavFile saveToWav;
        saveToWav.OpenWav(filename, format);
        saveToWav.SaveToWav(buffer, size);
    }

    CSpxSaveToWavFile() { }
    ~CSpxSaveToWavFile() { CloseWav(); }

    inline void OpenWav(std::string baseFileName, WAVEFORMATEX* format) { if (IsEnabled()) { OpenWavInternal(baseFileName, format); } }
    inline void SaveToWav(const uint8_t* buffer, uint32_t size) { if (m_file != nullptr) { SaveToWavInternal(buffer, size); } }
    inline void CloseWav() { if (m_file != nullptr) { CloseWavInternal(); } }


private:

    // trick to make header only class static value
    static bool IsEnabled() { return Enable(-1); }
    static bool Enable(int value)
    {
        static bool enabled = false;
        if (value >= 0) enabled = value;
        return enabled;
    }

    void OpenWavInternal(std::string baseFileName, WAVEFORMATEX* format)
    {
        CloseWav();
        m_totalSize = 0;

        std::string filename = GetFileName(baseFileName);

        SPX_DBG_TRACE_VERBOSE("Saving to wav: %s", filename.c_str());
        PAL::fopen_s(&m_file, filename.c_str(), "wb");

        SaveToWavInternal("RIFF____WAVEfmt ", 16);

        uint32_t sizeFormat = sizeof(WAVEFORMAT) + format->cbSize;
        SaveToWavInternal(&sizeFormat, 4);
        SaveToWavInternal(format, sizeFormat);

        SaveToWavInternal("data____", 8);
    }

    std::string GetFileName(std::string baseFileName)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto dur = now.time_since_epoch();
        auto hour = std::chrono::duration_cast<std::chrono::hours>(dur);
        auto min = std::chrono::duration_cast<std::chrono::minutes>(dur);
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(dur);

        static auto counter = 0;
        return baseFileName +
            std::to_string(100 + (hour.count() % 100)).substr(1) +
            std::to_string(100 + (min.count() % 100)).substr(1) +
            std::to_string(100 + (sec.count() % 100)).substr(1) +
            std::to_string(1000 + (counter++ % 1000)).substr(1) +
            ".wav";
    }

    void SaveToWavInternal(uint32_t number)
    {
        SaveToWavInternal(&number, sizeof(number));
    }

    template<class T>
    void SaveToWavInternal(const T* ptr, uint32_t size)
    {
        fwrite(ptr, 1, size, m_file);
        m_totalSize += size;
    }

    void CloseWavInternal()
    {
        // update the header RIFF header size
        fseek(m_file, 4, SEEK_SET);
        SaveToWavInternal(m_totalSize - 8);

        // update the data chunk size
        fseek(m_file, 40, SEEK_SET);
        SaveToWavInternal(m_totalSize - 44);

        // close the file
        fclose(m_file);
        m_file = nullptr;
        m_totalSize = 0;
    }

    FILE* m_file = nullptr;
    uint32_t m_totalSize = 0;
};


} } } } // Microsoft::CognitiveServices::Speech::Impl
