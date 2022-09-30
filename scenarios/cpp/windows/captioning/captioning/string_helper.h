//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <algorithm>
#include <string>
#include <vector>

class StringHelper
{
public:
    static bool CaseInsensitiveCompare(const std::string& a, const std::string& b)
    {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char a, char b) {
            return tolower(a) == tolower(b);
        });
    }

    static bool EndsWith(const std::string& str, const std::string& suffix)
    {
        return (str.size() >= suffix.size()) && (0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix));
    }

    static bool IsUUID(const std::string &str)
    {
        if (str.length() != 36)
        {
            return false;
        }
        for (int i = 0; i < str.length(); i++)
        {
            if (i == 8 || i == 13 || i == 18 || i == 23)
            {
                if ('-' != str[i])
                {
                    return false;
                }
                else
                {
                    continue;
                }
            }
            else if (!isxdigit(str[i]))
            {
                return false;
            }
        }
        return true;
    }

    static std::string Join(const std::vector<std::string>& xs, const std::string& delimiter)
    {
        std::string retval;

        for (std::vector<std::string>::const_iterator i = xs.begin(); i != xs.end(); ++i)
        {
            retval += *i;
            if (i != xs.end() - 1)
            {
                retval += delimiter;
            }
        }

        return retval;
    }

    static std::string LeftTrim(std::string str)
    {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); }));
        return str;
    }

    static std::string RightTrim(std::string str)
    {
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), str.end());
        return str;
    }

    static std::vector<std::string> Split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while(std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

    static std::string ToLower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::tolower(c); });
        return str;
    }

    static std::string Trim(std::string str)
    {
        return LeftTrim(RightTrim(str));
    }
};
