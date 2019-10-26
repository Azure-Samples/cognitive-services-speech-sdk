//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// read_write_ring_buffer.cpp: Implementation definitions for CSpxReadWriteRingBuffer C++ class
//
//      The ReadWriteRingBuffer (RB) owns and manages a conceptually infinite memory buffer
//      into/from which clients can write/read data.
//
//      Data can be accessed:
//      (1) Sequentially via an internally maintained ReadPtr/ReadPos, or
//      (2) Randomly by caller supplied Pos, if the data is still available in the SlidingWindow.
//      (3) Using CallerSupplied (copies the data) or CalleeManaged (view into DataStorage) DataBuffers.
//      (4) Using ExactSize or ReducedSize DataAccess and DataStorage.
//
//      Additional Caller requirements/guarantees:
//      (1) TwoPhaseInitialization required via ISpxReadWriteBufferInit::SetSize().
//      (2) Optional TwoPhaseDestruction via ISpxObjectInit::Term().
//      (3) Maintains a copy of the data supplied via ISpxReadWriteBuffer::Write().
//      (4) ExactSize DataAccess invocations ThrowExceptions if not enough data available.
//
//      From RB's POV:
//
//      - DataStorage - means ISpxReadWriteBuffer:Write() method.
//      - ExactSize/ReducedSize DataStorage
//        - ExactSize DataStorage is indiciated by null bytesWritten pointer.
//        - ReducedSize DataStorage is indiciated by non-null bytesWritten pointer.
//        - Two different models of DataStorage
//          (a) ExactSize requires that the size of data requested to be written by the caller, must
//              fit inside the SlidingWindow. If it does not, an exception is thrown.
//          (b) ReducedSize removes the exception, and instead always returns successfully, indicating
//              the number of bytes actually written via the non-null bytesWritten pointer supplied
//              in the DataStorage method by the caller. Thus the amount of data actually stored is
//              governed by the max(0, space left in SlidingWindow) and min(amount requested, space left
//              in the SlidingWindow).
//
//      - DataAccess - means ISpxReadWriteBuffer::Read*() methods.
//      - ExactSize/ReducedSize DataAccess
//        - ExactSize DataAccess is indiciated by null bytesRead pointer.
//        - ReducedSize DataAccess is indiciated by non-null bytesRead pointer.
//        - Two different models of DataAccess
//          (a) ExactSize requires the size of data requested by caller to be available; if less
//              data is available, an exception is thrown. If at least that much data is available,
//              the DataAccess method returns exactly that number of bytes to the caller.
//          (b) ReducedSize removes the exception, and instead always returns successfully, indicating
//              the number of bytes actually read via the non-null bytesRead pointer supplied in the
//              DataAccess method by the caller. Thus the amount of data returned is governed by the 
//              max(0, amount available) and min(amount available, amount requested).
//
//      - SlidingWindow - data still available in the RB's managed buffer
//        - "to the left of" WritePtr/WritePos
//        - "to the right of" max(InitPos, GetWritePos() - GetSize())
//
//      - CallerSupplied/CalleeManaged DataBuffers
//        (a) CallerSupplied DataBuffers - means ```data``` parameter in Read()/ReadAtBytePos() methods.
//        (b) CalleeManaged DataBuffers - means both
//            - SpxSharedUint8Buffer_Type RingBuffer, and/or
//            - std::shared_ptr<uint8_t> returned from ReadShared*()
//
//      NOTEs:
//      - ReadPtr/ReadPos cannot advance past WritePtr/WritePos
//      - WritePtr/WritePos cannot advance past ReadPtr/ReadPos + GetSize()
//

// ROBCH: Introduced in AUDIO.V3

#include "stdafx.h"
#include "read_write_ring_buffer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

