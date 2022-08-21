//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Translation;
using Microsoft.CognitiveServices.Speech.Transcription;
using System.Collections.Generic;
using System.Globalization;

namespace Azure.AI.Details.Common.CLI
{
    public struct Caption
    {
        public int Sequence;
        public TimeSpan Begin;
        public TimeSpan End;
        public string Text;
    }

    public class CaptionHelper
    {
// JW 20220819
/*
        public static IEnumerable<Caption> GetCaptions(string language, int maxWidth, int maxHeight, IEnumerable<object> results)
        {
            var helper = new CaptionHelper(language, maxWidth, maxHeight, results);
            return helper.GetCaptions();
        }
*/

        public CaptionHelper(string language, int maxWidth, int maxHeight, IEnumerable<object> results)
        {
            this._language = language;

            this._maxWidth = maxWidth;
            this._maxHeight = maxHeight;
            this._results = results;
// JW 20220819
            this._captions = new List<Caption>();

            // consider adapting to use http://unicode.org/reports/tr29/#Sentence_Boundaries
            var iso639 = _language?.Split('-').FirstOrDefault();
            this._firstPassTerminators = iso639 switch
            {
                "zh" => new string[]{"，", "、", "；", "？", "！", "?", "!", ",", ";"},
                _ => new string[]{"?", "!", ",", ";"}
            };
            this._secondPassTerminators = iso639 switch
            {
                "zh" => new string[]{"。", " "},
                _ => new string[]{" ", "."}
            };

            if (maxWidth == 37 && iso639 == "zh")
            {
                this._maxWidth = 30;
            }
        }

// JW 20220819
/*
        public IEnumerable<Caption> GetCaptions()
        {
            EnsureCaptions();
            return _captions;
        }

        private void EnsureCaptions()
        {
            if (_captions == null)
            {
                _captions = new List<Caption>();
                AddCaptionsForAllResults();
            }
        }

        private void AddCaptionsForAllResults()
        {
            foreach (RecognitionResult result in _results)
            {
                if (result.OffsetInTicks <= 0 || !IsFinalResult(result)) continue;

                var text = GetTextOrTranslation(result);
                if (string.IsNullOrEmpty(text)) continue;

                AddCaptionsForFinalResult(result, text);
            }
        }
*/

        // JW 20220819 Changed return type to nullable.
        private string? GetTextOrTranslation(RecognitionResult result)
        {
            if (_language == null) return result.Text;

            return result is TranslationRecognitionResult translated && translated.Translations.Keys.Contains(_language)
                ? translated.Translations[_language]
                : null;
        }

// JW 20220819
//        private void AddCaptionsForFinalResult(RecognitionResult result, string text)
        public List<Caption> GetCaptions(RecognitionResult result, string text)
        {
            var captionStartsAt = 0;
            var captionLines = new List<string>();

// JW 20220819
            var retval = new List<Caption>();

            var index = 0;
            while (index < text.Length)
            {
                index = SkipSkippable(text, index);

                int lineLength = GetBestWidth(text, index);
                captionLines.Add(text.Substring(index, lineLength).Trim());
                index = index + lineLength;

                var isLastCaption = index >= text.Length;
                var maxCaptionLines = captionLines.Count >= _maxHeight;

                var addCaption = isLastCaption || maxCaptionLines;
                if (addCaption)
                {
                    var captionText = string.Join('\n', captionLines.ToArray());
                    captionLines.Clear();

                    var captionSequence = _captions.Count + 1;
                    var isFirstCaption = captionStartsAt == 0;

                    (var captionBegin, var captionEnd) = isFirstCaption && isLastCaption
                        ? GetFullResultCaptionTiming(result)
                        : GetPartialResultCaptionTiming(result, text, captionText, captionStartsAt, index - captionStartsAt);

                    var caption = new Caption()
                    {
                        Sequence = captionSequence,
                        Begin = captionBegin,
                        End = captionEnd,
                        Text = captionText
                    };
                    _captions.Add(caption);
                    
                    captionStartsAt = index;
                    
                    // JW 20220819
                    retval.Add(caption);
                }
            }
            
            // JW 20220819
            return retval;
        }

        private int GetBestWidth(string text, int startIndex)
        {
            var remaining = text.Length - startIndex;
            var bestWidth = remaining < _maxWidth
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

        private int FindBestWidth(IEnumerable<string> terminators, string text, int startAt)
        {
            var remaining = text.Length - startAt;
            var checkChars = Math.Min(remaining, _maxWidth);

            var bestWidth = -1;
            foreach (var terminator in terminators)
            {
                var index = text.LastIndexOf(terminator, startAt + checkChars, checkChars);
                var width = index - startAt;
                if (width > bestWidth)
                {
                    bestWidth = width + terminator.Length;
                }
            }

            return bestWidth;
        }

        private int SkipSkippable(string text, int startIndex)
        {
            var index = startIndex;
            while (text.Length > index && text[index] == ' ')
                index++;

            return index;
        }

        private (TimeSpan captionBegin, TimeSpan captionEnd) GetFullResultCaptionTiming(RecognitionResult result)
        {
            var resultBegin = TimeSpan.FromTicks(result.OffsetInTicks);
            var resultEnd = resultBegin + result.Duration;
            return (resultBegin, resultEnd);
        }

        private (TimeSpan captionBegin, TimeSpan captionEnd) GetPartialResultCaptionTiming(RecognitionResult result, string text, string captionText, int captionStartsAt, int captionLength)
        {
            (var resultBegin, var resultEnd) = GetFullResultCaptionTiming(result);
            var resultDuration = resultEnd.Subtract(resultBegin);

            var textLength = text.Length;
            var partialBegin = resultBegin + resultDuration * captionStartsAt / textLength;
            var partialEnd = resultBegin + resultDuration * (captionStartsAt + captionLength) / textLength;

            return (partialBegin, partialEnd);
        }

        private static bool IsFinalResult(object result)
        {
            // JW 20220819 Changed to nullable to fix compiler warning.
            // Note nullable reference types are enabled in .csproj.
            RecognitionResult? final = result as RecognitionResult;
            return final?.Reason == ResultReason.RecognizedSpeech ||
                   final?.Reason == ResultReason.RecognizedIntent ||
                   final?.Reason == ResultReason.TranslatedSpeech;
        }

        // JW 20220819 Changed to nullable to fix compiler warning.
        private readonly string? _language;
        private readonly string[] _firstPassTerminators;
        private readonly string[] _secondPassTerminators;

        private readonly int _maxWidth;
        private readonly int _maxHeight;
        private readonly IEnumerable<object> _results;

        private List<Caption> _captions;
    }
}
