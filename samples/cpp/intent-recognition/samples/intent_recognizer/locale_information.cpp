//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include <array>
#include <vector>
#include "locale_information.h"
#include "utf8_utils.h"
#include "string_utils.h"
#include "substrings_matcher.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {
namespace Locales {

    static const std::array<const OrthographyInformation, 6> ORTHOGRAPHY_INFORMATION =
    {
        // English (the default one to use) should be first
        OrthographyInformation{
            "en",
            "\t\r\n ", // Whitespace
            "[]{}()^_=`~<>‑—‒⁻₋−➖﹣－;:!?…‘’\"“”§@*/\\&#†‡′″£₤￡$﹩＄€؉‰%٪﹪％'ʼ՚᾽᾿’＇:∶︓﹕：.․。︒﹒．｡¥￥₩￦₨₹,،٫⹁、︐︑﹐﹑，､+⁺₊➕﬩﹢＋", // InputPunctuation
            "<>^_=`~‑—‒⁻₋−➖﹣－;:!?…‘’\"“”§@*/\\&#†‡′″£₤￡$﹩＄€؉‰%٪﹪％'ʼ՚᾽᾿’＇:∶︓﹕：.․。︒﹒．｡¥￥₩￦₨₹,،٫⹁、︐︑﹐﹑，､+⁺₊➕﬩﹢＋", // PatternPunctuation
            "!.?", // SentenceEndCharacters
            { ' ' }, // WordBoundary
            false // RightToLeft
        },
        OrthographyInformation{
            "zh",
            "", // Whitespace
            "[]{}()<>=。，、：；？！「」『』·《》〈〉…—～﹁﹂\"'", // InputPunctuation
            "<>=。，、：；？！「」『』·《》〈〉…—～﹁﹂\"'", // PatternPunctuation
            "。？！?", // SentenceEndCharacters
            {}, // WordBoundary
            false // RightToLeft
        },
        OrthographyInformation{
            "de",
            "\t\r\n ", // Whitespace
            "<>[]{}()<>\\‐‑–—;!?…‘‚\"“„«»§@*/&#:∶。︒'%٪﹪％؉‰-‒⁻₋−➖﹣－,،٫、︑﹑､+⁺₊➕﬩﹢＋︐﹐，.․﹒．｡£₤￡$﹩＄€¥￥₩￦₨₹^_=ʼ՚ߴ᾽᾿’＇", // InputPunctuation
            "<>\\‐‑–—;!?…‘‚\"“„«»§@*/&#:∶。︒'%٪﹪％؉‰-‒⁻₋−➖﹣－,،٫、︑﹑､+⁺₊➕﬩﹢＋︐﹐，.․﹒．｡£₤￡$﹩＄€¥￥₩￦₨₹^_=ʼ՚ߴ᾽᾿’＇", // PatternPunctuation
            "!.?", // SentenceEndCharacters
            { ' ' }, // WordBoundary
            false // RightToLeft
        },
        OrthographyInformation{
            "es",
            "\t\r\n ", // Whitespace
            "`~¡!@#$%^&*()_+=<>/¿?\\[]{},.;:'\"", // InputPunctuation
            "`~¡!@#$%^&*_+=<>/¿?\\,.;'\"", // PatternPunctuation
            "!.?", // SentenceEndCharacters
            { ' ' }, // WordBoundary
            false // RightToLeft
        },
        OrthographyInformation{
            "fr",
            "\t\r\n ", // Whitespace
            "[]{}()=<>-‐‑–—,;\\:!?.…’\"“”«»§@ * / &#†‡․。︒﹒．｡'%٪﹪％؉‰$﹩＄£₤¥￥₩￦₨₹‒⁻₋−➖﹣－،٫、︐︑﹐﹑，､+⁺₊➕﬩﹢＋∶ʼ՚ߴ᾽᾿＇", // InputPunctuation
            "=<>-‐‑–—,;\\:!?.…’\"“”«»§@ * / &#†‡․。︒﹒．｡'%٪﹪％؉‰$﹩＄£₤¥￥₩￦₨₹‒⁻₋−➖﹣－،٫、︐︑﹐﹑，､+⁺₊➕﬩﹢＋∶ʼ՚ߴ᾽᾿＇", // PatternPunctuation
            "!.?", // SentenceEndCharacters
            { ' ' }, // WordBoundary
            false // RightToLeft
        },
        OrthographyInformation{
            "ja",
            "", // Whitespace
            "()[]{}=<>‾_＿\\-－‐‑—―〜・･,，、､;；:：!！?？.．‥…。｡＇‘’\"＂“”（）［］｛｝〈〉《》「｢」｣『』【】〔〕‖§¶@＠ * ＊ / ／＼ & ＆#＃†‡′″〃※∶․︒﹒'ʼ՚᾽᾿%٪﹪％؉‰$﹩＄£₤¥￥₩￦₨₹‒⁻₋−➖﹣،٫︐︑﹑+⁺₊➕﬩﹢＋﹐", // InputPunctuation
            "=<>‾_＿\\-－‐‑—―〜・･,，、､;；:：!！?？.．‥…。｡＇‘’\"＂“”（）［］｛｝〈〉《》「｢」｣『』【】〔〕‖§¶@＠ * ＊ / ／＼ & ＆#＃†‡′″〃※∶․︒﹒'ʼ՚᾽᾿%٪﹪％؉‰$﹩＄£₤¥￥₩￦₨₹‒⁻₋−➖﹣،٫︐︑﹑+⁺₊➕﬩﹢＋﹐", // PatternPunctuation
            "！。？?", // SentenceEndCharacters
            { }, // WordBoundary
            false // RightToLeft
        }
    };

