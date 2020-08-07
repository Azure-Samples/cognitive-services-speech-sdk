//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// log_utls.h: Classes to help with logging.
//

#pragma once

#include <string>
#include <ispxinterfaces.h>
#include <iostream>

#define FILE_FILTER_POINTER_LENGTH 64

class LogFilter
{
public:
    LogFilter(LogFilter const&) = delete;
    void operator=(LogFilter const&) = delete;

    void SetFilter(std::string filter);

    bool ShouldLog(const char *format);

    LogFilter()
    {
        memset(&filters, 0, sizeof(filters));
        memset(&filtersRaw, 0, sizeof(filtersRaw));
        memset(&filterPointers, 0, sizeof(filterPointers));
    };

private:
    
    char filters[1024];
    char filtersRaw[1024];
    char *filterPointers[FILE_FILTER_POINTER_LENGTH];
};

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
