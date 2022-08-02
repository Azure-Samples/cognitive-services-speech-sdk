//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
// You can download json.hpp from:
// https://github.com/nlohmann/json/releases
#include "json.hpp"

#if !defined( JSON_HELPER_H )
#define JSON_HELPER_H

class JsonHelper
{
public:

    // Preconditions:
    // *json* is an array.
    // *json* has a size of at least 1.
    static nlohmann::json Chunk(nlohmann::json json, size_t size)
    {
        std::vector<nlohmann::json> chunk;
        std::vector<std::vector<nlohmann::json>> destination;
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
                destination.push_back(chunk);
                counter = 0;
                chunk.clear();
            }
        }
        if (0 < chunk.size())
        {
            destination.push_back(chunk);
        }
        return nlohmann::json(destination);
    }

    // Preconditions:
    // *outerJson* is an array.
    // Each element in *outerJson* is an array.
    static nlohmann::json Concat(nlohmann::json outerJson)
    {
        std::vector<nlohmann::json> destination;
        if (!outerJson.is_array())
        {
            throw std::exception(std::string("Flatten: outerJson is not an array. outerJson:\n" + outerJson.dump(4)).c_str());
        }
        else
        {
            for (auto& innerJson : outerJson)
            {
                if (!innerJson.is_array())
                {
                    throw std::exception(std::string("Flatten: innerJson is not an array. innerJson:\n" + innerJson.dump(4)).c_str());
                }
                else
                {
                    for (auto& item : innerJson)
                    {
                        destination.push_back(item);
                    }
                }
            }
            return nlohmann::json(destination);
        }
    }

    // Note: Templated classes/functions must be declared in header file, not source file. See:
    // https://stackoverflow.com/a/456716
    template<typename Function>
    static nlohmann::json Map(Function f, nlohmann::json json)
    {
        std::vector<nlohmann::json> source = VectorFromJson(json);
        std::vector<nlohmann::json> destination;
        destination.resize(source.size());
        std::transform(source.begin(), source.end(), destination.begin(), f);
        return nlohmann::json(destination);
    }

    template<typename Function>
    static nlohmann::json Fold(Function f, nlohmann::json acc, nlohmann::json items)
    {
        if (!items.is_array())
        {
            throw std::exception(std::string("Reduce: items argument is not an array. Argument:\n" + items.dump(4)).c_str());
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

    template<typename Function>
    static nlohmann::json SortBy(Function f, nlohmann::json json)
    {
        std::vector<nlohmann::json> v = VectorFromJson(json);
        std::sort(v.begin(), v.end(), f);
        return nlohmann::json(v);
    }
    
    static std::optional<nlohmann::json> TryFirst(std::function<bool(nlohmann::json)> f, nlohmann::json json)
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

    static std::vector<nlohmann::json> VectorFromJson(nlohmann::json json)
    {
        if (!json.is_array())
        {
            throw std::exception(std::string("JsonToVector: json argument is not an array. Argument:\n" + json.dump(4)).c_str());
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

#endif