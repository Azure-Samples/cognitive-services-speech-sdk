//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
// intentapi_cxx_pattern_matching_model.h: Intent recognition API declarations for PatternMatchingModel C++ class
//
#pragma once

#include <iterator>

#include <ajv.h>
#include <intentapi_cxx_language_understanding_model.h>
#include <intentapi_cxx_pattern_matching_entity.h>
#include <intentapi_cxx_pattern_matching_intent.h>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {

/// <summary>
/// Represents a pattern matching model used for intent recognition.
/// </summary>
class PatternMatchingModel : public LanguageUnderstandingModel
{
public:

    /// <summary>
    /// Creates a pattern matching model using the specified model ID.
    /// </summary>
    /// <param name="modelId">A string that represents a unique Id for this model.</param>
    /// <returns>A shared pointer to pattern matching model.</returns>
    static std::shared_ptr<PatternMatchingModel> FromModelId(const std::string& modelId)
    {
            return std::shared_ptr<PatternMatchingModel> {
                new PatternMatchingModel(modelId)
            };
    }

    /// <summary>
    /// Creates a pattern matching model using the specified .json file. This should follow the Microsoft LUIS JSON export schema.
    /// </summary>
    /// <param name="filepath">A string that representing the path to a '.json' file.</param>
    /// <returns>A shared pointer to pattern matching model.</returns>
    static std::shared_ptr<PatternMatchingModel> FromJSONFile(const std::string& filepath)
    {
        FILE* fp;
        int err;
#ifdef _MSC_VER
        err = fopen_s(&fp, filepath.c_str(), "r");
#else
        fp = fopen(filepath.c_str(), "r");
        if (fp == NULL)
        {
            err = -1;
        }
        else
        {
            err = 0;
        }
#endif
        if (err == 0 && fp != NULL)
        {
            char buffer[1024] = {};
            size_t numread = 0;
            std::string fileContents = "";
#ifdef _MSC_VER
            while ((numread = fread_s((void**)&buffer, sizeof(buffer), sizeof(char), sizeof(buffer), fp)) != 0)
#else
            while ((numread = fread((void**)&buffer, sizeof(char), sizeof(buffer), fp)) != 0)
#endif
            {
                fileContents.append(buffer, numread);
            }
            fclose(fp);
            return ParseJSONFile(fileContents);
        }
        else
        {
            // Attempt to read filepath failed.
            return nullptr;
        }
    }

    /// <summary>
    /// Creates a PatternMatchingModel using the specified istream  pointing to an .json file in the LUIS json format.
    /// This assumes the stream is already open and has permission to read.
    /// </summary>
    /// <param name="iStream">A stream that representing a '.json' file.</param>
    /// <returns>A shared pointer to pattern matching model.</returns>
    static std::shared_ptr<PatternMatchingModel> FromJSONFileStream(std::istream& iStream)
    {
        std::istreambuf_iterator<char> iterator{ iStream };
        std::string str(iterator, {});
        return ParseJSONFile(str);
    }

    /// <summary>
    /// Returns id for this model.
    /// </summary>
    /// <returns>A string representing the id of this model.</returns>
    std::string GetModelId() const { return m_modelId; }

    /// <summary>
    /// This container of Intents is used to define all the Intents this model will look for.
    /// </summary>
    std::vector<PatternMatchingIntent> Intents;

    /// <summary>
    /// This container of Intents is used to define all the Intents this model will look for.
    /// </summary>
    std::vector<PatternMatchingEntity> Entities;

private:
    PatternMatchingModel(const std::string& modelId) : LanguageUnderstandingModel(LanguageUnderstandingModelType::PatternMatchingModel), m_modelId(modelId) {}

    std::string m_modelId;

    static std::shared_ptr<PatternMatchingModel> ParseJSONFile(const std::string& fileContents)
    {
        auto model = std::shared_ptr<PatternMatchingModel>(new PatternMatchingModel(""));

        auto parser = ajv::json::Parse(fileContents);
        auto reader = parser.Reader();

        if (!reader.IsObject())
        {
            // Attempt to parse language understanding json file failed.
            return nullptr;
        }

        for (auto name = reader.FirstName(); name.IsOk(); name++)
        {
            auto nameStr = name.AsString();
            auto item = reader[nameStr.c_str()];

            if (nameStr == "luis_schema_version")
            {
                // We support any version that we are able to pull data out of.
            }
            else if (nameStr == "prebuiltEntities" && item.IsArray())
            {
                int prebuiltcount = item.ValueCount();
                for (int j = 0; j < prebuiltcount; j++)
                {
                    auto obj = item[j];
                    ParsePrebuiltEntityJson(obj, model);
                }
            }
            else if (nameStr == "name" && item.IsString())
            {
                model->m_modelId = item.AsString();
            }
            else if ((nameStr == "patternAnyEntities" || nameStr == "entities") && item.IsArray())
            {
                int anyCount = item.ValueCount();
                for (int j = 0; j < anyCount; j++)
                {
                    auto obj = item[j];
                    ParseEntityJson(obj, model);
                }
            }
            else if (nameStr == "patterns" && item.IsArray())
            {
                int patternCount = item.ValueCount();
                for (int j = 0; j < patternCount; j++)
                {
                    auto obj = item[j];
                    ParsePatternJson(obj, model);
                }
            }
            else if (nameStr == "closedLists" && item.IsArray())
            {
                int listCount = item.ValueCount();
                for (int j = 0; j < listCount; j++)
                {
                    auto obj = item[j];
                    ParseListEntityJson(obj, model);
                }
            }
        }
        return model;
    }

