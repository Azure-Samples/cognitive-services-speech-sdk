//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <array>
#include <string>

#include "utf8_utils.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Utils {

size_t ExtractLastUtf8Character(std::string& input, std::array<char, 4>& utf8Character)
{
    int currentIndex = (int)input.size() - 1;
    auto done = false;
    while (!done && currentIndex >= 0)
    {
        // Check if this is ascii or the first byte of a multi-byte character.
        if ((unsigned char)input.at(currentIndex) < 128 ||
            (unsigned char)input.at(currentIndex) >= 192)
        {
            done = true;
        }
        // Check if this is part of a multi-byte character.
        else if ((unsigned char)input.at(currentIndex) >= 128 && (unsigned char)input.at(currentIndex) < 192)
        {
            currentIndex--;
        }
    }

    if (currentIndex < 0)
    {
        SPX_TRACE_ERROR("Malformed UTF-8 string found.");
        return 0;
    }

    return ExtractUtf8Character(input.substr(currentIndex).c_str(), utf8Character);
}

size_t ExtractUtf8Character(const char* input, std::array<char, 4>& utf8CharacterArray)
{
    if (*input == '\0')
    {
        return 0;
    }

    size_t byteCount = 0;
    utf8CharacterArray[byteCount] = *input;
    byteCount += 1;

    if ((unsigned char)*input > 127)
    {
        byteCount = ExtractMultibyteUtf8Character(input, utf8CharacterArray);
    }
    return byteCount;
}

size_t ExtractMultibyteUtf8Character(const char* input, std::array<char, 4>& utf8CharacterArray)
{
    size_t byteCount = 2;
    utf8CharacterArray[0] = *input;
    if (*input == '\0')
    {
        return 0;
    }
    utf8CharacterArray[1] = *(input + 1);

    // If this is true we have at least 3 bytes.
    // 0b 111X XXXX
    if (((unsigned char)utf8CharacterArray[0] & (unsigned char)240) >= (unsigned char)224)
    {
        if (utf8CharacterArray[1] == '\0')
        {
            SPX_TRACE_ERROR("Malformed UTF-8 character found.");
            return 0;
        }
        utf8CharacterArray[byteCount] = *(input + byteCount);
        byteCount++;
        // If this is true we have 4 bytes.
        // 0b 1111 XXXX
        if (((unsigned char)utf8CharacterArray[0] & 240) >= 240)
        {
            if (utf8CharacterArray[2] == '\0')
            {
                SPX_TRACE_ERROR("Malformed UTF-8 character found.");
                return 0;
            }
            utf8CharacterArray[byteCount] = *(input + byteCount);
            byteCount++;
        }
    }
    return byteCount;
}

}}}}}
