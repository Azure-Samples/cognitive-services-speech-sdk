//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <mutex>
#include <type_traits>

#include <spxcore_common.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

template <class T, std::enable_if_t<std::is_base_of<ISpxReadWriteBuffer, T>::value, int> = 0>
class CSpxBlockingReadWriteBuffer : public T
{
public:
    /// <summary>
    /// ISpxReadWriteBuffer::Read (override) - DataAccess method that will attempt perform a BlockingRead if enough
    /// data isn't yet available, or simply delegate to the base class immediately if enough data is already available.
    /// </summary>
    void Read(void* data, size_t dataSizeInBytes, size_t* bytesRead = nullptr) final
    {
        auto available = T::GetBytesReadReady();
        available >= dataSizeInBytes ? BaseRead(data, dataSizeInBytes, bytesRead) : BlockingRead(data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// ISpxReadWriteBuffer::ReadAtBytePos (override) - DataAccess method that will attempt perform a BlockingRead if enough
    /// data isn't yet available, or simply delegate to the base class immediately if enough data is already available.
    /// </summary>
    void ReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead) final
    {
        auto available = T::GetBytesReadReadyAtPos(pos);
        available >= dataSizeInBytes ? BaseReadAtBytePos(pos, data, dataSizeInBytes, bytesRead) : BlockingReadAtBytePos(pos, data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// ISpxReadWriteBuffer::ReadShared (override) - DataAccess method that will attempt perform a BlockingReadShared if enough
    /// data isn't yet available, or simply delegate to the base class immediately if enough data is already available.
    /// </summary>
    std::shared_ptr<uint8_t> ReadShared(size_t dataSizeInBytes, size_t* bytesRead = nullptr) final
    {
        auto available = T::GetBytesReadReady();
        return available >= dataSizeInBytes ? BaseReadShared(dataSizeInBytes, bytesRead) : BlockingReadShared(dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// ISpxReadWriteBuffer::Write (override) - DataStorage method that will delegate to the base class immediately, after first checking
    /// to see if the UnblockTrigger criteria has been met (Write(nullptr, 0)); this method MUST notify all BlockingReads in that condition.
    /// </summary>
    void Write(const void* data, size_t dataSizeInBytes, size_t* bytesWritten = nullptr) final
    {
        m_writeZeroHappened = (data == nullptr && dataSizeInBytes == 0);
        BaseWrite(data, dataSizeInBytes, bytesWritten);
        m_cv.notify_all();
    }

protected:
    /// <summary>
    /// Wait until either the amount of data specified
    /// is available, or someone signals the UnblockTrigger.
    /// </summary>
    uint64_t WaitUntilBytesAvailable(uint64_t bytesRequired)
    {
        return WaitUntilBytesAvailable(bytesRequired,
            [this]() -> uint64_t
            {
                return T::GetBytesReadReady();
            });
    }

    /// <summary>
    /// Wait until either the amount of data specified
    /// is available at the given position, or someone signals the UnblockTrigger.
    /// </summary>
    uint64_t WaitUntilBytesAvailableAtPos(uint64_t pos, uint64_t bytesRequired)
    {
        return WaitUntilBytesAvailable(bytesRequired,
            [pos, this]() -> uint64_t
            {
                return T::GetBytesReadReadyAtPos(pos);
            });
    }

    /// <summary>
    /// Wait until either the amount of data specified with the lambda provided to do the check.
    /// is available at the given position, or someone signals the UnblockTrigger.
    /// </summary>
    template<typename F>
    uint64_t WaitUntilBytesAvailable(uint64_t bytesRequired, F checkDataAvailableFunc)
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
        T::Read(data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BaseReadAtBytePos - Simply delegate to the base class's implementation of ISpxReadWriteBuffer::ReadAtBytePos().
    /// </summary>
    void BaseReadAtBytePos(uint64_t pos, void* data, size_t dataSizeInBytes, size_t* bytesRead)
    {
        T::ReadAtBytePos(pos, data, dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BaseReadShared - Simply delegate to the base class's implementation of ISpxReadWriteBuffer::ReadShared().
    /// </summary>
    std::shared_ptr<uint8_t> BaseReadShared(size_t dataSizeInBytes, size_t* bytesRead)
    {
        return T::ReadShared(dataSizeInBytes, bytesRead);
    }

    /// <summary>
    /// CSpxBlockingReadWriteBuffer::BaseWrite - Simply delegate to the base class's implementation of ISpxReadWriteBuffer::Write().
    /// </summary>
    void BaseWrite(const void* data, size_t dataSizeInBytes, size_t* bytesWritten)
    {
        T::Write(data, dataSizeInBytes, bytesWritten);
    }

    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_writeZeroHappened = false;
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
