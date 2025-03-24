//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#pragma once
#include <array>
#include <string>

#include "intent_interfaces.h"

using namespace Microsoft::SpeechSDK::Standalone::Intent::Impl;

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Utils {

    /// <summary>
    /// This function checks the orthography pattern punctuation and skips any UTF8 characters in it.
    /// It moves the pointer for patternLocation past the characters.
    /// </summary>
    /// <param name="patternLocation">The null terminated UTF8 char buffer.</param>
    void SkipPatternPunctuationAndWhitespace(const char*& patternLocation, const OrthographyInformation& orthography);

    /// <summary>
    /// This function checks the orthography input punctuation and skips any UTF8 characters in it.
    /// It moves the pointer for inputLocation past the characters.
    /// </summary>
    /// <param name="inputLocation">The null terminated UTF8 char buffer.</param>
    void SkipInputPunctuationAndWhitespace(const char*& inputLocation, const OrthographyInformation& orthography);

    /// <summary>
    /// This function checks the punctuation in the paramater and skips any UTF8 characters in it.
    /// It moves the pointer for startLocation past the characters.
    /// </summary>
    /// <param name="inputLocation">The null terminated UTF8 char buffer.</param>
    void SkipPunctuationAndWhitespace(const char*& startLocation, const std::string& punctuation, const OrthographyInformation& orthography);

    /// <summary>
    /// This function checks the orthography input punctuation and skips any UTF8 characters in it.
    /// It moves the pointer for inputLocation past the characters.
    /// </summary>
    /// <param name="inputLocation">The null terminated UTF8 char buffer.</param>
    /// <returns>The next word or, if there is no whitespace in the orthography, the next token.
    /// Empty string if there is no word.</returns>
    std::string GrabNextWhitespaceWord(const char** input, const OrthographyInformation& orthography);

    /// <summary>
    /// This function returns the next full UTF8 token.
    /// It moves the pointer for inputLocation past the bytes.
    /// </summary>
    /// <param name="inputLocation">The null terminated UTF8 char buffer.</param>
    /// <returns>The next word or, if there is no whitespace in the orthography, the next token.
    /// Empty string if there is no word.</returns>
    std::string GrabNextNonWhitespaceWord(const char** input);

    /// <summary>
    /// Finds the next word or, if there is no whitespace in the orthography, the next token and returns it
    /// as a string. The input pointer is advanced past the word/token.
    /// </summary>
    /// <param name="input">A pointer to a null terminated character array of utf8 tokens.</param>
    /// <returns>The next word or, if there is no whitespace in the orthography, the next token.</returns>
    std::string GrabNextWord(const char** input, const OrthographyInformation& orthography);

    /// <summary>
    /// Removes the last word/token of the string based on orthography whitespace
    /// and returns the number of characters removed.
    /// </summary>
    /// <param name="input">The string to operate on.</param>
    /// <returns></returns>
    size_t RemoveLastToken(std::string& input, const OrthographyInformation& orthography);

    /// <summary>
    /// Returns the number of non-null characters before a null character is encountered starting at the beginning of the array.
    /// </summary>
    /// <param name="inputArray">The array of characters to count.</param>
    /// <returns>The number of non-null characters before a null character is encountered.</returns>
    size_t CountNumCharacters(const std::array<char, 4>& inputArray);

    /// <summary>
    /// Trims any whitespace off the end of the input based on the orthography.Whitespace.
    /// This function considers UTF-8 whitespace.
    /// </summary>
    /// <param name="input">The input to be trimmed.</param>
    /// <returns>The number of bytes trimmed.</returns>
    size_t TrimUTF8Whitespace(std::string& input, const OrthographyInformation& orthography);

    /// <summary>
    /// Trims any sentence end characters off the end of the input based on the orthography.SentenceEndCharacters.
    /// This function considers UTF-8 characters.
    /// </summary>
    /// <param name="input">The input to be trimmed.</param>
    /// <returns>The number of bytes trimmed.</returns>
    size_t TrimUTF8SentenceEndCharacters(std::string& input, const OrthographyInformation& orthography);

    /// <summary>
    /// Checks whether the input is pointing at a word boundary UTF8 character.
    /// </summary>
    /// <returns>true if the character is a word boundary.</returns>
    bool IsWordBoundary(const char* input, const OrthographyInformation& orthography);

}}}}}
