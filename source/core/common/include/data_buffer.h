//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// data_buffer.h: definitions for DataBuffer C++ struct
//

#pragma once

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

    struct DataBuffer
    {
    public:
        unsigned long long timestamp;
        unsigned int size;
        unsigned char* data;
    };

}}}} // Microsoft::CognitiveServices::Speech::Impl
