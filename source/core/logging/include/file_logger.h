//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// file_logger.h: FileLogger() implementation declaration
//

#pragma once

#include <string>
#include <iostream>
#include "log_utils.h"

class FileLogger
{
public:
    static FileLogger& Instance();

    FileLogger(FileLogger const&) = delete;
    void operator=(FileLogger const&) = delete;

    void SetFileOptions(std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxNamedProperties> properties);
    std::string GetFilename();
    bool IsFileLoggingEnabled();
    void CloseFile();

    void LogToFile(const char *format);
private:
    FileLogger()
    {
        //filter = std::make_shared<LogFilter>();
    };

    std::string filename;
    std::string baseFilename;
    uint32_t fileDurationMB;
    uint32_t fileDurationSeconds;
    bool append = false;

    ReaderWriterLock fileNameLock;

    uint32_t currentFileAppendix;
    std::chrono::steady_clock::time_point lastFileStartTime = std::chrono::steady_clock::time_point::min();
    std::atomic_size_t fileDataWritten;

    void AssignFile();
    std::string BuildFileName(std::string currentName);

    volatile FILE* file = nullptr;
    std::mutex mtx;

    LogFilter filter;
};
