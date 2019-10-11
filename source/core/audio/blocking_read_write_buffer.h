//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// blocking_read_write_buffer.h: Implementation declarations for CSpxBlockingReadWriteBuffer
//
//      The BlockingReadWriteBuffer implements the curiously recurring template pattern (CRTP) extending
//      a "ReadWriteBuffer" implementation with BlockingRead DataAccess.
//
//      In "ReadWriteBuffer" implementations without BlockingRead DataAccess, two forms of DataAccess
//      exist: ExactSize and ReducedSize...
//      (a) ExactSize throws an exception if the specified amount of data is not immediately available.
//      (b) ReducedSize allows the data size available to be smaller than that requested, returning
//          the actual size of the DataAccessed in an output parameter (e.g. size_t* bytesRead).
//
//      For both forms of DataAccess, BlockingRead modifies the standard behavior by blocking the
//      calling thread until the requested amount of data is available instead of throwing an exception or
//      returning less data that requested. Additionally, there is a way to "unblock" the BlockingRead
//      DataAccess triggering the original behavior of the non-BlockingRead DataAccess pattern above with
//      whatever data is already available.
//
//      Thus, ExactSize BlockingRead DataAccess operates as follows:
//      (1) Go to next step if enough data is already available, otherwise wait for data to be available.
//      (2) Once enough data is available, return the data (via CallerSupplied or CalleeManaged DataBuffers).
//      (3) If, while waiting in step #1, someone signals the UnblockTrigger, throw an exception immediately.
//
//      Whereas, ReducedSize BlockingRead DataAccess operates as follows:
//      (1) Go to next step if enough data is already available, otherwise wait for data to be available.
//      (2) Once enough data is available, return the data (via CallerSupplied or CalleeManaged DataBuffers).
//      (3) If, while waiting in step #1, someone signals the UnblockTrigger, return whatever data is
//          already available, returning the actual size of the DataAccessed in the bytesRead output parameter.
//
//      From the BlockingReadWriteBuffer's POV:
//      - UnblockTrigger - means when Caller FromAbove calls ISpxReadWriteBuffer::Write(nullptr, 0)
//
//      NOTE:
//      - ISpxReadWriteBuffer::ReadAtBytePos and ISpxReadWriteBuffer::ReadSharedAtBytePos are unaltered by
//        this BlockingReadWriteBuffer implementation.
//

// ROBCH: Introduced in AUDIO.V3

#pragma once
#include <mutex>
#include "spxcore_common.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class ReadWriteBufferT>
class CSpxBlockingReadWriteBuffer : public ReadWriteBufferT
{
protected:    
    using Base_Type = ReadWriteBufferT;

public:

