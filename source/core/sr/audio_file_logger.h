//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// audio_file_logger.h: Class to dump audio into a file
//
#pragma once
#include <memory>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxAudioFileLogger
{
private:
    struct NoPublic {};

public:
    static std::shared_ptr<CSpxAudioFileLogger> Setup(const std::string& folderName);
    CSpxAudioFileLogger(NoPublic, const std::string& folderName);
    ~CSpxAudioFileLogger();

    void Close();
    void SetDumpInstanceId(const std::string& id);
    size_t WriteAudio(const uint8_t* buffer, size_t bufferSize);

private:
    static std::atomic<unsigned int> s_nextAudioLogFileIndex;
    unsigned int m_audioLogFileIndex;
    std::string m_tempAudioLogFileName;

    FILE* m_audioDumpFile = nullptr;
    std::string m_audioDumpDir;
    std::string m_audioDumpInstTag;
    uint32_t m_audioDumpInstCount = 1;
    static constexpr auto s_tmpAudioDumpFileName = "tmpaudio.wav";
};

}}}}
