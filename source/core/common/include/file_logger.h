//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// file_logger.h: FileLogger() implementation declaration
//

#include <string>
#include <atomic>
#include <ispxinterfaces.h>
#include <iostream>

#define FILE_FILTER_POINTER_LENGTH 64

class ReaderWriterLock
{
public:
    ReaderWriterLock()
    {
        writeWaiting.store(false);
        readerCount.store(0);
    }
    ReaderWriterLock(ReaderWriterLock &&) {}

    void EnterRead();
    void ExitRead();
    void EnterWrite();
    void ExitWrite();

private:
    std::atomic_bool writeWaiting;
    std::atomic_int readerCount;
};

class WriteLock
{
public:
    WriteLock(ReaderWriterLock *lock)
    {
        m_lock = lock;
        m_lock->EnterWrite();
    }

    ~WriteLock()
    {
        m_lock->ExitWrite();
    }
private:
    ReaderWriterLock *m_lock;
};

class ReadLock
{
public:
    ReadLock(ReaderWriterLock *lock)
    {
        m_lock = lock;
        m_lock->EnterRead();
    }

    ~ReadLock()
    {
        m_lock->ExitRead();
    }
private:
    ReaderWriterLock *m_lock;
};


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

    void LogToFile(std::string&& format);
private:
    FileLogger()
    {
        memset(&filters, 0, sizeof(filters));
        memset(&filtersRaw, 0, sizeof(filtersRaw));
        memset(&filterPointers, 0, sizeof(filterPointers));
    };

    std::string filename;
    std::string baseFilename;
    bool append = false;

    ReaderWriterLock fileNameLock;

    uint32_t currentFileAppendix;
    std::chrono::steady_clock::time_point lastFileStartTime = std::chrono::steady_clock::time_point::min();
    std::atomic_size_t fileDataWritten;

    char filters[1024];
    char filtersRaw[1024];
    char *filterPointers[FILE_FILTER_POINTER_LENGTH];

    void AssignFile();
    std::string BuildFileName(std::string currentName);

    volatile FILE* file = nullptr;
    std::mutex mtx;
};