    /// <summary>
    /// ISpxReadWriteBuffer::Read (override) - DataAccess method that will attempt perform a BlockingRead if enough
    /// data isn't yet available, or simply delegate to the base class immediately if enough data is already available.
    /// </summary>
    void Read(void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr) override
    {
        auto available = Base_Type::GetBytesReadReady();
        available >= dataSizeInBytes ? BaseRead(data, dataSizeInBytes, bytesRead) : BlockingRead(data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// ISpxReadWriteBuffer::ReadAtBytePos (override) - DataAccess method that will attempt perform a BlockingRead if enough
    /// data isn't yet available, or simply delegate to the base class immediately if enough data is already available.
    /// </summary>
    void ReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead) override
    {
        //void* readPtr = nullptr;
        //ConvertPosToRingPtr(pos, &readPtr);
        auto available = Base_Type::GetBytesReadReadyAtPos(pos);
        available >= dataSizeInBytes ? BaseReadAtBytePos(pos, data, dataSizeInBytes, bytesRead) : BlockingReadAtBytePos(pos, data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// ISpxReadWriteBuffer::ReadShared (override) - DataAccess method that will attempt perform a BlockingReadShared if enough
    /// data isn't yet available, or simply delegate to the base class immediately if enough data is already available.
    /// </summary>
    std::shared_ptr<uint8_t> ReadShared(size_t dataSizeInBytes, size_t* bytesRead = nullptr) override
    {
        auto available = Base_Type::GetBytesReadReady();
        return available >= dataSizeInBytes ? BaseReadShared(dataSizeInBytes, bytesRead) : BlockingReadShared(dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// ISpxReadWriteBuffer::Write (override) - DataStorage method that will delegate to the base class immediately, after first checking
    /// to see if the UnblockTrigger criteria has been met (Write(nullptr, 0)); this method MUST notify all BlockingReads in that condition.
    /// </summary>
    void Write(const void* data, size_t dataSizeInBytes, size_t* bytesWritten = nullptr) override
    {
        m_writeZeroHappened = (data == nullptr && dataSizeInBytes == 0);
        BaseWrite(data, dataSizeInBytes, bytesWritten);
        m_cv.notify_all();
    }

protected:

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::WaitUntilBytesAvailable - Wait until either the amount of data specified
    /// is available, or someone signals the UnblockTrigger.
    /// </summary>
    uint64_t WaitUntilBytesAvailable(uint64_t bytesRequired)
    {
        return WaitUntilBytesAvailable(bytesRequired,
            [this]() -> uint64_t
            {
                return Base_Type::GetBytesReadReady();
            });
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::WaitUntilBytesAvailableAtPos - Wait until either the amount of data specified
    /// is available at the given position, or someone signals the UnblockTrigger.
    /// </summary>
    uint64_t WaitUntilBytesAvailableAtPos(uint64_t pos, uint64_t bytesRequired)
    {
        return WaitUntilBytesAvailable(bytesRequired,
                    [pos, this]() -> uint64_t
                    {
                        return Base_Type::GetBytesReadReadyAtPos(pos);
                    });
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::WaitUntilBytesAvailable - Wait until either the amount of data specified with the lambda provided to do the check.
    /// is available at the given position, or someone signals the UnblockTrigger.
    /// </summary>
    uint64_t WaitUntilBytesAvailable(uint64_t bytesRequired, std::function<uint64_t()> checkDataAvailableFunc)
    {
        // If the last write was an "UnblockTrigger", we won't wait at all...
        while (!m_writeZeroHappened)
        {
            auto bytesReadReadyBeforeWait = checkDataAvailableFunc();
            if (bytesReadReadyBeforeWait >= bytesRequired) break; // we don't need to wait any more... we have enough data...

            // Typically data should be flowing regularly and smoothly. However, if, callers are attempting DataAccess for
            // very very large DataBuffers, or we aren't receiving DataStorage DataBuffers regularly, we'll wake up here
            // periodically and log that this is happening...
            auto timeout = std::chrono::milliseconds(200);
            auto wakeFn = [&] {
                auto bytesReadReadyNow = checkDataAvailableFunc();
                return bytesReadReadyNow != bytesReadReadyBeforeWait || m_writeZeroHappened;
            };

            std::unique_lock<std::mutex> lock(m_mutex);
            m_cv.wait_for(lock, timeout, wakeFn);

            auto bytesReadyNow = checkDataAvailableFunc();
            if (bytesReadyNow >= bytesRequired) break; // we don't need to log that we've been waiting...

            SPX_TRACE_INFO("WaitUntilBytesAvailable: available=%" PRIu64 "; required=%" PRIu64 " writeZero=%s ...",
                bytesReadyNow, bytesRequired, (m_writeZeroHappened ? "true" : "false"));
        }

        // Now that we know how much data is available, let the caller know
        auto bytesReadReadyNow = checkDataAvailableFunc();
        return std::min(bytesRequired, bytesReadReadyNow);
    }


    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BlockingRead - Block until the requested number of bytes are available
    /// or the UnblockTrigger is signaled. Then, we'll go ahead and do the normal BaseRead behavior.
    /// </summary>
    void BlockingRead(void* data, size_t dataSizeInBytes, size_t* bytesRead)
    {
        dataSizeInBytes = (size_t)WaitUntilBytesAvailable(dataSizeInBytes);
        BaseRead(data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BlockingRead - Block until the requested number of bytes are available
    /// or the UnblockTrigger is signaled. Then, we'll go ahead and do the normal BaseRead behavior.
    /// </summary>
    void BlockingReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead)
    {
        dataSizeInBytes = (size_t)WaitUntilBytesAvailableAtPos(pos, dataSizeInBytes);
        BaseReadAtBytePos(pos, data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BlockingReadShared - Block until the requested number of bytes are
    /// available or the UnblockTrigger is signaled. Then, we'll go ahead and do the normal BaseReadShared
    /// behavior.
    /// </summary>
    std::shared_ptr<uint8_t> BlockingReadShared(size_t dataSizeInBytes, size_t* bytesRead)
    {
        dataSizeInBytes = (size_t)WaitUntilBytesAvailable(dataSizeInBytes);
        return BaseReadShared(dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BaseRead - Simply delegate to the base class's implementation of ISpxReadWriteBuffer::Read().
    /// </summary>
    void BaseRead(void* data, size_t dataSizeInBytes, size_t* bytesRead)
    {
        Base_Type::Read(data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BaseReadAtBytePos - Simply delegate to the base class's implementation of ISpxReadWriteBuffer::ReadAtBytePos().
    /// </summary>
    void BaseReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead)
    {
        Base_Type::ReadAtBytePos(pos, data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BaseReadShared - Simply delegate to the base class's implementation of ISpxReadWriteBuffer::ReadShared().
    /// </summary>
    std::shared_ptr<uint8_t> BaseReadShared(size_t dataSizeInBytes, size_t* bytesRead)
    {
        return Base_Type::ReadShared(dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BaseWrite - Simply delegate to the base class's implementation of ISpxReadWriteBuffer::Write().
    /// </summary>
    void BaseWrite(const void* data, size_t dataSizeInBytes, size_t* bytesWritten)
    {
        Base_Type::Write(data, dataSizeInBytes, bytesWritten);
    }

    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_writeZeroHappened = false;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
