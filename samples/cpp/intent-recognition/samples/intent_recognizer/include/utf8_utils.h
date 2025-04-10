//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once
#include <array>
#include <string>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Utils {

    /// <summary>
    /// This function takes in a char pointer pointing to the first byte of a UTF-8 character.
    /// The utf8CharacterArray needs to be at least 4 char large in order to store the largest UTF8
    /// character. If the input is malformed, an exception will be thrown.
    /// </summary>
    /// <param name="input">This should point to a UTF8 encoded multi-byte character or string. </param>
    /// <param name="utf8CharacterBuffer">The array to place the multi-byte character into.
    /// This does not need to include a null terminator.</param>
    /// <returns> The number of bytes making up the character.</returns>
    size_t ExtractUtf8Character(const char* input, std::array<char, 4>& utf8CharacterArray);

    /// <summary>
    /// This function takes in a string containing UTF8 character bytes. This fuction will return
    /// the last full UTF8 character.
    /// The utf8CharacterArray needs to be at least 4 char large in order to store the largest UTF8
    /// character. If the input is malformed, an exception will be thrown.
    /// </summary>
    /// <param name="input">This should point to a UTF8 encoded multi-byte character or string. </param>
    /// <param name="utf8CharacterBuffer">The array to place the multi-byte character into.
    /// This does not need to include a null terminator.</param>
    /// <returns> The number of bytes making up the character.</returns>
    size_t ExtractLastUtf8Character(std::string& input, std::array<char, 4>& utf8Character);

    /// <summary>
    /// This function takes in a char pointer pointing to the first byte of a multi-byte UTF-8 character.
    /// The multiByteCharacterBuffer should be at least 4 char large in order to store the largest UTF8
    /// character. If the input is malformed, an exception will be thrown.
    /// </summary>
    /// <param name="input">This should point to a UTF8 encoded multi-byte character.
    /// This does not need to include a null terminator.</param>
    /// <param name="utf8CharacterArray">The array to place the multi-byte character into.</param>
    /// <returns> The number of bytes making up the character.</returns>
    size_t ExtractMultibyteUtf8Character(const char* input, std::array<char, 4>& utf8CharacterArray);

}}}}}
