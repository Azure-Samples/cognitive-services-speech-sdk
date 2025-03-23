//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

#include "stdafx.h"

#include <cmath>
#include <future>
#include <memory>
#include <map>
#include <string>
#include <regex>
#include <codecvt>
#include "zh_integer_parser.h"

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

    // Must use this so the maps will compare the values && not the pointers.
    struct cmp_str
    {
        bool operator()(char const* a, char const* b) const
        {
            return std::strcmp(a, b) < 0;
        }
    };

    const std::map<const char*, int64_t, cmp_str> CardinalNumberMap
    {
        { u8"零", 0L },
        { u8"一", 1L },
        { u8"幺", 1L },
        { u8"二", 2L },
        { u8"两", 2L },
        { u8"俩", 2L },
        { u8"三", 3L },
        { u8"仨", 3L },
        { u8"四", 4L },
        { u8"五", 5L },
        { u8"六", 6L },
        { u8"七", 7L },
        { u8"八", 8L },
        { u8"九", 9L },
        { u8"十", 10L },
        { u8"百", 100L },
        { u8"千", 1000L },
        { u8"k", 1000L },
        { u8"〇", 0L },
        { u8"壹", 1L },
        { u8"贰", 2L },
        { u8"兩", 2L },
        { u8"倆", 2L },
        { u8"叁", 3L },
        { u8"肆", 4L },
        { u8"伍", 5L },
        { u8"陆", 6L },
        { u8"陸", 6L },
        { u8"柒", 7L },
        { u8"捌", 8L },
        { u8"玖", 9L },
        { u8"拾", 10L },
        { u8"佰", 100L },
        { u8"仟", 1000L },
        // some other units larger than 1k
        // do not support for now, maybe used in the future
        // { u8"万", 10000L },
        // { u8"亿", 100000000L },
        // { u8"兆", 1000000000000L },
        // { u8"万万", 100000000L },
        // { u8"万亿", 1000000000000L },
        // { u8"亿万", 1000000000000L },
        // { u8"萬", 10000L },
        // { u8"億", 100000000L },
        // { u8"萬萬", 100000000L },
        // { u8"萬億", 1000000000000L },
        // { u8"億萬", 1000000000000L },
        // { u8"m", 1000000L },
        // { u8"g", 1000000000L },
        // { u8"t", 1000000000000L },
    };

    const char* NumberPatternRegexZhSim = u8"(\\d+|\\s|零|一|幺|二|两|俩|三|仨|四|五|六|七|八|九|十|百|千|〇|壹|贰|兩|倆|叁|肆|伍|陆|陸|柒|捌|玖|拾|佰|仟|k)";

    Maybe<std::string> CSpxZHIntegerParser::Parse(const std::string& input) const
    {
        std::string stringToProcess = input;
        try
        {
            std::regex numberPatternRegexZhSim(NumberPatternRegexZhSim, std::regex_constants::icase);
            std::regex addBlankZh(u8"([\u4e00-\u9fa5]{3})");
            std::smatch sm;
            int64_t result = 0;
            bool found = false;

            // Separate into Chinese chracters && numbers.
            stringToProcess = std::regex_replace(stringToProcess, addBlankZh, u8" \\1 ", std::regex_constants::format_sed);
            stringToProcess = std::regex_replace(stringToProcess, std::regex(u8"\\s+"), u8" ");
            stringToProcess = std::regex_replace(stringToProcess, std::regex(u8"^\\s+|\\s+$"), u8"");

            auto matchBegin = std::sregex_iterator(stringToProcess.begin(), stringToProcess.end(), numberPatternRegexZhSim);
            auto matchEnd = std::sregex_iterator();
            std::vector<std::string> matches;

            for (std::sregex_iterator iter = matchBegin; iter != matchEnd; ++iter)
            {
                auto match = *iter;

                auto prefix = match.prefix().str();
                auto suffix = match.suffix().str();

                if ((!prefix.empty() && !std::regex_search(prefix, sm, numberPatternRegexZhSim))
                    || (!suffix.empty() && !std::regex_search(suffix, sm, numberPatternRegexZhSim)))
                {
                    matches.clear();
                    break;
                }

                if (match[1].str() != u8" ")
                    matches.push_back(match[1].str());
            }

            if (matches.size() != 0)
            {
                auto numVector = ConvertMatchesToNumVector(matches);
                if (numVector.size() != 0)
                {
                    auto num = ConvertToMultiplicativeAdditive(numVector);
                    if (num.size() != 0)
                    {
                        auto numNorm = NormalizeNumbers(num);
                        if (!numNorm.empty())
                        {
                            auto strResult = ConvertVectorToNum(numNorm);
                            char* endptr = NULL;
                            // errno can be set to any non-zero value by a library function call
                            // regardless of whether there was an error, so it needs to be cleared
                            // in order to check the error set by strtol
                            errno = 0;
                            auto numResult = strtoull(strResult.c_str(), &endptr, 10);
                            if (endptr != strResult.c_str() && errno == 0)
                            {
                                result = numResult;
                                found = true;
                            }
                        }
                    }
                }
            }

            return found ? std::string(std::to_string(result)) : Maybe<std::string>();;
        }
        catch (const std::regex_error& ex)
        {
            // warning gets trigger from not using ex outside of macro.
            UNUSED(ex);
            SPX_TRACE_ERROR("REGEX: Error: %s", ex.what());
            return Maybe<std::string>();
        }
    }

    std::vector<uint64_t> CSpxZHIntegerParser::ConvertMatchesToNumVector(const std::vector<std::string>& matches) const
    {
        std::vector<uint64_t> results;

        for (auto& match : matches)
        {
            auto cardinal = CardinalNumberMap.find(match.c_str());
            if (cardinal != CardinalNumberMap.end())
            {
                results.push_back(cardinal->second);
                continue;
            }

            auto pullOutDigitNum = [&](auto& regex, const auto& str)
                {
                    std::cmatch cm;
                    if (std::regex_match(str.c_str(), cm, regex))
                    {
                        // errno can be set to any non-zero value by a library function call
                        // regardless of whether there was an error, so it needs to be cleared
                        // in order to check the error set by strtol
                        errno = 0;
                        char* endptr = NULL;
                        auto digitNum = strtol(str.c_str(), &endptr, 10);
                        if (endptr != str.c_str() && errno == 0)
                        {
                            if (str[0] == '0' && str.size() > 1)
                                results.push_back(0);
                            results.push_back(digitNum);
                        }
                    }
                };

            try
            {
                std::regex digitRegex(u8"\\d+");
                pullOutDigitNum(digitRegex, match);
            }
            catch (const std::regex_error& ex)
            {
                // warning gets trigger from not using ex outside of macro.
                UNUSED(ex);
                SPX_TRACE_ERROR("REGEX: Error: %s", ex.what());
            }
        }

        return results;
    }

    std::string CSpxZHIntegerParser::ConvertVectorToNum(const std::vector<uint64_t>& nums) const
    {
        uint64_t result = 0;

        try
        {
            if (CSpxZHIntegerParser::IsContinuousNum(nums) || (nums.size() == 1 && nums[0] == 10))
            {
                for (auto& num : nums)
                    result = result * 10 + num;
            }
            else
            {
                for (auto iter = nums.begin(); iter != nums.end();)
                {
                    auto number = *iter++;
                    auto unit = *iter++;
                    result += number * unit;
                }
            }
        }
        catch (const std::regex_error& ex)
        {
            // warning gets trigger from not using ex outside of macro.
            UNUSED(ex);
            SPX_TRACE_ERROR("REGEX: Error: %s", ex.what());
            return std::string();
        }


        return std::to_string(result);
    }

    std::vector<uint64_t> CSpxZHIntegerParser::ConvertToMultiplicativeAdditive(const std::vector<uint64_t> matches) const
    {
        std::vector<uint64_t> results;

        if ((matches.size() == 1 && matches[0] <= 10) || CSpxZHIntegerParser::IsContinuousNum(matches))
            results = matches;
        else
        {
            try
            {
                bool highUnit = true;
                for (auto& match : matches)
                {
                    std::vector<uint64_t> result;

                    // if it is at highest unit, we should not push it directly.
                    // the definition of highest unit is it "does" at highest unit, or after unit 10^4, 10^8, 10^12 or 0.
                    // otherwise, if this number is less than 10 or the power of 10, then we push it directly.
                    // for example,
                    // [1000, 2, 100] should be [1, 1000, 2, 100], "1000" split into "1" && "1000";
                    // [2, 1000, 1, 100] should still be [2, 1000, 1, 100], keep "100" unchanged.
                    if (!highUnit && (match < 10 || CSpxZHIntegerParser::IsPowerOfTen(match)))
                    {
                        if (results.size() > 0 && CSpxZHIntegerParser::IsPowerOfTen(match) && !CSpxZHIntegerParser::IsPowerOfTenThousand(match) && CSpxZHIntegerParser::IsPowerOfTen(results.back()) && results.back() != 1)
                            result.push_back(1);
                        result.push_back(match);
                    }
                    else if (results.size() > 0 && CSpxZHIntegerParser::IsPowerOfTenThousand(results.back()) && results.back() != 1 && CSpxZHIntegerParser::IsPowerOfTenThousand(match) && match != 1)
                    {
                        result.push_back(match);
                    }
                    else if (results.size() == 0 && CSpxZHIntegerParser::IsPowerOfTen(match) && match != 1)
                    {
                        result.push_back(1);
                        result.push_back(match);
                    }
                    else
                    {
                        std::string numberStr = std::to_string(match);
                        uint64_t length = numberStr.length();
                        for (unsigned int i = 0; i < length; i++)
                        {
                            uint64_t digit = numberStr[i] - (uint64_t)'0';
                            uint64_t power = length - i - 1;

                            if (digit != 0)
                            {
                                if (power > 0)
                                {
                                    if (result.size() > 0 && result.back() == 0)
                                        result.pop_back();
                                    if (results.size() < 1 || (results.size() > 0 && !(match >= 10 && match < 20 && power == 1 && results.back() < 10)))
                                        result.push_back(digit);
                                    result.push_back(uint64_t(pow(10, power)));
                                }
                                else
                                    result.push_back(digit);
                            }
                            else
                                if (result.size() > 0 && result.back() != 0)
                                    result.push_back(0);
                                else
                                    result.push_back(0);
                        }

                        while (result.size() > 1 && result.back() == 0)
                            result.pop_back();
                    }
                    results.insert(std::end(results), std::begin(result), std::end(result));

                    if (result.size() == 1 && (result.back() == 0 || (result.back() != 1 && CSpxZHIntegerParser::IsPowerOfTenThousand(result.back()))))
                        highUnit = true;
                    else
                        highUnit = false;
                }
            }
            catch (const std::regex_error& ex)
            {
                // warning gets trigger from not using ex outside of macro.
                UNUSED(ex);
                SPX_TRACE_ERROR("REGEX: Error: %s", ex.what());
            }
        }
        return results;
    }

    std::vector<uint64_t> CSpxZHIntegerParser::NormalizeNumbers(std::vector<uint64_t> matches) const
    {
        std::vector<uint64_t> results = matches;
        std::wstring resultString;

        try
        {
            // if all numbers are less than 10, we serve it as continuous number, for example,
            // [3, 2, 0, 5, 8, 2], we keep it unchanged && return it directly.
            if (results.size() > 1 && !CSpxZHIntegerParser::IsContinuousNum(results))
            {
                // need to have a check here,
                // it is not legal if the occurance of 10 is larger than 1 && the distance is not larger than 2, since it brings ambiguous.
                // for example [10, 2, 10],
                // the lexical format is 十二十, both "10 20" or "12 10" are the correct one.
                int64_t count = -1;
                for (auto& result : results)
                {
                    if (result == 10)
                    {
                        if (count > 0)
                        {
                            results.clear();
                            break;
                        }
                        else
                            count = 2;
                    }
                    else
                        --count;
                }

                if (results.empty())
                    return results;

                // normalize abbreviatory numbers
                // [1, 100, 3] will be normalized to [1, 100, 3, 10]
                // [1, 100, 0, 3] will not be normalized
                uint64_t lastNum = *(results.rbegin());
                uint64_t secondLastNum = *(results.rbegin() + 1);
                if (lastNum > 0 && lastNum < 10 && secondLastNum > 10 && CSpxZHIntegerParser::IsPowerOfTen(secondLastNum))
                {
                    uint64_t unit = uint64_t(secondLastNum / 10);
                    results.push_back(unit);
                }

                // merge the adjencent units
                // [4, 10, 10000, 10000] will be normalized to [4, 1000000000]
                for (auto iter = results.begin(); iter + 1 != results.end();)
                {
                    if (*iter != 1 && *(iter + 1) != 1 && CSpxZHIntegerParser::IsPowerOfTen(*iter) && CSpxZHIntegerParser::IsPowerOfTen(*(iter + 1)))
                    {
                        *(iter + 1) *= *iter;
                        iter = results.erase(iter);
                    }
                    else
                        ++iter;
                }

                // if there are adjencent single numbers, add the unit.
                if (results.size() > 1)
                {
                    bool hasZero = false;
                    uint64_t preUnit = 10;
                    for (auto iter = results.begin(); iter != results.end(); iter++)
                    {
                        if (*iter == 0)
                            hasZero = true;
                        if (*iter != 1 && CSpxZHIntegerParser::IsPowerOfTen(*iter))
                        {
                            preUnit = *iter;
                            hasZero = false;
                        }
                        if (iter > results.begin() && *iter < 10 && *(iter - 1) > 0 && *(iter - 1) < 10)
                        {
                            if (hasZero)
                                iter = results.insert(iter, 1);
                            else
                                iter = results.insert(iter, uint64_t(preUnit / 10));
                            iter++;
                        }
                    }
                }

                // remove all irrelevant 0 elements
                // [1, 100, 0, 3] will be normalized to [1, 100, 3]
                for (auto iter = results.begin(); iter != results.end();)
                {
                    if (*iter == 0 && iter > results.begin())
                    {
                        if (*(iter - 1) > 10 && CSpxZHIntegerParser::IsPowerOfTen(*(iter - 1)))
                            iter = results.erase(iter);
                        else if (*(iter - 1) < 9)
                        {
                            if (*(iter - 1) == 1 && iter > results.begin() + 1 && *(iter - 2) < 9) // (iter-1) is a unit
                            {
                                iter++;
                                if (CSpxZHIntegerParser::IsPowerOfTen(*(iter - 2)) && *(iter - 2) != 1)
                                    iter = results.insert(iter, 10);
                                else
                                    iter = results.insert(iter, 1);
                                iter++;
                            }
                            else // (iter-1) is not a unit
                            {
                                *iter = 10;
                                ++iter;
                            }
                        }
                        else // (iter-1) is a unit
                        {
                            iter++;
                            if (CSpxZHIntegerParser::IsPowerOfTen(*(iter - 2)) && *(iter - 2) != 1)
                                iter = results.insert(iter, 10);
                            else
                                iter = results.insert(iter, 1);
                            iter++;
                        }
                    }
                    else
                    {
                        ++iter;
                    }
                }

                if (results.empty())
                    return results;

                // check whether the numbers are legal after nomalization
                // it should follow the rules that,
                // 1. the size of the vector is even
                // 2. the even unit should be the number ranges 0-9, the odd unit should be the power of 10
                if (results.size() % 2 == 1)
                    results.push_back(1);
                bool hasErr = false;
                for (auto iter = results.begin(); iter != results.end();)
                {
                    if (*iter > 9)
                    {
                        hasErr = true;
                        break;
                    }
                    iter++;

                    if (!CSpxZHIntegerParser::IsPowerOfTen(*iter))
                    {
                        hasErr = true;
                        break;
                    }
                    iter++;
                }
                if (hasErr)
                    results.clear();

                if (results.empty())
                    return results;

                // update all locations unit
                // [2, 10, 4, 10000] will be normalized to [2, 100000, 4, 10000]
                auto resultsCopy = results;
                auto iterCopy = resultsCopy.begin();

                for (auto iterStart = results.begin(); iterStart != results.end();)
                {
                    uint64_t maxUnit = 0;
                    iterCopy = resultsCopy.begin() + (iterStart - results.begin());
                    for (auto iter = iterStart; iter != results.end() - 1;)
                    {
                        if (iter == iterStart)
                        {
                            iter++;
                            iterCopy++;
                        }
                        else
                        {
                            iter += 2;
                            iterCopy += 2;
                        }
                        if (CSpxZHIntegerParser::IsPowerOfTen(*iter) && *iter >= maxUnit)
                        {
                            maxUnit = *iter;
                            if ((iter - iterStart) > 1)
                            {
                                for (auto unitIndex = iter; unitIndex != results.begin() + 1;)
                                {
                                    unitIndex -= 2;
                                    if (CSpxZHIntegerParser::IsPowerOfTen(*unitIndex))
                                    {
                                        *unitIndex *= *iterCopy;
                                        if ((!CSpxZHIntegerParser::IsPowerOfTenThousand(*iterCopy) || *iterCopy == 1) && !(*iterCopy == 10 && *(iterCopy - 2) == 10))
                                            *unitIndex *= 10;
                                    }
                                }
                            }
                        }
                    }
                    iterStart += 2;
                }
            }
        }
        catch (const std::regex_error& ex)
        {
            // warning gets trigger from not using ex outside of macro.
            UNUSED(ex);
            SPX_TRACE_ERROR("REGEX: Error: %s", ex.what());
        }

        return results;
    }

    bool CSpxZHIntegerParser::IsPowerOfTen(const uint64_t inputNumber) const
    {
        if (!inputNumber)
            return false;
        auto number = inputNumber;
        if (number == 0)
            return false;
        while (number % 10 == 0)
            number /= 10;
        return number == 1;
    }

    bool CSpxZHIntegerParser::IsPowerOfTenThousand(const uint64_t inputNumber) const
    {
        if (!inputNumber)
            return false;
        auto number = inputNumber;
        if (number == 0)
            return false;
        while (number % 10000 == 0)
            number /= 10000;
        return number == 1;
    }

    bool CSpxZHIntegerParser::IsContinuousNum(const std::vector<uint64_t> matches) const
    {
        if (matches.empty())
            return true;
        for (auto& match : matches)
            if (match > 9)
                return false;
        return true;
    }

}}}}}
