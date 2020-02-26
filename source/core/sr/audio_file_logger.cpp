//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"
#include "audio_file_logger.h"
#include "file_utils.h"
#include "service_helpers.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

#ifdef _DEBUG
std::atomic<unsigned int> CSpxAudioFileLogger::s_nextAudioLogFileIndex;

/*static*/ std::shared_ptr<CSpxAudioFileLogger> CSpxAudioFileLogger::Setup(const std::string& folderName)
{
    if (folderName.empty())
    {
        return nullptr;
    }

    return std::make_shared<CSpxAudioFileLogger>(NoPublic(), folderName);
}

CSpxAudioFileLogger::CSpxAudioFileLogger(NoPublic, const std::string& folderName)
{
    m_audioLogFileIndex = s_nextAudioLogFileIndex++;

    // It will hold a prefix of 8 hex digits as "FFFFFFFF_";
    char buffLogIndex[10]; 

    snprintf(buffLogIndex, sizeof(buffLogIndex), "%08x_", m_audioLogFileIndex);

    m_audioDumpDir = folderName;
    m_tempAudioLogFileName = PAL::AppendPath(m_audioDumpDir, buffLogIndex);
    m_tempAudioLogFileName.append(s_tmpAudioDumpFileName);
    auto err = PAL::fopen_s(&m_audioDumpFile, m_tempAudioLogFileName.c_str(), "wb");

    if (err != 0)
    {
        SPX_TRACE_ERROR("%s: FAILED to open audio dump file %s for write (Error=%d)", __FUNCTION__, m_tempAudioLogFileName.c_str(), err);
    }
    else
    {
        SPX_DBG_TRACE_VERBOSE("%s: Writing audio dump to file %s", __FUNCTION__, m_tempAudioLogFileName.c_str());
    }
}

CSpxAudioFileLogger::~CSpxAudioFileLogger()
{
    Close();
}

void CSpxAudioFileLogger::Close()
{
    if (m_audioDumpFile)
    {
        fflush(m_audioDumpFile);
        fclose(m_audioDumpFile);
        auto newName = PAL::AppendPath(m_audioDumpDir, std::to_string(m_audioDumpInstCount) + "_" + m_audioDumpInstTag + ".wav");
        m_audioDumpInstCount++;
        m_audioDumpInstTag.clear();
        if (0 != std::rename(m_tempAudioLogFileName.c_str(), newName.c_str()))
        {
            SPX_TRACE_ERROR("%s: Failed to rename audio dump %s to %s", __FUNCTION__, m_tempAudioLogFileName.c_str(), newName.c_str());
        }
        else
        {
            SPX_DBG_TRACE_VERBOSE("%s: Closing audio dump file %s", __FUNCTION__, newName.c_str());
        }
        m_audioDumpFile = nullptr;
        m_tempAudioLogFileName.clear();
    }
}

void CSpxAudioFileLogger::SetDumpInstanceId(const std::string& id)
{
    m_audioDumpInstTag = id;
}

size_t CSpxAudioFileLogger::WriteAudio(const uint8_t* buffer, size_t bufferSize)
{
    return fwrite(buffer, 1, bufferSize, m_audioDumpFile);
}

#endif

}}}}
