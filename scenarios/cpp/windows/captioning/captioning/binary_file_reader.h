//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <fstream>
#include <speechapi_cxx.h>

using namespace Microsoft::CognitiveServices::Speech::Audio;

// Adapted from code in:
// https://github.com/Azure-Samples/cognitive-services-speech-sdk/blob/master/samples/cpp/windows/console/samples/speech_recognition_samples.cpp
class BinaryFileReader final : public PullAudioInputStreamCallback
{
private:

    std::fstream m_fs;

public:

    // Constructor that creates an input stream from a file.
    BinaryFileReader(const std::string& audioFileName)
    {
        if (audioFileName.empty())
        {
            throw std::invalid_argument("Audio filename is empty");
        }

        std::ios_base::openmode mode = std::ios_base::binary | std::ios_base::in;
        m_fs.open(audioFileName, mode);
        if (!m_fs.good())
        {
            throw std::invalid_argument("Failed to open the specified audio file.");
        }
    }

    // Implements AudioInputStream::Read() which is called to get data from the audio stream.
    // It copies data available in the stream to 'dataBuffer', but no more than 'size' bytes.
    // If the data available is less than 'size' bytes, it is allowed to just return the amount of data that is currently available.
    // If there is no data, this function must wait until data is available.
    // It returns the number of bytes that have been copied in 'dataBuffer'.
    // It returns 0 to indicate that the stream reaches end or is closed.
    int Read(uint8_t* dataBuffer, uint32_t size)
    {
        if (m_fs.eof())
            // returns 0 to indicate that the stream reaches end.
            return 0;
        m_fs.read((char*)dataBuffer, size);
        if (!m_fs.eof() && !m_fs.good())
            // returns 0 to close the stream on read error.
            return 0;
        else
            // returns the number of bytes that have been read.
            return (int)m_fs.gcount();
    }

    // Implements AudioInputStream::Close() which is called when the stream needs to be closed.
    void Close()
    {
        m_fs.close();
    }
};
