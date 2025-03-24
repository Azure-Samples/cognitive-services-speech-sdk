//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <array>
#include <string>

#include "pattern_matching_utils.h"
#include "utf8_utils.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Utils {

size_t TrimUTF8Whitespace(std::string& input, const OrthographyInformation& orthography)
{
    size_t bytesRemoved = 0;
    std::array<char, 4> utf8Character = { 0 };
    auto done = false;
    if (!orthography.Whitespace.empty())
    {
        while (!done)
        {
            auto characterSize = Utils::ExtractLastUtf8Character(input, utf8Character);
            if (characterSize == 0)
            {
                return 0;
            }
            // If this utf8 character is in our locale whitespace, remove it and try again.
            if (orthography.Whitespace.find(utf8Character.data(), 0, characterSize) != std::string::npos)
            {
                input.erase(input.size() - characterSize, characterSize);
                bytesRemoved += characterSize;
            }
            else
            {
                done = true;
            }
        }
    }
    return bytesRemoved;
}

size_t TrimUTF8SentenceEndCharacters(std::string& input, const OrthographyInformation& orthography)
{
    size_t bytesRemoved = 0;
    std::array<char, 4> utf8Character = { 0 };
    auto done = false;
    if (!orthography.SentenceEndCharacters.empty())
    {
        while (!done)
        {
            auto characterSize = Utils::ExtractLastUtf8Character(input, utf8Character);
            if (characterSize == 0)
            {
                return 0;
            }
            // If this utf8 character is in our locale SentenceEndCharacters, remove it and try again.
            if (orthography.SentenceEndCharacters.find(utf8Character.data(), 0, characterSize) != std::string::npos)
            {
                input.erase(input.size() - characterSize, characterSize);
                bytesRemoved += characterSize;
            }
            else
            {
                done = true;
            }
        }
    }
    return bytesRemoved;
}

bool IsWordBoundary(const char* input, const OrthographyInformation& orthography)
{
    std::array<char, 4> utf8Character = { 0 };
    auto bytes = Utils::ExtractUtf8Character(input, utf8Character);
    if (bytes == 0)
    {
        return false;
    }
    if (orthography.WordBoundary == utf8Character)
    {
        return true;
    }
    return false;
}

size_t CountNumCharacters(const std::array<char, 4>& inputArray)
{
    size_t result = 0;
    for (size_t i = 0; i < inputArray.size(); i++)
    {
        if (inputArray[i] != '\0')
        {
            result++;
        }
        else
        {
            break;
        }
    }
    return result;
}

size_t RemoveLastToken(std::string& input, const OrthographyInformation& orthography)
{
    size_t charactersRemoved = 0;

    if (orthography.Whitespace.empty())
    {
        // Find out how many bytes in the last UTF8 character and remove that many.
        auto done = false;
        while (!done)
        {
            // Check if this is ascii or the first byte of a multi-byte character.
            if ((unsigned char)input.back() < 128 ||
                (unsigned char)input.back() > 192)
            {
                input.pop_back();
                charactersRemoved++;
                done = true;
            }
            // Check if this is part of a multi-byte character.
            else if ((unsigned char)input.back() >= 128 && (unsigned char)input.back() <= 192)
            {
                input.pop_back();
                charactersRemoved++;
            }
        }
    }
    else
    {
        // npos is passed as the second parameter to say "search the whole string".
        auto lastOf = input.rfind(orthography.WordBoundary.data(), std::string::npos, CountNumCharacters(orthography.WordBoundary));
        charactersRemoved = input.length() - lastOf;
        input.erase(lastOf, charactersRemoved);
    }

    return charactersRemoved;
}

void SkipPatternPunctuationAndWhitespace(const char*& patternLocation, const OrthographyInformation& orthography)
{
    SkipPunctuationAndWhitespace(patternLocation, orthography.PatternPunctuation, orthography);
}

void SkipInputPunctuationAndWhitespace(const char*& inputLocation, const OrthographyInformation& orthography)
{
    SkipPunctuationAndWhitespace(inputLocation, orthography.InputPunctuation, orthography);
}

void SkipPunctuationAndWhitespace(const char*& startLocation, const std::string& punctuation, const OrthographyInformation& orthography)
{
    while (startLocation != nullptr && *startLocation != '\0')
    {
        // Check for multi-byte characters. For multi-byte characters we need to compare at least 2
        // bytes in the locale punctuation list.
        if ((unsigned char)*startLocation > 127)
        {
            std::array<char, 4> multiByteCharacterArray = { 0,0,0,0 };
            auto byteCount = Utils::ExtractMultibyteUtf8Character(startLocation, multiByteCharacterArray);

            if (punctuation.find(multiByteCharacterArray.data(), 0, byteCount) != std::string::npos ||
                orthography.Whitespace.find(multiByteCharacterArray.data(), 0, byteCount) != std::string::npos)
            {
                startLocation += byteCount;
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            // Check if we should skip something in the patternInput.
            if (punctuation.find(*startLocation) != std::string::npos ||
                orthography.Whitespace.find(*startLocation) != std::string::npos)
            {
                startLocation++;
                continue;
            }
            else
            {
                break;
            }
        }
    }
}

std::string GrabNextWhitespaceWord(const char** input, const OrthographyInformation& orthography)
{
    std::string nextWord = "";

    std::array<char, 4> utf8Character = { 0,0,0,0 };
    auto bytes = Utils::ExtractUtf8Character(*input, utf8Character);
    if (bytes == 0 || utf8Character[0] == '\0')
    {
        return "";
    }

    // Grab everything until we see whitespace again.
    while (orthography.Whitespace.find(utf8Character.data(), 0, bytes) == std::string::npos)
    {
        nextWord.append(utf8Character.data(), bytes);
        *input += bytes;
        if (**input == '\0')
        {
            break;
        }
        bytes = Utils::ExtractUtf8Character(*input, utf8Character);
        if (bytes == 0)
        {
            return "";
        }
    }
    return nextWord;
}

std::string GrabNextNonWhitespaceWord(const char** input)
{
    std::array<char, 4> utf8Character = { 0,0,0,0 };
    auto bytes = Utils::ExtractUtf8Character(*input, utf8Character);
    if (bytes == 0)
    {
        return "";
    }
    if (utf8Character[0] != '\0')
    {
        *input += bytes;
        return std::string(utf8Character.data(), bytes);
    }
    return "";
}

std::string GrabNextWord(const char** input, const OrthographyInformation& orthography)
{
    SkipInputPunctuationAndWhitespace(*input, orthography);

    if (orthography.Whitespace.empty())
    {
        return GrabNextNonWhitespaceWord(input);
    }
    else
    {
        return GrabNextWhitespaceWord(input, orthography);
    }
}

}}}}}
