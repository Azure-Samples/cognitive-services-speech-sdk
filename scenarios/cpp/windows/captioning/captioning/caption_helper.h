//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <speechapi_cxx.h>
#include <string>
#include <vector>
#include "string_helper.h"
#include "user_config.h"

using namespace Microsoft::CognitiveServices::Speech;

struct Timestamp
{
    int Hours;
    int Minutes;
    int Seconds;
    int Milliseconds;

    Timestamp(
        int hours,
        int minutes,
        int seconds,
        int milliseconds
    ) :
    Hours(hours),
    Minutes(minutes),
    Seconds(seconds),
    Milliseconds(milliseconds)
    {}
};

int CompareTimestamps(Timestamp t1, Timestamp t2);
uint64_t MillisecondsFromTimestamp(Timestamp ts);
std::string StringFromTimestamp(Timestamp ts, bool srt);
Timestamp TimestampFromMilliseconds(uint64_t milliseconds);
Timestamp TimestampFromTicks(uint64_t ticks);
Timestamp TimestampPlusMilliseconds(Timestamp ts, uint32_t milliseconds);

struct Caption
{
    std::optional<std::string> language;
    int sequence;
    Timestamp begin;
    Timestamp end;
    std::string text;
    
    Caption(std::optional<std::string> language, int sequence, Timestamp begin, Timestamp end, std::string text) : language(language), sequence(sequence), begin(begin), end(end), text(text)
    {}
};

struct CaptionTiming
{
    Timestamp begin;
    Timestamp end;
    
    CaptionTiming(Timestamp begin, Timestamp end) : begin(begin), end(end)
    {}
};

class CaptionHelper
{
private:

    std::optional<std::string> _language;
    std::vector<std::string> _firstPassTerminators;
    std::vector<std::string> _secondPassTerminators;

    int _maxWidth;
    int _maxHeight;
    std::vector<std::shared_ptr<RecognitionResult>> _results;

    std::optional<std::vector<Caption>> _captions;

public:

    CaptionHelper(std::optional<std::string> language, int maxWidth, int maxHeight, std::vector<std::shared_ptr<RecognitionResult>> results) : _language(language), _maxWidth(maxWidth), _maxHeight(maxHeight), _results(results)
    {
        // consider adapting to use http://unicode.org/reports/tr29/#Sentence_Boundaries
        std::string iso639;
        if (_language.has_value())
        {
            iso639 = StringHelper::Split(_language.value(), '-')[0];
        }

        if (StringHelper::CaseInsensitiveCompare(iso639, "zh"))
        {
            _firstPassTerminators = {"，", "、", "；", "？", "！", "?", "!", ",", ";"};
            _secondPassTerminators = {"。", " "};
        }
        else
        {
            _firstPassTerminators = {"?", "!", ",", ";"};
            _secondPassTerminators = {" ", "."};
        }

        if (maxWidth == UserConfig::defaultMaxLineLengthSBCS && (StringHelper::CaseInsensitiveCompare(iso639, "zh")))
        {
            _maxWidth = UserConfig::defaultMaxLineLengthMBCS;
        }
    }

    static std::vector<Caption> GetCaptions(std::optional<std::string> language, int maxWidth, int maxHeight, std::vector<std::shared_ptr<RecognitionResult>> results)
    {
        auto helper = std::make_shared<CaptionHelper>(language, maxWidth, maxHeight, results);
        return helper->GetCaptions();
    }

    std::vector<std::string> LinesFromText(std::string text)
    {
        std::vector<std::string> retval;
        
        auto index = 0;
        while (index < text.length())
        {
            index = SkipSkippable(text, index);

            int lineLength = GetBestWidth(text, index);
            retval.push_back(StringHelper::Trim(text.substr(index, lineLength)));
            index = index + lineLength;
        }
        
        return retval;
    }
    
    std::vector<Caption> GetCaptions()
    {
        EnsureCaptions();
        return _captions.value();
    }

    void EnsureCaptions()
    {
        if (!_captions.has_value())
        {
            _captions = std::vector<Caption>();
            AddCaptionsForAllResults();
        }
    }

    void AddCaptionsForAllResults()
    {
        for (std::shared_ptr<RecognitionResult> result : _results)
        {
            // RecognitionResult.Offset is uint64_t so cannot be less than 0.
            if (0 == result->Offset() || !CaptionHelper::IsFinalResult(result))
            {
                continue;
            }

            std::optional<std::string> text = GetTextOrTranslation(result);
            if (!text.has_value())
            {
                continue;
            }

            AddCaptionsForFinalResult(result, text.value());
        }
    }
    