/// <summary>
/// CSpxReadWriteRingBuffer constructor - minimal initialization; TwoPhaseInitialization
/// required via ISpxReadWriteBufferInit::SetSize().
/// </summary>
/// <remarks>No object locking required.</remarks>
CSpxReadWriteRingBuffer::CSpxReadWriteRingBuffer() :
    m_ringSize(0),
    m_allowOverflow{false},
    m_initPos(0),
    m_writePos(0),
    m_readPos(0),
    m_ptr1(nullptr),
    m_ptr2(nullptr),
    m_writePtr(nullptr),
    m_readPtr(nullptr)
{
}

/// <summary>
/// CSpxReadWriteRingBuffer destructor - Termination _MAY_ have already occurred via ISpxObjectInit::Term().
/// </summary>
/// <remarks>NO LOCK REQUIRED; by contract, no methods can be called (by external callers) after dtor starts.</remarks>
CSpxReadWriteRingBuffer::~CSpxReadWriteRingBuffer()
{
    // 
    SetZeroRingSize();
}

/// <summary>
/// ISpxReadWriteBufferInit::SetSize - Set the size of the RB SlidingWindow.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access.</remarks>
size_t CSpxReadWriteRingBuffer::SetSize(size_t size)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return SetSizeInternal(size);
}

/// <summary>
/// ISpxReadWriteBufferInit::SetInitPos - Sets the SlidingWindow's InitialPosition.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access; ThrowsException if already initialized. </remarks>
void CSpxReadWriteRingBuffer::SetInitPos(uint64_t pos)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(m_initPos != 0 || m_readPos != 0 || m_writePos != 0, SPXERR_ALREADY_INITIALIZED);

    m_initPos = pos;
    SetSizeInternal(m_ringSize);
}

/// <summary>
/// ISpxReadWriteBufferInit::AllowOverflow - Allows the buffer to overflow so old data will be discarded
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access; ThrowsException if already initialized. </remarks>
void CSpxReadWriteRingBuffer::AllowOverflow(bool allow)
{
    m_allowOverflow = allow;
}


/// <summary>
/// ISpxReadWriteBufferInit::SetName - Sets "Name" for this ring buffer; sometimes useful for debugging purposes.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access; ThrowsException if already initialized. </remarks>
void CSpxReadWriteRingBuffer::SetName(const std::string& name)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SPX_IFTRUE_THROW_HR(!m_ringName.empty(), SPXERR_ALREADY_INITIALIZED);

    m_ringName = name;
}

/// <summary>
/// ISpxReadWriteBufferInit::Term - Release references to all resources "below".
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access</remarks>
void CSpxReadWriteRingBuffer::Term()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    SetZeroRingSize();
}

/// <summary>
/// ISpxReadWriteBufferInit::GetSize - Gets the SlidingWindow size.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access; ThrowsException if not already initialized. </remarks>
size_t CSpxReadWriteRingBuffer::GetSize() const
{
    std::unique_lock<std::mutex> lock(const_cast<CSpxReadWriteRingBuffer*>(this)->m_mutex);
    ThrowExceptionIfNotInitialized();

    return m_ringSize;
}

/// <summary>
/// ISpxReadWriteBufferInit::GetInitPos - Gets the SlidingWindow's InitialPosition.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access; ThrowsException if not already initialized. </remarks>
uint64_t CSpxReadWriteRingBuffer::GetInitPos() const
{
    std::unique_lock<std::mutex> lock(const_cast<CSpxReadWriteRingBuffer*>(this)->m_mutex);
    ThrowExceptionIfNotInitialized();

    return m_initPos;
}

/// <summary>
/// ISpxReadWriteBufferInit::GetName - Gets the SlidingWindow's InitialPosition.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access; ThrowsException if not already initialized. </remarks>
std::string CSpxReadWriteRingBuffer::GetName() const
{
    std::unique_lock<std::mutex> lock(const_cast<CSpxReadWriteRingBuffer*>(this)->m_mutex);
    return m_ringName;
}

