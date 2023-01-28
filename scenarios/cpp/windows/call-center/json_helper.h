//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
// You can download json.hpp from:
// https://github.com/nlohmann/json/releases
#include "json.hpp"

class JsonHelper
{
public:

    // Preconditions:
    // *json* is an array.
    // *json* has a size of at least 1.
    static nlohmann::json Chunk(nlohmann::json json, size_t size)
    {
        std::vector<nlohmann::json> chunk;
        std::vector<std::vector<nlohmann::json>> retval;
        int counter = 0;
        
        if (!json.is_array())
        {
            throw std::exception(std::string("Chunk: json argument is not an array. Argument:\n" + json.dump(4)).c_str());
        }
        if (size < 1)
        {
            throw std::exception("Chunk: size argument must be at least 1.");
        }
        
        for (auto& item : json)
        {
            chunk.push_back(item);
            if (size == ++counter)
            {
                retval.push_back(chunk);
                counter = 0;
                chunk.clear();
            }
        }
        if (chunk.size() > 0)
        {
            retval.push_back(chunk);
        }
        return nlohmann::json(retval);
    }

    // Preconditions:
    // *outerJson* is an array.
    // Each element in *outerJson* is an array.
    static nlohmann::json Concat(nlohmann::json outerJson)
    {
        std::vector<nlohmann::json> retval;
        if (!outerJson.is_array())
        {
            throw std::exception(std::string("Concat: outerJson argument is not an array. Argument:\n" + outerJson.dump(4)).c_str());
        }
        else
        {
            for (auto& innerJson : outerJson)
            {
                if (!innerJson.is_array())
                {
                    throw std::exception(std::string("Concat: An item in outerJson argument is not an array. Item:\n" + innerJson.dump(4)).c_str());
                }
                else
                {
                    for (auto& item : innerJson)
                    {
                        retval.push_back(item);
                    }
                }
            }
            return nlohmann::json(retval);
        }
    }

    // Preconditions:
    // *items* is an array.
    template<typename Function>
    static nlohmann::json Fold(Function f, nlohmann::json acc, nlohmann::json items)
    {
        if (!items.is_array())
        {
            throw std::exception(std::string("Fold: items argument is not an array. Argument:\n" + items.dump(4)).c_str());
        }
        else
        {
            for (auto& item : items)
            {
                acc = f(acc, item);
            }
            return acc;
        }
    }

    // Preconditions:
    // *json* is an array.
    // Note: Templated classes/functions must be declared in header file, not source file. See:
    // https://stackoverflow.com/a/456716
    template<typename Function>
    static nlohmann::json Map(Function f, nlohmann::json json)
    {
        if (!json.is_array())
        {
            throw std::exception(std::string("Map: json argument is not an array. Argument:\n" + json.dump(4)).c_str());
        }
        else
        {
            std::vector<nlohmann::json> source = VectorFromJson(json);
            std::vector<nlohmann::json> retval;
            retval.resize(source.size());
            std::transform(source.begin(), source.end(), retval.begin(), f);
            return nlohmann::json(retval);
        }
    }

    // Preconditions:
    // *json* is an array.
    template<typename Function>
    static nlohmann::json SortBy(Function f, nlohmann::json json)
    {
        if (!json.is_array())
        {
            throw std::exception(std::string("SortBy: json argument is not an array. Argument:\n" + json.dump(4)).c_str());
        }
        else
        {
            std::vector<nlohmann::json> v = VectorFromJson(json);
            std::sort(v.begin(), v.end(), f);
            return nlohmann::json(v);
        }
    }

    // Preconditions:
    // *json* argument is an array.
    static std::optional<nlohmann::json> TryFirstWhere(std::function<bool(nlohmann::json)> f, nlohmann::json json)
    {
        if (!json.is_array())
        {
            throw std::exception(std::string("TryFirstWhere: json argument is not an array. Argument:\n" + json.dump(4)).c_str());
        }
        else
        {        
            std::optional<nlohmann::json> retval = std::nullopt;
            for (auto& item : json)
            {
                if (f(item))
                {
                    retval = std::optional<nlohmann::json>{ item };
                    break;
                }
            }
            return retval;
        }
    }

    // Preconditions:
    // *json* argument is an array.
    static std::vector<nlohmann::json> VectorFromJson(nlohmann::json json)
    {
        if (!json.is_array())
        {
            throw std::exception(std::string("VectorFromJson: json argument is not an array. Argument:\n" + json.dump(4)).c_str());
        }
        else
        {
            std::vector<nlohmann::json> retval;
            for (auto& item : json)
            {
                retval.push_back(item);
            }
            return retval;
        }
    }
};
