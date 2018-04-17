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


template <class T>
inline std::shared_ptr<T> SpxSharedPtrFromThis(T* ptr)
{
    return ptr != nullptr
        ? ptr->shared_from_this()
        : nullptr;
}

template <class T>
inline std::shared_ptr<T> SpxAllocSharedBuffer(size_t sizeInBytes)
{
    auto ptr = reinterpret_cast<T*>(new uint8_t[sizeInBytes]);
    auto deleter = [](T* p) { delete [] reinterpret_cast<uint8_t*>(p); };

    std::shared_ptr<T> buffer(ptr, deleter);
    return buffer;
}

using SpxSharedUint8Buffer_Type = std::shared_ptr<uint8_t>;
inline SpxSharedUint8Buffer_Type SpxAllocSharedUint8Buffer(size_t sizeInBytes)
{
    return SpxAllocSharedBuffer<uint8_t>(sizeInBytes);
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