/// <summary>
/// ISpxReadWriteBuffer::Write - DataStorage method to store a CalleeManaged copy of the CallerSupplied
/// DataBuffer in the RB's SlidingWindow at the WritePtr/WritePos using either ExactSize or ReducedSize.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if (a) not already initialized or,
/// (b) if there isn't enough space in the SlidingWindow when using ExactSize DataStorage.
/// </remarks>
void CSpxReadWriteRingBuffer::Write(const void* data, size_t dataSizeInBytes, size_t* bytesWritten)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    ThrowExceptionIfNotInitialized();

    EnsureSpaceToWrite(&dataSizeInBytes, bytesWritten);
    InternalWriteToRing(data, dataSizeInBytes, bytesWritten);
}

/// <summary>
/// ISpxReadWriteBuffer::GetWritePos - Gets the WritePos, which indicates the location of the next byte
/// to be used for DataStorage with ISpxReadWriteBuffer::Write().
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if not already initialized.</remarks>
uint64_t CSpxReadWriteRingBuffer::GetWritePos() const
{
    std::unique_lock<std::mutex> lock(const_cast<CSpxReadWriteRingBuffer*>(this)->m_mutex);
    ThrowExceptionIfNotInitialized();

    return m_writePos;
}

/// <summary>
/// ISpxReadWriteBuffer::Read - DataAccess method used to read data into CallerSupplied DataBuffer from the
/// RB's SlidingWindow at the ReadPos/ReadPtr using either ExactSize or ReducedSize.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if (a) not already initialized or,
/// (b) if there isn't enough data available in the SlidingWindow at the ReadPos/ReadPtr when using
/// ExactSize DataAccess.
/// </remarks>
void CSpxReadWriteRingBuffer::Read(void* data, size_t dataSizeInBytes, size_t* bytesRead)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    ThrowExceptionIfNotInitialized();

    EnsureSpaceToReadAtPos(m_readPos, &dataSizeInBytes, bytesRead);
    InternalReadFromRing(data, dataSizeInBytes, bytesRead);
}

/// <summary>
/// ISpxReadWriteBuffer::GetReadPos - Gets the ReadPos, which indicates the location of the next byte where
/// DataAccess will begin via ISpxReadWriteBuffer::Read().
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if not already initialized.</remarks>
uint64_t CSpxReadWriteRingBuffer::GetReadPos() const
{
    std::unique_lock<std::mutex> lock(const_cast<CSpxReadWriteRingBuffer*>(this)->m_mutex);
    ThrowExceptionIfNotInitialized();

    return m_readPos;
}

/// <summary>
/// ISpxReadWriteBuffer::ResetReadPos - Updates the ReadPos/ReadPtr to match the current WritePos/WritePtr.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if not already initialized.</remarks>
uint64_t CSpxReadWriteRingBuffer::ResetReadPos()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    ThrowExceptionIfNotInitialized();

    m_readPos = m_writePos;
    m_readPtr = m_writePtr;

    return m_readPos;
}

/// <summary>
/// ISpxReadWriteBuffer::ReadAtBytePos - DataAccess method used to read data into CallerSupplied DataBuffer from the
/// RB's SlidingWindow at the specified position using either ExactSize or ReducedSize DataAccess.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if (a) not already initialized or,
/// (b) if there isn't enough data available in the SlidingWindow at the specified position when using
/// ExactSize DataAccess.
/// </remarks>
void CSpxReadWriteRingBuffer::ReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    ThrowExceptionIfNotInitialized();

    void* readPtr = nullptr;
    ConvertPosToRingPtr(pos, &readPtr);

    EnsureSpaceToReadAtPos(pos, &dataSizeInBytes, bytesRead);
    InternalReadFromRingPtr(data, dataSizeInBytes, bytesRead, readPtr);
}

