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

    auto name = properties->GetStringValue("SPEECH-LogFilename", "");
    auto filter = properties->GetStringValue("SPEECH-FileLogFilters", "");
    auto fileDuration = std::stoul(properties->GetStringValue("SPEECH-FileLogDurationSeconds", "0"));
    auto fileDurationSize = std::stoul(properties->GetStringValue("SPEECH-FileLogSizeMB", "0"));
    auto appendToFile = std::stoul(properties->GetStringValue("SPEECH-AppendToLogFile", "0"));

    append = 0 != appendToFile;

    if (((!filtersRaw[0]) && filter.empty()) || // Current filter has something, but we're being asked to clear it.
        ((filtersRaw[0]) && !filter.empty()) || // Current is empty, and the new one is not.
        (filter.compare(filtersRaw))) // The filter being passed is different than the current one.
    {
        // We take a max size...
        if (!filter.empty() &&
            (filter.length() > sizeof(filtersRaw)))
        {
            Microsoft::CognitiveServices::Speech::Impl::ThrowRuntimeError("Length of filter criteria too large.", 0);
        }

        // Clear the existing filters
        memset(&filters, 0, sizeof(filters));
        memset(&filtersRaw, 0, sizeof(filtersRaw));
        memset(&filterPointers, 0, sizeof(filterPointers));

        if (!filter.empty())
        {
            strncpy(filtersRaw, filter.c_str(), sizeof(filtersRaw));
            strncpy(filters, filtersRaw, sizeof(filters));

            int currentFilterPointer = 0;
            char *token = strtok(filters, ";");
            while (token != NULL)
            {
                if (currentFilterPointer >= FILE_FILTER_POINTER_LENGTH)
                {
                    Microsoft::CognitiveServices::Speech::Impl::ThrowRuntimeError("Too many filters passed.", 0);
                }

                filterPointers[currentFilterPointer++] = token;

                token = strtok(NULL, ";");
            }
        }
    }

    if (name.compare(baseFilename))
    {
        currentFileAppendix = 0;
        baseFilename = name;
    }

    if (fileDuration > 0 && !name.empty())
    {
        auto nextTime = lastFileStartTime + std::chrono::seconds(fileDuration);
        if (nextTime <= std::chrono::steady_clock::now())
        {
            currentFileAppendix++;
        }
        name = BuildFileName(name);
    }

    if (fileDurationSize > 0 && !name.empty())
    {
        if (fileDataWritten.load() > fileDurationSize * 1024 * 1024) // MB
        {
            currentFileAppendix++;
        }
        name = BuildFileName(name);
    }

    // if user tries to change a filename, let them.
    if (!name.compare(filename))
    {
        return;
    }

    filename = name;

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
}

void FileLogger::LogToFile(std::string&& logLine)
{
    if (file != nullptr)
    {
        bool log = true;

        if (filterPointers[0])
        {
            log = false;

            for (int i = 0; i < FILE_FILTER_POINTER_LENGTH; i++)
            {
                char *currentFilter = filterPointers[i];
                if (NULL == currentFilter)
                {
                    break;
                }

                if (strstr(logLine.c_str(), currentFilter))
                {
                    log = true;
                    break;
                }
            }
        }

        if (log)
        {
            WriteLock lock(&fileNameLock);
            if (file == nullptr)
            {
                return;
            }

            FILE *fileToUse = (FILE *)file;
            fprintf(fileToUse, "%s", logLine.c_str());
            fflush(fileToUse);
            fileDataWritten.fetch_add(logLine.length());
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
        FILE *newFile;
        errno_t err = PAL::fopen_s(&newFile, filename.c_str(), append ? "a" : "w");
        SPX_IFFALSE_THROW_HR(err == 0, SPXERR_FILE_OPEN_FAILED);
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

void ReaderWriterLock::EnterRead()
{
    while (writeWaiting.load()) {
        // Yield immediately to give writer priority
        std::this_thread::yield();
    }

    auto currentHolders = readerCount.load();
    auto newVal = currentHolders + 1;
    int loopCount = 0;
    while (!readerCount.compare_exchange_weak(currentHolders, newVal))
    {
        if (loopCount++ == 100 || currentHolders == -1)
        {
            loopCount = 0;
            std::this_thread::yield();
        }

        newVal = currentHolders + 1;
    }
}

void ReaderWriterLock::ExitRead()
{
    readerCount--;
}

void ReaderWriterLock::EnterWrite()
{
    writeWaiting = true;

    auto currentHolders = 0;
    int loopCount = 0;
    while (!readerCount.compare_exchange_weak(currentHolders, -1))
    {
        if (loopCount++ == 100)
        {
            loopCount = 0;
            std::this_thread::yield();
        }

        currentHolders = 0;
    }
}

void ReaderWriterLock::ExitWrite()
{
    readerCount.store(0);
    writeWaiting.store(false);
}