    const OrthographyInformation& default_orthography()
    {
        return ORTHOGRAPHY_INFORMATION[0];
    }

    const OrthographyInformation* find_orthography(const std::string& name)
    {
        auto search = std::find_if(ORTHOGRAPHY_INFORMATION.begin(), ORTHOGRAPHY_INFORMATION.end(),
            [name](const OrthographyInformation& entry)
            {
                return PAL::stricmp(name.c_str(), entry.Name.c_str()) == 0;
            });

        return search != ORTHOGRAPHY_INFORMATION.end()
            ? &(*search)
            : nullptr;
    }

    // There are 3 spaces here. Space, non-breaking space, and narrow non-breaking space.
    static const SubstringsMatcher& Spaces{ { " ", " ", " " } };

    static const SubstringsMatcher& Punctuation{
    {
        "!", "?", ";",
        ":", "∶", "%",
        "٪", "﹪", "％",
        "؉", "‰", "$",
        "﹩", "＄", "£",
        "₤", "¥", "￥",
        "₩", "￦", "₨",
        "₹", "#", "«",
        "»"
    } };

    std::string Utils::RemoveLeadingPunctuationSpaceFR(const std::string& input)
    {
        std::string output(input);
        size_t inputOffset = 0;
        size_t outputDelta = 0;

        while (inputOffset < input.length())
        {
            // looking for space
            std::string spaceChars;
            size_t spaceIndex = Spaces.Find(input, inputOffset, &spaceChars);
            if (spaceIndex != SubstringsMatcher::NO_MATCH)
            {
                // is this followed by punctuation?
                std::string punctChars;
                size_t punctIndex = Punctuation.Find(input, spaceIndex + spaceChars.length(), Punctuation.MaxMatchLen(), &punctChars);
                if (punctIndex != SubstringsMatcher::NO_MATCH)
                {
                    // remove the space preceding the punctuation
                    output.erase(spaceIndex - outputDelta, spaceChars.length());
                    outputDelta += spaceChars.length();

                    // we've already evaluated the punctuation characters so we can skip over those
                    inputOffset = punctIndex + punctChars.length();
                }
                else
                {
                    // we've already evaluated the space characters so we can skip over those
                    inputOffset = spaceIndex + spaceChars.length();
                }
            }
            else
            {
                // no more spaces detected so we are done searching
                break;
            }
        }

        return output;
    }

    std::string Utils::AddLeadingPunctuationSpaceFR(const std::string& input)
    {
        if (input.length() == 0)
        {
            return input;
        }

        std::vector<char> buffer;
        buffer.reserve(input.length() + 10);

        size_t inputIndex = 0;
        std::string punctuation;
        while (inputIndex < input.length())
        {
            size_t nextPunct = Punctuation.Find(input, inputIndex, &punctuation);
            if (nextPunct == SubstringsMatcher::NO_MATCH)
            {
                // add rest of string
                buffer.insert(buffer.end(), input.begin() + inputIndex, input.end());
                break;
            }
            else
            {
                // add what was before this
                buffer.insert(buffer.end(), input.begin() + inputIndex, input.begin() + nextPunct);

                // add space and then the punctuation
                buffer.push_back(' ');
                buffer.insert(buffer.end(), punctuation.begin(), punctuation.end());
                inputIndex = nextPunct + punctuation.length();
            }
        }

        return std::string(buffer.data(), buffer.size());
    }

}}}}}}
