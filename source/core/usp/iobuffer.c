//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// iobuffer.c: implements IOBUFFER related functions.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/condition.h"
#include "azure_c_shared_utility/buffer_.h"
#include "iobuffer.h"

IOBUFFER* IoBufferNew()
{
    IOBUFFER* buffer = malloc(sizeof(IOBUFFER));
    if (buffer)
    {
        memset(buffer, 0, sizeof(IOBUFFER));
        buffer->lockerHandle = Lock_Init();
        buffer->dataConditionHandle = Condition_Init();
        buffer->bufferHandle = BUFFER_new();
        buffer->refCount = 1;
        buffer->activeReaders = 0;
    }

    return buffer;
}

void IoBufferDelete(IOBUFFER* buffer)
{
    int refCount = 0;
    Lock(buffer->lockerHandle);
    refCount = --buffer->refCount;
    Unlock(buffer->lockerHandle);
    if (refCount != 0)
    {
        return;
    }

    IoBufferReset(buffer); // ensure IO readers exit

    if (buffer->lockerHandle)
    {
        Lock_Deinit(buffer->lockerHandle);
    }

    if (buffer->dataConditionHandle)
    {
        Condition_Deinit(buffer->dataConditionHandle);
    }

    if (buffer->bufferHandle)
    {
        BUFFER_delete(buffer->bufferHandle);
    }

    free(buffer);
}

void IoBufferAddRef(IOBUFFER* buffer)
{
    Lock(buffer->lockerHandle);
    ++buffer->refCount;
    Unlock(buffer->lockerHandle);
}

void IoBufferReset(IOBUFFER* buffer)
{
    Lock(buffer->lockerHandle);

    // make sure all active readers exit as part of the reset
    if (buffer->activeReaders > 0)
    {
        buffer->readBytes = 0;
        buffer->hasNewCome = 1;
        buffer->hasCompleted = 1;
        buffer->totalBytes = 0;
        Condition_Post(buffer->dataConditionHandle);
        while (buffer->activeReaders > 0)
        {
            // spin until they exit
            Unlock(buffer->lockerHandle);
            ThreadAPI_Sleep(5);
            Lock(buffer->lockerHandle);
        }
    }

    buffer->readBytes = 0;
    buffer->hasCompleted = 0;
    buffer->totalBytes = 0;
    buffer->hasNewCome = 0;
    Unlock(buffer->lockerHandle);
}

int IoBufferGetTotalBytes(IOBUFFER* buffer)
{
    int ret = 0;
    Lock(buffer->lockerHandle);
    ret = buffer->hasCompleted ? buffer->totalBytes : -1;
    Unlock(buffer->lockerHandle);
    return ret;
}

int IoBufferGetUnReadBytes(IOBUFFER* buffer)
{
    int ret = 0;
    Lock(buffer->lockerHandle);
    ret = buffer->totalBytes - buffer->readBytes;
    Unlock(buffer->lockerHandle);
    return ret;
}

int IoBufferWaitForNewBytes(IOBUFFER* buffer, int timeout)
{
    COND_RESULT waitret = COND_OK;
    Lock(buffer->lockerHandle);
    ++buffer->activeReaders;
    if (!buffer->hasNewCome)
    {
        waitret = Condition_Wait(buffer->dataConditionHandle, buffer->lockerHandle, timeout);
    }

    buffer->hasNewCome = 0;
    --buffer->activeReaders;
    Unlock(buffer->lockerHandle);
    return waitret == COND_OK ? 0 : -1;
}

static int IoBufferReadInternal(IOBUFFER* buffer, void* targetBuffer, int offset, int bytesToRead, int timeout, int isPeek)
{
    COND_RESULT waitret = COND_OK;
    Lock(buffer->lockerHandle);
    ++buffer->activeReaders;
    while (waitret == COND_OK && buffer->totalBytes < buffer->readBytes + bytesToRead && !buffer->hasCompleted)
    {
        waitret = Condition_Wait(buffer->dataConditionHandle, buffer->lockerHandle, timeout);
    }

    if (buffer->totalBytes < buffer->readBytes + bytesToRead)
    {
        bytesToRead = -1;
    }
    else
    {
        memcpy((uint8_t*)targetBuffer + offset, BUFFER_u_char(buffer->bufferHandle) + buffer->readBytes, bytesToRead);
        if (!isPeek)
        {
            buffer->readBytes += bytesToRead;
        }
    }

    buffer->hasNewCome = 0;
    --buffer->activeReaders;
    Unlock(buffer->lockerHandle);

    return bytesToRead;
}

int IoBufferPeekRead(IOBUFFER* buffer, void* targetBuffer, int offset, int bytesToRead, int timeout)
{
    return IoBufferReadInternal(buffer, targetBuffer, offset, bytesToRead, timeout, 1);
}


int IoBufferRead(IOBUFFER* buffer, void* targetBuffer, int offset, int bytesToRead, int timeout)
{
    return IoBufferReadInternal(buffer, targetBuffer, offset, bytesToRead, timeout, 0);
}

int IoBufferWrite(IOBUFFER* buffer, const void* sourceBuffer, int offset, int bytesToWrite)
{
    int ret = bytesToWrite;
    Lock(buffer->lockerHandle);
    if (sourceBuffer == NULL)
    {
        buffer->hasCompleted = 1;
    }
    else
    {
        if (buffer->totalBytes + bytesToWrite > (int)BUFFER_length(buffer->bufferHandle) &&
            BUFFER_enlarge(buffer->bufferHandle, buffer->totalBytes + bytesToWrite))
        {
            ret = -1;
        }
        else
        {
            memcpy(BUFFER_u_char(buffer->bufferHandle) + buffer->totalBytes, (uint8_t*)sourceBuffer + offset, bytesToWrite);
            buffer->totalBytes += bytesToWrite;
        }
    }

    if (!buffer->hasNewCome)
    {
        Condition_Post(buffer->dataConditionHandle);
        buffer->hasNewCome = 1;
    }

    Unlock(buffer->lockerHandle);
    return ret;
}
