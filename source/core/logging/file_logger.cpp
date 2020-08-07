//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "file_logger.h"
#include "file_utils.h"
#include "exception.h"
#include <stdio.h>
#include <string.h>

void FileLogger::SetFileOptions(std::shared_ptr<Microsoft::CognitiveServices::Speech::Impl::ISpxNamedProperties> properties)
{
    std::lock_guard<std::mutex> lock(mtx);

    bool nameSet = properties->HasStringValue("SPEECH-LogFilename");
    std::string name;
    if (nameSet)
    {
        name = properties->GetStringValue("SPEECH-LogFilename", "");
    }

    bool filterSet = properties->HasStringValue("SPEECH-LogFileFilters");
    std::string filterValue;
    if (filterSet)
    {
        filterValue = properties->GetStringValue("SPEECH-LogFileFilters", "");
    }

    bool fileDurationSet = properties->HasStringValue("SPEECH-FileLogDurationSeconds");
    uint32_t fileDuration = 0;
    if (fileDurationSet)
    {
        fileDuration = std::stoul(properties->GetStringValue("SPEECH-FileLogDurationSeconds", "0"));
    }

    bool fileDurationSizeSet = properties->HasStringValue("SPEECH-FileLogSizeMB");
    uint32_t fileDurationSize = 0;
    if (fileDurationSizeSet)
    {
        fileDurationSize = std::stoul(properties->GetStringValue("SPEECH-FileLogSizeMB", "0"));
    }

    bool appendToFileSet = properties->HasStringValue("SPEECH-AppendToLogFile");
    uint32_t appendToFile = 0;
    if (appendToFileSet)
    {
        appendToFile = std::stoul(properties->GetStringValue("SPEECH-AppendToLogFile", "0"));
        append = 0 != appendToFile;
    }

    if (filterSet)
    {
        filter.SetFilter(filterValue);
    }

    if (nameSet && name.compare(baseFilename))
    {
        currentFileAppendix = 0;
        baseFilename = name;
    }

    std::string currentFileName = baseFilename;
    bool counterIncreased = false;

    if (fileDurationSet)
    {
        fileDurationSeconds = fileDuration;
    }

    if (fileDurationSeconds > 0 && !currentFileName.empty())
    {
        auto nextTime = lastFileStartTime + std::chrono::seconds(fileDurationSeconds);
        if (nextTime <= std::chrono::steady_clock::now())
        {
            currentFileAppendix++;
            counterIncreased = true;
        }
        currentFileName = BuildFileName(currentFileName);
    }

    if (fileDurationSizeSet)
    {
        fileDurationMB = fileDurationSize;
    }

    if (fileDurationMB > 0 && !currentFileName.empty() && !counterIncreased)
    {
        if (fileDataWritten.load() > fileDurationMB * 1024 * 1024) // MB
        {
            currentFileAppendix++;
        }
        currentFileName = BuildFileName(currentFileName);
    }

    // if user tries to change a filename, let them.
    if (!currentFileName.compare(filename))
    {
        return;
    }

    filename = currentFileName;

    AssignFile();
}

std::string FileLogger::GetFilename()
{
    return filename;
}

bool FileLogger::IsFileLoggingEnabled()
{
    return file != nullptr;
}

void FileLogger::CloseFile()
{
    WriteLock lock(&fileNameLock);
    if (file != nullptr)
    {
        fclose((FILE *)file);
        file = nullptr;
    }

    // Someone wanting to turn logging on can set a file name.
    baseFilename.clear();

}

void FileLogger::LogToFile(const char *logLine)
{
    if (file != nullptr && filter.ShouldLog(logLine))
    {
        ReadLock lock(&fileNameLock);
        FILE *fileToUse = (FILE *)file;
        if (fileToUse != nullptr)
        {
            fprintf(fileToUse, "%s", logLine);
            fflush(fileToUse);
            fileDataWritten.fetch_add(strlen(logLine));
        }
    }
}

FileLogger& FileLogger::Instance()
{
    static FileLogger instance;
    return instance;
}

void FileLogger::AssignFile()
{
    WriteLock lock(&fileNameLock);
    if (nullptr != file)
    {
        fclose((FILE *)file);
        file = nullptr;
    }

    if (!filename.empty())
    {
        FILE *newFile = PAL::fsopen(filename.c_str(), append ? "a" : "w");
        SPX_IFFALSE_THROW_HR(newFile != NULL, SPXERR_FILE_OPEN_FAILED);
        file = (volatile FILE *)newFile;
        lastFileStartTime = std::chrono::steady_clock::now();
        fileDataWritten.store(0);
    }
}

std::string FileLogger::BuildFileName(std::string fileName)
{
    auto lastDot = fileName.find_last_of('.');
    if (std::string::npos == lastDot)
    {
        // Just append it.
        return fileName += "-" + std::to_string(currentFileAppendix);
    }
    else
    {
        return fileName.substr(0, lastDot) + "-" + std::to_string(currentFileAppendix) + fileName.substr(lastDot);
    }
}