/// <summary>
/// ISpxReadWriteBuffer::ReadShared - DataAccess method used to return CalleeManaged DataBuffer initialized from
/// the RB's SlidingWindow at the ReadPos/ReadPtr using either ExactSize or ReducedSize.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if (a) not already initialized or,
/// (b) if there isn't enough data available in SlidingWindow at ReadPos/ReadPtr when using ExactSize DataAccess.
/// </remarks>
std::shared_ptr<uint8_t> CSpxReadWriteRingBuffer::ReadShared(size_t dataSizeInBytes, size_t* bytesRead)
{
    // CONSIDER: Optimize to return shared_ptr directly into ring buffer memory
    auto data = SpxAllocSharedUint8Buffer(dataSizeInBytes);
    Read(data.get(), dataSizeInBytes, bytesRead);
    return data;
}

/// <summary>
/// ISpxReadWriteBuffer::ReadShared - DataAccess method used to return CalleeManaged DataBuffer initialized from
/// the RB's SlidingWindow at the ReadPos/ReadPtr using either ExactSize or ReducedSize.
/// </summary>
/// <remarks>LOCK REQUIRED to guard multithreaded access. ThrowsException if (a) not already initialized or,
/// (b) if there isn't enough data available in SlidingWindow at specified position when using ExactSize DataAccess.
/// </remarks>
std::shared_ptr<uint8_t> CSpxReadWriteRingBuffer::ReadSharedAtBytePos(uint64_t pos, size_t dataSizeInBytes, size_t* bytesRead)
{
    // CONSIDER: Optimize to return shared_ptr directly into ring buffer memory
    auto data = SpxAllocSharedUint8Buffer(dataSizeInBytes);
    ReadAtBytePos(pos, data.get(), dataSizeInBytes, bytesRead);
    return data;
}

/// <summary>
/// CSpxReadWriteRingBuffer::ThrowExceptionIfNotInitialized - Checks to see if already initialized... If not, throw exception.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::ThrowExceptionIfNotInitialized() const
{
    SPX_IFTRUE_THROW_HR(m_writePtr == nullptr || m_readPtr == nullptr, SPXERR_UNINITIALIZED);
}

/// <summary>
/// ISpxReadWriteBufferInit::SetSizeInternal - Set the size of the RB SlidingWindow.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
size_t CSpxReadWriteRingBuffer::SetSizeInternal(size_t size)
{
    return size == 0 ? SetZeroRingSize() : SetNonZeroRingSize(size);
}

/// <summary>
/// ISpxReadWriteBufferInit::SetZeroRingSize - Resets the SlidingWindow, releasing resources "below".
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
size_t CSpxReadWriteRingBuffer::SetZeroRingSize()
{
    m_readPtr = m_writePtr = nullptr;
    m_ptr1 = m_ptr2 = nullptr;

    m_ring = nullptr;
    m_ringSize = 0;

    m_readPos = m_writePos = m_initPos = 0;

    return m_ringSize;
}

/// <summary>
/// CSpxReadWriteRingBuffer::SetNonZeroRingSize - Sets the size of the RB SlidingWindow to a non-zero value.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
size_t CSpxReadWriteRingBuffer::SetNonZeroRingSize(size_t size)
{
    auto buffer = (size == m_ringSize) ? m_ring : SpxAllocSharedUint8Buffer(size);
    SPX_IFTRUE_THROW_HR(buffer.get() == nullptr, SPXERR_OUT_OF_MEMORY);

    m_ring = buffer;
    m_ringSize = size;

    m_ptr1 = buffer.get();
    m_ptr2 = m_ptr1 + size;

    m_writePos = m_initPos;
    m_readPos = m_initPos;

    m_readPtr = m_writePtr = &m_ptr1[m_initPos % m_ringSize];

    return m_ringSize;
}

