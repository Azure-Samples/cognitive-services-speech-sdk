//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// condition_variable_buffer.h: Implementation definitions for ConditionalVariableBuffer C++ class
//

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace RNNT {

template <typename T>
struct Buffer_traits;

// traits to define behaviors required by ConditionalVariableBuffer
// to manage a buffer type.
template <typename T>
struct Buffer_traits<std::queue<T>>
{
    using data_type = std::queue<T>;

    static void Clear(data_type& v)
    {
        if (!v.empty())
        {
            data_type empty;
            v.swap(empty);
        }
    }
};

// A buffer controlled by a conditional variable with eos state.
// Producer uses Write to add data into buffer and notify consumers.
// Buffer consumer uses Read to get data from buffer.
//     T: type of the data buffer
//     Traits: buffer traits, default to Buffer_traits
template<typename T, typename Traits = Buffer_traits<T>>
class ConditionalVariableBuffer
{
public:
    using _traits_type = Traits;
    using _buffer_type = T;

    ConditionalVariableBuffer() noexcept : m_end(false), m_data(T{}) {};

    // Reset buffer to initial state and clear the internal buffer
    void Reset()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_end = false;
        _traits_type::Clear(m_data);
    }

    // Reset buffer to initial state and clear the internal buffer
    // This is performed without lock so it can only be called when no reader is waiting.
    void ResetNoLock()
    {
        m_end = false;
        _traits_type::Clear(m_data);
    }

    // Set EndOfStream on the buffer and send notification under lock.
    void SetEndOfStream()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_end = true;
        }
        m_dataCV.notify_all();
    };

    // Check EndOfStream
    bool IsEndOfStream()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_end;
    }

    // Perform read by invoking a caller provided routine, similar to conditional variable wait(callback).
    // Mutex locked around the routine.  It will block if stream is not ended and more data is pending.
    // Parameters:
    //    pred: a caller provided callback to read and remove data from the buffer passed in.
    // Return:
    //    true if data is read.
    //    false if stream is ended.
    // Caller should provide a callable as input with signature of
    //    bool Callable(_buffer_type)
    //    The callback should read and remove data from the buffer of type _buffer_type passed in as parameter.
    //    Return true indicates condition is met so CV doesn't need to wait,
    //    Return false to let CV wait
    template<typename _Callback>
    bool Read(_Callback&& pred)
    {
        // TODO: exception handling
        std::unique_lock<std::mutex> lock(m_mutex);
        bool haveData = false;
        m_dataCV.wait(lock, [&, this]() {
            haveData = pred(m_data);
            return haveData || m_end;
            });
        return haveData;
    }

    // Perform write by invoking a caller provided routine.
    // Mutex locked around the routine. CV notification will be sent to consumers on data written.
    // Parameters:
    //    pred: a caller provided callback to write data into the buffer passed in.
    // Return:
    //    true if new data is written.
    // Caller should provide a callable as input with signature of
    //     bool Callable(_buffer_type)
    //     The callback should add data into the buffer of type _buffer_type passed in as parameter.
    //     return true indicates changes are made to buffer so CV will notify.
    template<typename _Callback>
    bool Write(_Callback&& pred)
    {
        bool ret = false;
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_end)
            {
                ret = pred(m_data);
            }
        }
        if (ret)
        {
            m_dataCV.notify_all();
        }
        return ret;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_dataCV;
    bool m_end;
    T m_data;
};

}}}}
