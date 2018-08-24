//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// iobuffer.h: defines IOBUFFER and related functions.
//

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LOCK_HANDLE;
typedef void* COND_HANDLE;
typedef struct BUFFER_TAG* BUFFER_HANDLE;

/**
* Defines IOBUFFER which is used for communication with the service.
*/
typedef struct _IOBUFFER
{
    LOCK_HANDLE lockerHandle;
    COND_HANDLE dataConditionHandle; // signals that new data is available
    BUFFER_HANDLE bufferHandle;
    int readBytes;
    int totalBytes;
    int hasCompleted; // indicates that the IOBuffer is done receiving new data
    int hasNewCome; // indicates that IoBufferWrite() was called and new data is available
    int refCount; // reference count for this IOBUFFER.  The buffer is released when the count hits 0.
    int activeReaders;  // number of threads currently waiting on hDataCondition
} IOBUFFER;

/**
* Creates a new IOBUFFER.
* @return A pointer to the new IOBUFFER.
*/
IOBUFFER* IoBufferNew();

/**
* Deletes an IOBUFFER.
* @param buffer The pointer to the IOBUFFER to be deleted.
*/
void IoBufferDelete(IOBUFFER* buffer);

/**
* Increase the reference count of the IOBUFFER.
* @param buffer The pointer to the IOBUFFER.
*/
void IoBufferAddRef(IOBUFFER* buffer);

/**
* Resets the IOBUFFER.
* @param buffer The pointer to the IOBUFFER.
*/
void IoBufferReset(IOBUFFER* buffer);

/**
* Gets the total bytes in the IOBUFFER.
* @param buffer The pointer to the IOBUFFER.
* @return the total bytes in the IOBUFFER.
*/
int IoBufferGetTotalBytes(IOBUFFER* buffer);

/**
* Gets the number of bytes that have not been yet read in the IOBUFFER.
* @param buffer The pointer to the IOBUFFER.
* @return the number of unread bytes in the IOBUFFER.
*/
int IoBufferGetUnReadBytes(IOBUFFER* buffer);

/**
* Waits for the availability of new bytes in the IOBUFFER.
* @param buffer The pointer to the IOBUFFER.
* @param timeout The time-out interval value.
* @return 0 if data becomes available before timeout, -1 means timeout.
*/
int IoBufferWaitForNewBytes(IOBUFFER* buffer, int timeout);

/**
* Reads data in the IOBUFFER, but without changing the state of the IOBUFFER.
* @param buffer The pointer to IOBUFFER.
* @param targetBuffer The buffer to store the data read from the IOBUFFER.
* @param offset The offset in the targetBuffer indicating the start position to store the data.
* @param bytesToRead The number of bytes to read.
* @param timeout The time-out interval value.
* @return the number of bytes that have been read.
*/
int IoBufferPeekRead(IOBUFFER* buffer, void* targetBuffer, int offset, int bytesToRead, int timeout);

/**
* Reads data in the IOBUFFER. The state of the IOBUFFER is updated after return.
* @param buffer The pointer to IOBUFFER.
* @param targetBuffer The buffer to store the data read from the IOBUFFER.
* @param offset The offset in the targetBuffer indicating the start position to store the data.
* @param bytesToRead The number of bytes to read.
* @param timeout The time-out interval value.
* @return the number of bytes that have been read.
*/
int IoBufferRead(IOBUFFER* buffer, void* targetBuffer, int offset, int bytesToRead, int timeout);

/**
* Writes data into the IOBUFFER. The state of the IOBUFFER is updated after return.
* @param buffer The pointer to IOBUFFER.
* @param sourceBuffer The buffer contains data to be written.
* @param offset The offset in the sourceBuffer indicating the start position of data to be written.
* @param bytesToRead The number of bytes to read.
* @return the number of bytes that have been read.
*/
int IoBufferWrite(IOBUFFER* buffer, const void* sourceBuffer, int offset, int bytesToWrite);

#ifdef __cplusplus
}
#endif
