//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


#ifndef stream_utils_hpp
#define stream_utils_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>

void ReadChunkTypeAndSize(std::fstream& fs, char* chunkType, uint32_t* chunkSize);
void GetFormatFromWavFile(std::fstream& fs);

#endif /* stream_utils_hpp */
