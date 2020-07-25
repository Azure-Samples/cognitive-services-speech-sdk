//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include "stdafx.h"

#include <factory_helpers.h>

#include "read_write_ring_buffer.h"
#include "blocking_read_write_ring_buffer.h"
#include "buffer_data.h"
#include "buffer_properties.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

SPX_EXTERN_C void* DataLib_CreateModuleObject(const char* className, const char* interfaceName)
{
    SPX_FACTORY_MAP_BEGIN();
        SPX_FACTORY_MAP_ENTRY(CSpxBlockingReadWriteRingBuffer, ISpxReadWriteBufferInit);
        SPX_FACTORY_MAP_ENTRY(CSpxBufferData, ISpxBufferData);
        SPX_FACTORY_MAP_ENTRY(CSpxBufferProperties, ISpxBufferProperties);
        SPX_FACTORY_MAP_ENTRY(CSpxReadWriteRingBuffer, ISpxReadWriteBufferInit);
    SPX_FACTORY_MAP_END();
}


} } } } // Microsoft::CognitiveServices::Speech::Impl
