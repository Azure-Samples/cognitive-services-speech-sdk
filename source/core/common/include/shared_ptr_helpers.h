//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// shared_ptr_helpers.h: Implementation declarations/definitions for shared pointer helper methods
//

#pragma once
#include "spxcore_common.h"


namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

namespace Details
{
    template<typename T>
    void BufferDeleter(T* p)
    {
        delete[] reinterpret_cast<uint8_t*>(p);
    }
}

template <class T>
std::shared_ptr<T> SpxSharedPtrFromThis(T* ptr)
{
    return ptr != nullptr
        ? ptr->shared_from_this()
        : nullptr;
}

template <class T>
std::shared_ptr<T> SpxAllocSharedBuffer(size_t sizeInBytes)
{
    auto ptr = reinterpret_cast<T*>(new uint8_t[sizeInBytes]);

    std::shared_ptr<T> buffer(ptr, Details::BufferDeleter<T>);
    return buffer;
}

template<typename T>
using buffer_unique_ptr_t = std::unique_ptr<T, decltype(&Details::BufferDeleter<T>)>;

template<typename T>
buffer_unique_ptr_t<T> SpxAllocUniqueBuffer(size_t sizeInBytes)
{
    auto ptr = reinterpret_cast<T*>(new uint8_t[sizeInBytes]);
    return buffer_unique_ptr_t<T>(ptr, &Details::BufferDeleter<T>);
}

using SpxSharedUint8Buffer_Type = std::shared_ptr<uint8_t>;
inline SpxSharedUint8Buffer_Type SpxAllocSharedUint8Buffer(size_t sizeInBytes)
{
    return SpxAllocSharedBuffer<uint8_t>(sizeInBytes);
}

//
// Replace with reinterpret_pointer_cast when we move every compiler to C++17
//
template <class _Ty1, class _Ty2>
std::shared_ptr<_Ty1> SpxReinterpretPointerCast(const std::shared_ptr<_Ty2>& _Other) noexcept
{
    // reinterpret_cast for shared_ptr that properly respects the reference count control block
    const auto _Ptr = reinterpret_cast<typename std::shared_ptr<_Ty1>::element_type*>(_Other.get());
    return std::shared_ptr<_Ty1>(_Other, _Ptr);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
