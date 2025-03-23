//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//
#pragma once
#include <array>
#include <list>
#include <map>
#include <memory>
#include <string>

#include "maybe.h"

#include <intentapi_cxx_enums.h>

namespace Microsoft {
namespace SpeechSDK {
namespace Standalone {
namespace Intent {
namespace Impl {

struct OrthographyInformation
{
    std::string Name;
    std::string Whitespace;
    std::string InputPunctuation;
    std::string PatternPunctuation;
    std::string SentenceEndCharacters;
    std::array<char, 4> WordBoundary;
    bool RightToLeft;
};

class ISpxEntity // -> CSpxIntegerEntity, CSpxListEntity, CSpxPatternAnyEntity
{
public:
    using Ptr = std::shared_ptr<ISpxEntity>;

    virtual const std::string& GetName() const = 0;
    virtual Intent::EntityMatchMode GetMode() const = 0;
    virtual Intent::EntityType GetType() const = 0;
    virtual unsigned int GetGreed() const = 0;
    virtual bool IsRequired() const = 0;

    virtual Maybe<std::string> Parse(const std::string & input) const = 0;

    virtual void Init(const std::string& name, const OrthographyInformation& orthography) = 0;
    virtual void SetMode(Intent::EntityMatchMode mode) = 0;
    virtual void AddPhrase(const std::string& phrase) = 0;
};

class ISpxIntegerParser // -> CSpxENIntegerParser, CSpxESIntegerParser, CSpxFRIntegerParser, CSpxJPIntegerParser, CSpxZHIntegerParser, NoOpIntegerParser
{
public:
    virtual Maybe<std::string> Parse(const std::string & input) const = 0;
};

struct EntityResult
{
    std::string Value;
    Intent::EntityType Type;
};

class CSpxPatternMatchingModel;

class ISpxTrigger // -> CSpxIntentTrigger, CSpxPatternMatchingIntent
{
public:
    using Ptr = std::shared_ptr<ISpxTrigger>;

    virtual void InitPhraseTrigger(const std::string& phrase) = 0;
    virtual void InitPatternMatchingModelTrigger(const std::shared_ptr<CSpxPatternMatchingModel>& model) = 0;

    virtual const std::string GetPhrase() const = 0;

    virtual std::shared_ptr<CSpxPatternMatchingModel> GetModel() const = 0;
};

}}}}}
