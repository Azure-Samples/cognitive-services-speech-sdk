//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include "blocking_read_write_buffer.h"
#include "read_write_ring_buffer.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

class CSpxBlockingReadWriteRingBuffer : public CSpxBlockingReadWriteBuffer<CSpxReadWriteRingBuffer>
{
};

} } } } // Microsoft::CognitiveServices::Speech::Impl