/// <summary>
/// CSpxReadWriteRingBuffer::EnsureSpaceToWrite - Checks to ensure there's enough space to write the specfied
/// amount of data. If there's not enough space and this is an ExactSize DataStorage operation, throw exception.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::EnsureSpaceToWrite(size_t *bytesToWrite, const size_t *bytesActuallyWritten)
{
    SPX_DBG_ASSERT(bytesToWrite != nullptr);
    size_t bytesCanWrite = m_ringSize - (size_t)(m_writePos - m_readPos);
    if (*bytesToWrite > bytesCanWrite)
    {
        if (!m_allowOverflow)
        {
            SPX_IFTRUE_THROW_HR(bytesActuallyWritten == nullptr, SPXERR_BUFFER_TOO_SMALL);
            *bytesToWrite = bytesCanWrite;
        }
        else
        {
            // We drop the old data to allow for the write to happen.
            UpdateReadPosition(*bytesToWrite - bytesCanWrite);

            // Make sure the condition came through at this time
            SPX_DBG_ASSERT(*bytesToWrite <= m_ringSize - (size_t)(m_writePos - m_readPos));
            // Move m_readPos ahead so the buffer can accomodate the value to write. 
        }
    }
}

/// <summary>
/// CSpxReadWriteRingBuffer::EnsureSpaceToReadAtPos - Checks to ensure there's enough space to read the specfied
/// amount of data at the specified postion. If there's not enough space and this is an ExactSize DataAccess
/// operation, throw exception.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::EnsureSpaceToReadAtPos(uint64_t pos, size_t* bytesToRead, const size_t* bytesActuallyRead)
{
    SPX_DBG_ASSERT(bytesToRead != nullptr);
    size_t bytesCanRead = (m_writePos > pos) ? (size_t)(m_writePos - pos) : 0;
    if (*bytesToRead > bytesCanRead)
    {
        SPX_IFTRUE_THROW_HR(bytesActuallyRead == nullptr, SPXERR_RINGBUFFER_DATA_UNAVAILABLE);
        *bytesToRead = bytesCanRead;
    }
}

/// <summary>
/// CSpxReadWriteRingBuffer::ConvertPosToRingPtr - Convert the specified position to a ptr that points into the
/// SlidingWindow at that data position. If the requested position is outside of the SlidingWindow, throw exception.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::ConvertPosToRingPtr(uint64_t pos, void** ptr)
{
    SPX_IFTRUE_THROW_HR(pos > m_writePos, SPXERR_RINGBUFFER_DATA_UNAVAILABLE);
    SPX_IFTRUE_THROW_HR(pos < m_initPos, SPXERR_RINGBUFFER_DATA_UNAVAILABLE);
    SPX_IFTRUE_THROW_HR(m_writePos - pos > m_ringSize, SPXERR_RINGBUFFER_DATA_UNAVAILABLE);

    if (pos >= m_ringSize)
    {
        pos = pos % m_ringSize;
    }

    if (ptr != nullptr)
    {
        *ptr = m_ptr1 + pos;
    }
}

/// <summary>
/// CSpxReadWriteRingBuffer::InternalWriteToRing - Write the CallerSupplied DataBuffer into the SlidingWindow at
/// the current WritePos/WritePtr, and then update the current WritePos/WritePtr appropriately.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::InternalWriteToRing(void const* data, size_t bytesToWrite, size_t* bytesActuallyWritten)
{
    // Write to the ring buffer
    InternalWriteToRingPtr(data, bytesToWrite, bytesActuallyWritten, m_writePtr);

    // Update our write position and pointer
    m_writePos += bytesToWrite;
    m_writePtr += bytesToWrite;

    // If our write pointer wraps... fix it... 
    if (m_writePtr >= m_ptr2)
    {
        m_writePtr = m_ptr1 + (m_writePtr - m_ptr2);
    }
}