    static void ParsePrebuiltEntityJson(ajv::JsonReader& reader, std::shared_ptr<PatternMatchingModel> model)
    {
        for (auto name = reader.FirstName(); name.IsOk(); name++)
        {
            auto nameStr = name.AsString();
            auto item = reader[nameStr.c_str()];
            if (nameStr == "name" && item.IsString())
            {
                auto valueStr = item.AsString();
                if (valueStr == "number")
                {
                    model->Entities.push_back({ "number", EntityType::PrebuiltInteger, EntityMatchMode::Basic, {} });
                }
                // ignore any other prebuilt types as they are not supported.
            }
        }
    }

    static void ParseEntityJson(ajv::JsonReader& reader, std::shared_ptr<PatternMatchingModel> model)
    {
        for (auto name = reader.FirstName(); name.IsOk(); name++)
        {
            auto nameStr = name.AsString();
            auto item = reader[nameStr.c_str()];
            if (nameStr == "name" && item.IsString())
            {
                auto valueStr = item.AsString();
                if (!valueStr.empty())
                {
                    model->Entities.push_back({ valueStr, EntityType::Any, EntityMatchMode::Basic, {} });
                }
                // ignore any other pairs since we only care about the name.
            }
        }
    }

    static void ParseListEntityJson(ajv::JsonReader& reader, std::shared_ptr<PatternMatchingModel> model)
    {
        // Default to Strict matching.
        PatternMatchingEntity entity{ "", EntityType::List, EntityMatchMode::Strict, {} };

        for (auto name = reader.FirstName(); name.IsOk(); name++)
        {
            auto nameStr = name.AsString();
            auto item = reader[nameStr.c_str()];

            if (nameStr == "name" && item.IsString())
            {
                auto valueStr = item.AsString();
                if (!valueStr.empty())
                {
                    entity.Id = valueStr;
                }
            }
            else if (nameStr == "subLists" && item.IsArray())
            {
                ParseSubList(item, entity);
            }
        }

        model->Entities.push_back(entity);
    }

    static void ParseSubList(ajv::JsonReader& reader, PatternMatchingEntity& entity)
    {
        int subListCount = reader.ValueCount();

        for (int subListIndex = 0; subListIndex < subListCount; subListIndex++)
        {
            auto subList = reader[subListIndex];

            for (auto name = subList.FirstName(); name.IsOk(); name++)
            {
                auto nameStr = name.AsString();
                auto item = subList[nameStr.c_str()];

                if (nameStr == "canonicalForm" && item.IsString())
                {
                    auto valueStr = item.AsString();
                    if (!valueStr.empty())
                    {
                        entity.Phrases.push_back(valueStr);
                    }
                }
                else if (nameStr == "list" && item.IsArray())
                {
                    auto subListSynonymItemCount = item.ValueCount();
                    for (int subListSynonymIndex = 0; subListSynonymIndex < subListSynonymItemCount; subListSynonymIndex++)
                    {
                        auto valueStr = item[subListSynonymIndex].AsString();
                        if (!valueStr.empty())
                        {
                            entity.Phrases.push_back(valueStr);
                        }
                    }
                }
            }
        }
    }

    static void ParsePatternJson(ajv::JsonReader& reader, std::shared_ptr<PatternMatchingModel> model)
    {
        std::string patternStr, intentIdStr;

        for (auto name = reader.FirstName(); name.IsOk(); name++)
        {
            auto nameStr = name.AsString();
            auto item = reader[nameStr.c_str()];
            if (item.IsString())
            {
                auto valueStr = item.AsString();
                if (!valueStr.empty())
                {
                    if (nameStr == "pattern")
                    {
                        patternStr = valueStr;
                    }
                    else if (nameStr == "intent")
                    {
                        intentIdStr = valueStr;
                    }
                }
            }
        }

        if (!patternStr.empty() && !intentIdStr.empty())
        {
            bool added = false;

            for (auto& intent : model->Intents)
            {
                if (intent.Id == intentIdStr)
                {
                    intent.Phrases.push_back(patternStr);
                    added = true;
                    break;
                }
            }

            if (!added)
            {
                model->Intents.push_back({ {patternStr}, intentIdStr });
            }
        }
    }

};

}}}}
