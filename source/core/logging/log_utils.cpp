//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "log_utils.h"
#include <stdio.h>
#include <string.h>

void LogFilter::SetFilter(std::string filter)
{
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
}

bool LogFilter::ShouldLog(const char * logLine)
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

            if (strstr(logLine, currentFilter))
            {
                log = true;
                break;
            }
        }
    }

    return log;
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