/// <summary>
/// CSpxReadWriteRingBuffer::InternalWriteToRingPtr - Write the CallerSupplied DataBuffer into the SlidingWindow
/// at the location specified.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::InternalWriteToRingPtr(void const* data, size_t bytesToWrite, size_t* bytesActuallyWritten, void* ptr)
{
    // This will never happen... Thus ... Let's check that it doesn't (only in debug builds)
    SPX_DBG_ASSERT(ptr >= m_ptr1 && ptr < m_ptr2);

    // If we can do it in one call to memcpy, do that...
    if ((uint8_t*)ptr + bytesToWrite <= m_ptr2)
    {
        memcpy(ptr, data, bytesToWrite);
    }
    else
    {
        // Copy the first part...
        auto part1Size = (size_t)(m_ptr2 - (uint8_t*)ptr);
        memcpy(ptr, data, part1Size);

        // Update our pointers...
        data = ((uint8_t*)data) + part1Size;
        ptr = m_ptr1;

        // Copy the second part
        auto part2Size = bytesToWrite - part1Size;
        memcpy(ptr, data, part2Size);
    }

    // If the caller wants to know how much we wrote, let em know...
    if (bytesActuallyWritten != nullptr)
    {
        *bytesActuallyWritten = bytesToWrite;
    }
}

/// <summary>
/// CSpxReadWriteRingBuffer::InternalReadFromRing - Read data into the CallerSupplied DataBuffer from the
/// SlidingWindow at the current ReadPos/ReadPtr and then update the current ReadPos/ReadPtr appropriately.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::InternalReadFromRing(void* data, size_t bytesToRead, size_t* bytesActuallyRead)
{
    // Read from the ring buffer
    InternalReadFromRingPtr(data, bytesToRead, bytesActuallyRead, m_readPtr);
    UpdateReadPosition(bytesToRead);
}

/// <summary>
/// CSpxReadWriteRingBuffer::UpdateReadPosition - update the current ReadPos/ReadPtr appropriately
/// after reading data from the SlidingWindow at the current ReadPos/ReadPtr or after overflow.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::UpdateReadPosition(size_t bytesRead)
{
    // Update our read position and pointer
    m_readPos += bytesRead;
    m_readPtr += bytesRead;

    // If our read pointer wraps... fix it...
    if (m_readPtr >= m_ptr2)
    {
        m_readPtr = m_ptr1 + (m_readPtr - m_ptr2);
    }
}

/// <summary>
/// CSpxReadWriteRingBuffer::InternalReadFromRingPtr - Read data into the CallerSupplied DataBuffer from the
/// SlidingWindow at the current specified postion.
/// </summary>
/// <remarks>NO LOCK REQUIRED - always called from methods that have already acquired lock.</remarks>
void CSpxReadWriteRingBuffer::InternalReadFromRingPtr(void* data, size_t bytesToRead, size_t* bytesActuallyRead, void* ptr)
{
    // This will never happen... Thus ... Let's check that it doesn't (only in debug builds)
    SPX_DBG_ASSERT(ptr >= m_ptr1 && ptr < m_ptr2);

    // If the caller asked for the data, copy it into the data buffer supplied
    if (data != NULL)
    {
        // If we can copy the data using one call to memcpy, do that...
        if ((uint8_t*)ptr + bytesToRead <= m_ptr2)
        {
            memcpy(data, ptr, bytesToRead);
        }
        else // ... otherwise ...
        {
            // Copy the first part...
            auto part1Size = (size_t)(m_ptr2 - (uint8_t*)ptr);
            memcpy(data, ptr, part1Size);

            // Update our pointers...
            data = ((uint8_t*)data) + part1Size;
            ptr = m_ptr1;

            // And then copy the second part...
            auto part2Size = bytesToRead - part1Size;
            memcpy(data, ptr, part2Size);
        }
    }

    // If the caller wants to know how much we read, let em know...
    if (bytesActuallyRead != nullptr)
    {
        *bytesActuallyRead = bytesToRead;
    }
}

} } } } // Microsoft::CognitiveServices::Speech::Impl