    std::optional<std::string> GetTextOrTranslation(std::shared_ptr<RecognitionResult> result)
    {
        return result->Text;
        // TODO2 Handle TranslationRecognitionResults.
    }
    
    void AddCaptionsForFinalResult(std::shared_ptr<RecognitionResult> result, std::string text)
    {
        auto captionStartsAt = 0;
        std::vector<std::string> captionLines;

        auto index = 0;
        while (index < text.length())
        {
            index = SkipSkippable(text, index);

            int lineLength = GetBestWidth(text, index);
            captionLines.push_back(StringHelper::Trim(text.substr(index, lineLength)));
            index = index + lineLength;

            auto isLastCaption = index >= text.length();
            auto maxCaptionLines = captionLines.size() >= _maxHeight;

            auto addCaption = isLastCaption || maxCaptionLines;
            if (addCaption)
            {
                auto captionText = StringHelper::Join(captionLines, "\n");
                captionLines.clear();

                auto captionSequence = _captions.value().size() + 1;
                auto isFirstCaption = captionStartsAt == 0;

                auto captionTiming = isFirstCaption && isLastCaption
                    ? GetFullResultCaptionTiming(result)
                    : GetPartialResultCaptionTiming(result, text, captionText, captionStartsAt, index - captionStartsAt);

                _captions.value().push_back(Caption(_language, captionSequence, captionTiming.begin, captionTiming.end, captionText));
                
                captionStartsAt = index;
            }
        }
    }

    int GetBestWidth(std::string text, int startIndex)
    {
        auto remaining = text.length() - startIndex;
        // Do not use auto for bestWidth, because C++ will assign type uint64_t.
        // However, FindBestWidth can return -1.
        int bestWidth = remaining < _maxWidth
            ? remaining
            : FindBestWidth(_firstPassTerminators, text, startIndex);

        if (bestWidth < 0)
        {
            bestWidth = FindBestWidth(_secondPassTerminators, text, startIndex);
        }

        if (bestWidth < 0)
        {
            bestWidth = _maxWidth;
        }

        return bestWidth;
    }

    int FindBestWidth(std::vector<std::string> terminators, std::string text, int startAt)
    {
        auto remaining = text.length() - startAt;
        auto checkChars = remaining < _maxWidth ? remaining : _maxWidth;

        auto bestWidth = -1;
        for (auto terminator : terminators)
        {
            // We need to get the last index of the terminator,
            // but only searching from startAt to startAt + checkChars.
            // So we take a substring of text from startAt to
            // startAt + checkChars.
            // Afterward, we need to re-add startAt to the resulting
            // index (which is from the substring) to align it with
            // the text from which the substring was taken.
            // Again, do not use auto for index, because find_last_of
            // can return npos, which is -1.
            int index = text.substr(startAt, checkChars).find_last_of(terminator) + startAt;
            int width = index - startAt;
            if (width > bestWidth)
            {
                bestWidth = width + terminator.length();
            }
        }

        return bestWidth;
    }

    int SkipSkippable(std::string text, int startIndex)
    {
        auto index = startIndex;
        while (text.length() > index && text[index] == ' ')
        {
            index++;
        }

        return index;
    }
    
    CaptionTiming GetFullResultCaptionTiming(std::shared_ptr<RecognitionResult> result)
    {
        auto resultBegin = TimestampFromTicks(result->Offset());
        auto resultEnd = TimestampFromTicks(result->Offset() + result->Duration());
        return CaptionTiming(resultBegin, resultEnd);
    }

    CaptionTiming GetPartialResultCaptionTiming(std::shared_ptr<RecognitionResult> result, std::string text, std::string captionText, int captionStartsAt, int captionLength)
    {
        auto captionTiming = GetFullResultCaptionTiming(result);
        auto msBegin = MillisecondsFromTimestamp(captionTiming.begin);
        auto msEnd = MillisecondsFromTimestamp(captionTiming.end);
        auto msDuration = msEnd - msBegin;
        auto textLength = text.length();
        // TODO2 Consider something more precise than ms.
        auto partialBegin = msBegin + msDuration * captionStartsAt / textLength;
        auto partialEnd = msBegin + msDuration * (captionStartsAt + captionLength) / textLength;
        return CaptionTiming(TimestampFromMilliseconds(partialBegin), TimestampFromMilliseconds(partialEnd));
    }

    static bool IsFinalResult(std::shared_ptr<RecognitionResult> result)
    {
        return result->Reason == ResultReason::RecognizedSpeech ||
               result->Reason == ResultReason::RecognizedIntent ||
               result->Reason == ResultReason::TranslatedSpeech;
    }
};