//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using DiffPlex;
using DiffPlex.DiffBuilder;
using DiffPlex.DiffBuilder.Model;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.PronunciationAssessment;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionSamples
    {

        // Pronunciation assessment with microphone as audio input.
        // See more information at https://aka.ms/csspeech/pa
        public static async Task PronunciationAssessmentWithMicrophoneAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Replace the language with your language in BCP-47 format, e.g., en-US.
            var language = "en-US";

            // The pronunciation assessment service has a longer default end silence timeout (5 seconds) than normal STT
            // as the pronunciation assessment is widely used in education scenario where kids have longer break in reading.
            // You can adjust the end silence timeout based on your real scenario.
            config.SetProperty(PropertyId.SpeechServiceConnection_EndSilenceTimeoutMs, "3000");

            var referenceText = "";
            // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
            var pronunciationConfig = new PronunciationAssessmentConfig(referenceText,
                GradingSystem.HundredMark, Granularity.Phoneme, true);

            pronunciationConfig.EnableProsodyAssessment();

            // Creates a speech recognizer for the specified language, using microphone as audio input.
            using (var recognizer = new SpeechRecognizer(config, language))
            {
                recognizer.SessionStarted += (s, e) => {
                    Console.WriteLine($"SESSION ID: {e.SessionId}");
                };

                while (true)
                {
                    // Receives reference text from console input.
                    Console.WriteLine("Enter reference text you want to assess, or enter empty text to exit.");
                    Console.Write("> ");
                    referenceText = Console.ReadLine();
                    if (string.IsNullOrEmpty(referenceText))
                    {
                        break;
                    }

                    pronunciationConfig.ReferenceText = referenceText;

                    // Starts recognizing.
                    Console.WriteLine($"Read out \"{referenceText}\" for pronunciation assessment ...");

                    pronunciationConfig.ApplyTo(recognizer);

                    // Starts speech recognition, and returns after a single utterance is recognized.
                    // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                    var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                    // Checks result.
                    if (result.Reason == ResultReason.RecognizedSpeech)
                    {
                        Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                        Console.WriteLine("  PRONUNCIATION ASSESSMENT RESULTS:");

                        var pronunciationResult = PronunciationAssessmentResult.FromResult(result);
                        Console.WriteLine(
                            $"    Accuracy score: {pronunciationResult.AccuracyScore}, Prosody Score: {pronunciationResult.ProsodyScore}, Pronunciation score: {pronunciationResult.PronunciationScore}, Completeness score : {pronunciationResult.CompletenessScore}, FluencyScore: {pronunciationResult.FluencyScore}");

                        Console.WriteLine("  Word-level details:");

                        foreach (var word in pronunciationResult.Words)
                        {
                            Console.WriteLine($"    Word: {word.Word}, Accuracy score: {word.AccuracyScore}, Error type: {word.ErrorType}.");
                        }
                    }
                    else if (result.Reason == ResultReason.NoMatch)
                    {
                        Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                    }
                    else if (result.Reason == ResultReason.Canceled)
                    {
                        var cancellation = CancellationDetails.FromResult(result);
                        Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                        if (cancellation.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }
                    }
                }
            }
        }

        // Pronunciation assessment with audio stream input.
        // See more information at https://aka.ms/csspeech/pa
        public static void PronunciationAssessmentWithStream()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Read audio data from file. In real scenario this can be from memory or network
            var audioDataWithHeader = File.ReadAllBytes("whatstheweatherlike.wav");
            var audioData = new byte[audioDataWithHeader.Length - 46];
            Array.Copy(audioDataWithHeader, 46, audioData, 0, audioData.Length);

            var resultReceived = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
            var resultContainer = new List<string>();

            var startTime = DateTime.Now;

            var task = PronunciationAssessmentWithStreamInternalAsync(config, "what's the weather like", audioData, resultReceived, resultContainer);
            Task.WaitAny(new[] { resultReceived.Task });
            var resultJson = resultContainer[0];

            var endTime = DateTime.Now;

            Console.WriteLine(resultJson);

            var timeCost = endTime.Subtract(startTime).TotalMilliseconds;
            Console.WriteLine($"Time cost: {timeCost}ms");
        }

        private static async Task PronunciationAssessmentWithStreamInternalAsync(SpeechConfig speechConfig, string referenceText, byte[] audioData, TaskCompletionSource<int> resultReceived, List<string> resultContainer)
        {
            using (var audioInputStream = AudioInputStream.CreatePushStream(AudioStreamFormat.GetWaveFormatPCM(16000, 16, 1))) // This need be set based on the format of the given audio data
            using (var audioConfig = AudioConfig.FromStreamInput(audioInputStream))
            // Specify the language used for Pronunciation Assessment.
            using (var speechRecognizer = new SpeechRecognizer(speechConfig, "en-US", audioConfig))
            {
                // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
                var pronAssessmentConfig = new PronunciationAssessmentConfig(referenceText, GradingSystem.HundredMark, Granularity.Phoneme, false);

                pronAssessmentConfig.EnableProsodyAssessment();

                speechRecognizer.SessionStarted += (s, e) => {
                    Console.WriteLine($"SESSION ID: {e.SessionId}");
                };

                pronAssessmentConfig.ApplyTo(speechRecognizer);

                audioInputStream.Write(audioData);
                audioInputStream.Write(new byte[0]); // send a zero-size chunk to signal the end of stream

                var result = await speechRecognizer.RecognizeOnceAsync().ConfigureAwait(false);
                if (result.Reason == ResultReason.Canceled)
                {
                    var cancellationDetail = CancellationDetails.FromResult(result);
                    Console.Write(cancellationDetail);
                }
                else
                {
                    var responseJson = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                    resultContainer.Add(responseJson);
                }

                resultReceived.SetResult(1);
            }
        }

        // Pronunciation assessment configured with json
        // See more information at https://aka.ms/csspeech/pa
        public static async Task PronunciationAssessmentConfiguredWithJson()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Replace the language with your language in BCP-47 format, e.g., en-US.
            var language = "en-US";

            // Creates an instance of audio config from an audio file
            var audioConfig = AudioConfig.FromWavFileInput(@"whatstheweatherlike.wav");

            var referenceText = "what's the weather like";
            // create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
            string json_config = "{\"GradingSystem\":\"HundredMark\",\"Granularity\":\"Phoneme\",\"EnableMiscue\":true, \"ScenarioId\":\"\"}";
            var pronunciationConfig = PronunciationAssessmentConfig.FromJson(json_config);
            pronunciationConfig.ReferenceText = referenceText;

            pronunciationConfig.EnableProsodyAssessment();

            // Creates a speech recognizer for the specified language
            using (var recognizer = new SpeechRecognizer(config, language, audioConfig))
            {

                recognizer.SessionStarted += (s, e) => {
                    Console.WriteLine($"SESSION ID: {e.SessionId}");
                };

                // Starts recognizing.
                pronunciationConfig.ApplyTo(recognizer);

                // Starts speech recognition, and returns after a single utterance is recognized.
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine("  PRONUNCIATION ASSESSMENT RESULTS:");

                    var pronunciationResult = PronunciationAssessmentResult.FromResult(result);
                    Console.WriteLine(
                        $"    Accuracy score: {pronunciationResult.AccuracyScore}, Prosody Score: {pronunciationResult.ProsodyScore}, Pronunciation score: {pronunciationResult.PronunciationScore}, Completeness score : {pronunciationResult.CompletenessScore}, FluencyScore: {pronunciationResult.FluencyScore}");

                    Console.WriteLine("  Word-level details:");

                    foreach (var word in pronunciationResult.Words)
                    {
                        Console.WriteLine($"    Word: {word.Word}, Accuracy score: {word.AccuracyScore}, Error type: {word.ErrorType}.");
                    }
                }
                else if (result.Reason == ResultReason.NoMatch)
                {
                    Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                }
                else if (result.Reason == ResultReason.Canceled)
                {
                    var cancellation = CancellationDetails.FromResult(result);
                    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                    if (cancellation.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                }
            }
        }
        private static List<string> ConvertReferenceWords(string referenceText, List<string> referenceWords)
        {
            HashSet<string> dictionary = new HashSet<string>(referenceWords);
            int maxLength = dictionary.Max(word => word.Length);

            referenceText = RemovePunctuation(referenceText);
            return SegmentWord(referenceText, dictionary, maxLength);
        }

        private static string RemovePunctuation(string text)
        {
            // Remove punctuation from the reference text
            return new string(text.Where(c => char.IsLetterOrDigit(c) || char.IsWhiteSpace(c)).ToArray());
        }

        private static List<string> SegmentWord(string referenceText, HashSet<string> dictionary, int maxLength)
        {
            List<string> leftToRight = LeftToRightSegmentation(referenceText, dictionary, maxLength);
            List<string> rightToLeft = RightToLeftSegmentation(referenceText, dictionary, maxLength);

            if (string.Join("", leftToRight) == referenceText)
                return leftToRight;
            if (string.Join("", rightToLeft) == referenceText)
                return rightToLeft;

            Console.WriteLine("WW failed to segment the text with the dictionary");

            if (leftToRight.Count < rightToLeft.Count)
                return leftToRight;
            if (leftToRight.Count > rightToLeft.Count)
                return rightToLeft;

            // If the word number is the same, then return the one with the smallest single word
            int leftToRightSingle = leftToRight.Count(word => word.Length == 1);
            int rightToLeftSingle = rightToLeft.Count(word => word.Length == 1);
            return leftToRightSingle < rightToLeftSingle ? leftToRight : rightToLeft;
        }

        // From left to right to do the longest matching to get the word segmentation
        private static List<string> LeftToRightSegmentation(string text, HashSet<string> dictionary, int maxLength)
        {
            List<string> result = new List<string>();
            while (text.Length > 0)
            {
                // If the length of the text is less than the max_length, then the sub_text is the text itself
                string subText = text.Length < maxLength ? text : text.Substring(0, maxLength);
                while (subText.Length > 0)
                {
                    // If the sub_text is in the dictionary or the length of the sub_text is 1, then add it to the result
                    if (dictionary.Contains(subText) || subText.Length == 1)
                    {
                        result.Add(subText);
                        text = text.Substring(subText.Length);
                        break;
                    }
                    // # If the sub_text is not in the dictionary, then remove the last character of the sub_text
                    subText = subText.Substring(0, subText.Length - 1);
                }
            }
            return result;
        }

        // From right to left to do the longest matching to get the word segmentation
        private static List<string> RightToLeftSegmentation(string text, HashSet<string> dictionary, int maxLength)
        {
            List<string> result = new List<string>();
            while (text.Length > 0)
            {
                // If the length of the text is less than the max_length, then the sub_text is the text itself
                string subText = text.Length < maxLength ? text : text.Substring(text.Length - maxLength);
                while (subText.Length > 0)
                {
                    // If the sub_text is in the dictionary or the length of the sub_text is 1, then add it to the result
                    if (dictionary.Contains(subText) || subText.Length == 1)
                    {
                        result.Add(subText);
                        text = text.Substring(0, text.Length - subText.Length);
                        break;
                    }

                    // If the sub_text is not in the dictionary, then remove the first character of the sub_text
                    subText = subText.Substring(1);
                }
            }

            // Reverse the result to get the correct order
            result.Reverse();
            return result;
        }
        private static List<string> GetReferenceWords(string waveFilename, string referenceText, string language, SpeechConfig speechConfig)
        {
            var audioConfig = AudioConfig.FromWavFileInput(waveFilename);
            speechConfig.SpeechRecognitionLanguage = language;

            var speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);

            // Create pronunciation assessment config, set grading system, granularity, and enable miscue based on requirement
            bool enableMiscue = true;
            var pronunciationConfig = new PronunciationAssessmentConfig(referenceText,
                GradingSystem.HundredMark, Granularity.Phoneme, enableMiscue);

            // Apply pronunciation assessment config to speech recognizer
            pronunciationConfig.ApplyTo(speechRecognizer);

            // Perform speech recognition
            var result = speechRecognizer.RecognizeOnceAsync().Result;

            if (result.Reason == ResultReason.RecognizedSpeech)
            {
                var referenceWords = new List<string>();

                var responseJson = result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                // Parse the JSON result to extract NBest and Words
                JsonDocument doc = JsonDocument.Parse(responseJson);
                JsonElement root = doc.RootElement;


                JsonElement words = root.GetProperty("NBest")[0].GetProperty("Words");
                foreach (JsonElement item in words.EnumerateArray())
                {
                    string word_item = item.GetProperty("Word").GetString();
                    string errorType_item = item.GetProperty("PronunciationAssessment").GetProperty("ErrorType").GetString();

                    if (errorType_item != "Insertion")
                    {
                        referenceWords.Add(word_item);
                    }
                }

                return ConvertReferenceWords(referenceText, referenceWords);
            }
            else if (result.Reason == ResultReason.NoMatch)
            {
                Console.WriteLine("No speech could be recognized");
                return null;
            }
            else if (result.Reason == ResultReason.Canceled)
            {
                var cancellation = CancellationDetails.FromResult(result);
                Console.WriteLine($"Speech Recognition canceled: {cancellation.Reason}");
                if (cancellation.Reason == CancellationReason.Error)
                {
                    Console.WriteLine($"Error details: {cancellation.ErrorDetails}");
                }
                return null;
            }

            return null;
        }

        // Pronunciation assessment continous from file
        // See more information at https://aka.ms/csspeech/pa
        public static async Task PronunciationAssessmentContinuousWithFile()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");
            var waveFileName = @"zhcn_continuous_mode_sample.wav";
            var scriptFileName = @"zhcn_continuous_mode_sample.txt";

            var referenceText = File.ReadAllText(scriptFileName);

            // You can adjust the segmentation silence timeout based on your real scenario.
            config.SetProperty(PropertyId.Speech_SegmentationSilenceTimeoutMs, "1500");

            // Switch to other languages for example Spanish, change language "en-US" to "es-ES". Language name is not case sensitive.
            var language = "zh-CN";
            if (language == "zh-CN")
            {
                Console.OutputEncoding = Encoding.UTF8;
            }

            // Creates a speech recognizer using file as audio input. 
            using (var audioInput = AudioConfig.FromWavFileInput(waveFileName))
            {

                using (var recognizer = new SpeechRecognizer(config, language, audioInput))
                {

                    bool enableMiscue = true;
                    bool enableProsodyAssessment = true;
                    bool unscriptedScenario = referenceText.Length == 0 ? true : false;

                    string[] referenceWords;

                    if (language == "zh-CN")
                    {
                        // Split words for Chinese using the reference text and any short wave file
                        // Remove the blank characters in the reference text
                        referenceText = referenceText.Replace(" ", "");
                        referenceWords = GetReferenceWords(@"zhcn_short_dummy_sample.wav", referenceText, language, config).ToArray();
                    }
                    else
                    {
                        referenceWords = referenceText.ToLower().Split(' ');
                        for (int j = 0; j < referenceWords.Length; j++)
                        {
                            referenceWords[j] = Regex.Replace(referenceWords[j], "^[\\p{P}\\s]+|[\\p{P}\\s]+$", "");
                        }
                    }

                    // Remove empty words
                    referenceWords = referenceWords.Where(w => w.Trim().Length > 0).ToArray();
                    referenceText = string.Join(" ", referenceWords);
                    Console.WriteLine("Reference text: " + referenceText);

                    var pronConfig = new PronunciationAssessmentConfig(referenceText, GradingSystem.HundredMark, Granularity.Phoneme, enableMiscue);

                    if (enableProsodyAssessment)
                        pronConfig.EnableProsodyAssessment();

                    recognizer.SessionStarted += (s, e) => {
                        Console.WriteLine($"SESSION ID: {e.SessionId}");
                    };

                    pronConfig.ApplyTo(recognizer);

                    var recognizedWords = new List<string>();
                    var pronWords = new List<Word>();
                    var finalWords = new List<Word>();
                    var prosody_scores = new List<double>();
                    var startOffset = 0L;
                    var endOffset = 0L;
                    var durations = new List<int>();
                    var done = false;

                    recognizer.SessionStopped += (s, e) => {
                        Console.WriteLine("ClOSING on {0}", e);
                        done = true;
                    };

                    recognizer.Canceled += (s, e) => {
                        Console.WriteLine("ClOSING on {0}", e);
                        done = true;
                    };

                    recognizer.Recognized += (s, e) => {
                        Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                        var pronResult = PronunciationAssessmentResult.FromResult(e.Result);
                        Console.WriteLine($"    Accuracy score: {pronResult.AccuracyScore}, prosody score:{pronResult.ProsodyScore}, pronunciation score: {pronResult.PronunciationScore}, completeness score: {pronResult.CompletenessScore}, fluency score: {pronResult.FluencyScore}");

                        prosody_scores.Add(pronResult.ProsodyScore);

                        foreach(var word in pronResult.Words)
                        {
                            var newWord = new Word(word.Word, word.ErrorType, word.AccuracyScore);
                            pronWords.Add(newWord);
                        }

                        foreach (var result in e.Result.Best())
                        {
                            durations.AddRange(result.Words.Select(item => item.Duration + 100000).ToList());
                            recognizedWords.AddRange(result.Words.Select(item => item.Word.ToLower()).ToList());

                            if (startOffset == 0) startOffset = result.Words.First().Offset;

                            endOffset = result.Words.Last().Offset + result.Words.Last().Duration + 100000;
                        }
                    };

                    // Starts continuous recognition.
                    await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                    while (! done)
                    {
                        // Allow the program to run and process results continuously.
                        await Task.Delay(1000); // Adjust the delay as needed.
                    }

                    // Waits for completion.
                    await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);

                    // set the duration of Word in pronWords
                    pronWords.Zip(durations, (word, duration) => word.Duration = duration).ToList();

                    // For continuous pronunciation assessment mode, the service won't return the words with `Insertion` or `Omission`
                    // even if miscue is enabled.
                    // We need to compare with the reference text after received all recognized words to get these error words.
                    if (enableMiscue && !unscriptedScenario)
                    {
                        referenceWords = AlignListsWithInlineDiff(referenceWords.ToList(), recognizedWords).ToArray();

                        var differ = new Differ();
                        var inlineBuilder = new InlineDiffBuilder(differ);
                        var diffModel = inlineBuilder.BuildDiffModel(string.Join("\n", referenceWords), string.Join("\n", recognizedWords));

                        int currentIdx = 0;

                        foreach (var delta in diffModel.Lines)
                        {
                            if (delta.Type == ChangeType.Unchanged)
                            {
                                finalWords.Add(pronWords[currentIdx]);

                                currentIdx += 1;
                            }

                            if (delta.Type == ChangeType.Deleted || delta.Type == ChangeType.Modified)
                            {
                                var word = new Word(delta.Text, "Omission");
                                finalWords.Add(word);
                            }

                            if (delta.Type == ChangeType.Inserted || delta.Type == ChangeType.Modified)
                            {
                                Word w = new Word(pronWords[currentIdx].WordText, pronWords[currentIdx].ErrorType, pronWords[currentIdx].AccuracyScore, pronWords[currentIdx].Duration);
                                if (w.ErrorType == "None")
                                {
                                    w.ErrorType = "Insertion";
                                    finalWords.Add(w);
                                }

                                currentIdx += 1;
                            }
                        }
                    }
                    else
                    {
                        finalWords = pronWords;
                    }

                    // If accuracy score is below 60, mark as mispronunciation
                    foreach (var word in finalWords)
                    {
                        if (word.AccuracyScore < 60)
                        { 
                            word.ErrorType = "Mispronunciation";
                        }
                    }
                    // We can calculate whole accuracy by averaging
                    var filteredWords = finalWords.Where(item => item.ErrorType != "Insertion");
                    var accuracyScore = filteredWords.Sum(item => item.AccuracyScore) / filteredWords.Count();

                    // Recalculate the prosody score by averaging
                    var prosodyScore = prosody_scores.Average();

                    // Recalculate fluency score
                    var durations_sum = finalWords.Where(item => item.ErrorType == "None")
                        .Sum(item => item.Duration);

                    var fluencyScore = durations_sum * 1.0 / (endOffset - startOffset) * 100;

                    // Calculate whole completeness score
                    var completenessScore = 0.0;
                    if (!unscriptedScenario)
                    {
                        completenessScore = (double)finalWords.Count(item => item.ErrorType == "None") / filteredWords.Count() * 100;
                        completenessScore = completenessScore <= 100 ? completenessScore : 100;
                    }
                    else
                    {
                        completenessScore = 100.0;
                    }

                    List<double> scores_list;
                    double pronunciationScore = 0.0;
                    if (!unscriptedScenario)
                    {
                        // Scripted scenario
                        if (enableProsodyAssessment && !double.IsNaN(prosodyScore))
                        {
                            scores_list = new List<double> { accuracyScore, prosodyScore, completenessScore, fluencyScore };
                            pronunciationScore = scores_list.Sum(n => n * 0.2) + scores_list.Min() * 0.2;
                        }
                        else
                        {
                            scores_list = new List<double> { accuracyScore, completenessScore, fluencyScore };
                            pronunciationScore = scores_list.Sum(n => n * 0.2) + scores_list.Min() * 0.4;
                        }
                    }
                    else
                    {
                        // Unscripted scenario
                        if (enableProsodyAssessment && !double.IsNaN(prosodyScore))
                        {
                            scores_list = new List<double> { accuracyScore, prosodyScore, fluencyScore };
                            pronunciationScore = scores_list.Sum(n => n * 0.2) + scores_list.Min() * 0.4;
                        }
                        else
                        {
                            scores_list = new List<double> { accuracyScore, fluencyScore };
                            pronunciationScore = scores_list.Sum(n => n * 0.4) + scores_list.Min() * 0.2;
                        }
                    }

                    Console.WriteLine("Paragraph accuracy score: {0:0}, prosody score: {1:0} completeness score: {2:0}, fluency score: {3:0}, pronunciation score: {4:0}", accuracyScore, prosodyScore, completenessScore, fluencyScore, pronunciationScore);

                    for (int idx = 0; idx < finalWords.Count(); idx++)
                    {
                        Word word = finalWords[idx];
                        Console.WriteLine("{0}: word: {1}\taccuracy score: {2}\terror type: {3:0}",
                            idx + 1, word.WordText, word.AccuracyScore, word.ErrorType);
                    }
                }
            }
        }

        // Pronunciation assessment with Microsoft Audio Stack (MAS) enabled
        // See more information at https://aka.ms/csspeech/pa
        public static async Task PronunciationAssessmentWithMas()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Replace the language with your language in BCP-47 format, e.g., en-US.
            var language = "en-US";

            // Creates an instance of audio processing options with the default settings
            var audioProcessingOptions = AudioProcessingOptions.Create(
                AudioProcessingConstants.AUDIO_INPUT_PROCESSING_DISABLE_ECHO_CANCELLATION |
                AudioProcessingConstants.AUDIO_INPUT_PROCESSING_ENABLE_DEFAULT,
                PresetMicrophoneArrayGeometry.Mono);

            // Creates an instance of audio config from an audio file
            var audioConfig = AudioConfig.FromWavFileInput(@"whatstheweatherlike.wav", audioProcessingOptions);

            var referenceText = "what's the weather like";

            // Create pronunciation assessment config, set grading system, granularity and if enable miscue based on your requirement.
            var pronunciationConfig = new PronunciationAssessmentConfig(referenceText, GradingSystem.HundredMark, Granularity.Phoneme, enableMiscue: true);

            // Enable prosody assessment
            pronunciationConfig.EnableProsodyAssessment();

            // Creates a speech recognizer for the specified language
            using (var recognizer = new SpeechRecognizer(config, language, audioConfig))
            {
                // Starts recognizing.
                pronunciationConfig.ApplyTo(recognizer);

                // Starts speech recognition, and returns after a single utterance is recognized.
                // For long-running multi-utterance recognition, use StartContinuousRecognitionAsync() instead.
                var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);

                // Checks result.
                if (result.Reason == ResultReason.RecognizedSpeech)
                {
                    Console.WriteLine($"RECOGNIZED: Text={result.Text}");
                    Console.WriteLine("  PRONUNCIATION ASSESSMENT RESULTS:");

                    var pronunciationResult = PronunciationAssessmentResult.FromResult(result);
                    Console.WriteLine(
                        $"    Accuracy score: {pronunciationResult.AccuracyScore}, Prosody Score: {pronunciationResult.ProsodyScore}, Pronunciation score: {pronunciationResult.PronunciationScore}, Completeness score : {pronunciationResult.CompletenessScore}, FluencyScore: {pronunciationResult.FluencyScore}");

                    Console.WriteLine("  Word-level details:");

                    foreach (var word in pronunciationResult.Words)
                    {
                        Console.WriteLine($"    Word: {word.Word}, Accuracy score: {word.AccuracyScore}, Error type: {word.ErrorType}.");
                    }
                }
                else if (result.Reason == ResultReason.NoMatch)
                {
                    Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                }
                else if (result.Reason == ResultReason.Canceled)
                {
                    var cancellation = CancellationDetails.FromResult(result);
                    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                    if (cancellation.Reason == CancellationReason.Error)
                    {
                        Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                        Console.WriteLine($"CANCELED: ErrorDetails={cancellation.ErrorDetails}");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                }
            }
        }

        /// <summary>
        /// Performs pronunciation assessment asynchronously with REST API for a short audio file.
        /// See more information at https://learn.microsoft.com/azure/ai-services/speech-service/rest-speech-to-text-short
        /// </summary>
        /// <returns></returns>
        public static async Task PronunciationAssessmentWithRestApi()
        {
            // Replace with your own region and subscription key.
            var serviceRegion = "YourServiceRegion";
            var speechKey = "YourSubscriptionKey";

            string locale = "en-US";
            string referenceText = "Good morning.";
            var audioFilePath = "good_morning.pcm";
            bool enableProsodyAssessment = true;
            string phonemeAlphabet = "SAPI"; // IPA or SAPI
            bool enableMiscue = true;
            int nbestPhonemeCount = 5;

            // Build Pronunciation Assessment parameters
            var pronAssessmentParams = new Dictionary<string, object>
            {
                { "GradingSystem", "HundredMark" },
                { "Dimension", "Comprehensive" },
                { "ReferenceText", referenceText },
                { "EnableProsodyAssessment", enableProsodyAssessment },
                { "PhonemeAlphabet", phonemeAlphabet },
                { "EnableMiscue", enableMiscue },
                { "NBestPhonemeCount", nbestPhonemeCount }
            };

            string pronAssessmentJson = JsonSerializer.Serialize(pronAssessmentParams);
            string pronAssessmentBase64 = Convert.ToBase64String(Encoding.UTF8.GetBytes(pronAssessmentJson));

            // Build Session ID
            string sessionId = Guid.NewGuid().ToString("N");

            // Build request URL
            string url = $"https://{serviceRegion}.stt.speech.microsoft.com/speech/recognition/conversation/cognitiveservices/v1" +
                         $"?format=detailed&language={locale}&X-ConnectionId={sessionId}";

            DateTime uploadFinishTime = DateTime.UtcNow;
            using (var httpClient = new HttpClient())
            {
                using (var audioFile = new FileStream(audioFilePath, FileMode.Open, FileAccess.Read))
                using (var content = new PushStreamContent(async (stream, httpContent, transportContext) =>
                {
                    byte[] buffer = new byte[1024]; // 1KB per chunk
                    int read;
                    while ((read = await audioFile.ReadAsync(buffer, 0, buffer.Length)) > 0)
                    {
                        await stream.WriteAsync(buffer, 0, read);
                        await Task.Delay(read / 32);
                    }

                    // Record upload finish time when the last chunk is written
                    uploadFinishTime = DateTime.UtcNow;
                    stream.Close();
                }))
                {
                    // Set Content-Type for PCM WAV audio
                    content.Headers.TryAddWithoutValidation(
                            "Content-Type",
                            "audio/wav; codecs=audio/pcm; samplerate=16000"
                        );

                    // Add required headers
                    content.Headers.Add("Ocp-Apim-Subscription-Key", speechKey);
                    content.Headers.Add("Pronunciation-Assessment", pronAssessmentBase64);

                    // Accept JSON response
                    httpClient.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue("application/json"));

                    Console.WriteLine($"II URL: {url}");
                    Console.WriteLine($"II Config: {pronAssessmentJson}");
                    Console.WriteLine($"II Session ID: {sessionId}");

                    // Send request
                    var response = await httpClient.PostAsync(url, content);
                    var getResponseTime = DateTime.UtcNow;

                    if (!response.IsSuccessStatusCode)
                    {
                        Console.WriteLine($"EE Error code: {response.StatusCode}");
                        string errorMsg = await response.Content.ReadAsStringAsync();
                        Console.WriteLine($"EE Error message: {errorMsg}");
                        return;
                    }

                    string responseContent = await response.Content.ReadAsStringAsync();
                    Console.WriteLine($"II Response: {responseContent}");

                    var latency = getResponseTime - uploadFinishTime;
                    Console.WriteLine($"II Latency: {(int)latency.TotalMilliseconds}ms");
                }
            }
        }

        /// <summary>
        /// Aligns tokens from the raw list to the reference list by merging or splitting tokens
        /// </summary>
        /// <param name="rawList">The list of tokens as originally segmented</param>
        /// <param name="refList">The target list of reference tokens that should guide the alignment process.</param>
        /// <returns>A new list of strings representing the raw tokens realigned so that they are consistent with the reference list</returns>
        public static List<string> AlignRawTokensByRef(List<string> rawList, List<string> refList)
        {
            int refIdx = 0;
            int rawIdx = 0;
            int refLen = refList.Count;
            List<string> alignedRaw = new List<string>();

            // Make a copy to avoid modifying the original list
            List<string> rawCopy = new List<string>(rawList);

            while (rawIdx < rawCopy.Count && refIdx < refLen)
            {
                bool mergedSplitDone = false;
                for (int length = 1; length <= rawCopy.Count - rawIdx; length++)
                {
                    if (rawIdx + length > rawCopy.Count)
                        break;

                    // Merge consecutive tokens
                    string mergedRaw = string.Join("", rawCopy.GetRange(rawIdx, length));
                    string refWord = refList[refIdx];

                    if (mergedRaw.Contains(refWord))
                    {
                        string[] parts = mergedRaw.Split(new string[] { refWord }, 2, StringSplitOptions.None);

                        // Handle prefix before refWord
                        if (!string.IsNullOrEmpty(parts[0]))
                        {
                            int consumed = 0;
                            int tokenIdx = rawIdx;

                            // Consume characters from rawCopy until we cover all of parts[0]
                            while (consumed < parts[0].Length && tokenIdx < rawCopy.Count)
                            {
                                string token = rawCopy[tokenIdx];
                                int remain = parts[0].Length - consumed;

                                if (token.Length <= remain)
                                {
                                    // Add it directly to aligned result if the whole token is fully within prefix
                                    alignedRaw.Add(token);
                                    consumed += token.Length;
                                    tokenIdx++;
                                }
                                else
                                {
                                    // Add only the prefix part, the rest will be handled later
                                    string prefixPart = token.Substring(0, remain);
                                    alignedRaw.Add(prefixPart);

                                    consumed += remain;
                                }
                            }
                        }

                        // Append the matched refWord
                        alignedRaw.Add(refWord);

                        // Handle suffix after refWord
                        if (!string.IsNullOrEmpty(parts[1]))
                        {
                            rawCopy[rawIdx] = parts[1];
                            // Remove extra merged tokens
                            for (int i = 1; i < length; i++)
                            {
                                rawCopy.RemoveAt(rawIdx + 1);
                            }
                        }
                        else
                        {
                            // No suffix: remove all merged tokens
                            for (int i = 0; i < length; i++)
                            {
                                rawCopy.RemoveAt(rawIdx);
                            }
                        }

                        refIdx++;
                        mergedSplitDone = true;
                    }

                    if (mergedSplitDone)
                        break;
                }

                if (!mergedSplitDone)
                {
                    refIdx++;
                }
            }

            // Append any remaining raw tokens
            while (rawIdx < rawCopy.Count)
            {
                alignedRaw.Add(rawCopy[rawIdx]);
                rawIdx++;
            }

            return alignedRaw;
        }


        /// <summary>
        /// Aligns two token lists (raw and reference) using inline diff analysis (via DiffPlex)
        /// </summary>
        /// <param name="rawWords">The original list of tokens</param>
        /// <param name="refWords">The target list of reference tokens</param>
        /// <returns>A new list of tokens, adjusted from the raw list</returns>
        public static List<string> AlignListsWithInlineDiff(List<string> rawWords, List<string> refWords)
        {

            List<string> alignedWords = new List<string>();

            string oldText = string.Join("\n", rawWords);
            string newText = string.Join("\n", refWords);

            var differ = new Differ();
            var inlineBuilder = new InlineDiffBuilder(differ);
            var diffModel = inlineBuilder.BuildDiffModel(oldText, newText);

            var lines = diffModel.Lines.ToList();
            int index = 0;

            while (index < lines.Count)
            {
                var line = lines[index];

                switch (line.Type)
                {
                    case ChangeType.Unchanged:
                        alignedWords.Add(line.Text);
                        index++;
                        break;

                    case ChangeType.Inserted:
                        index++;
                        break;

                    case ChangeType.Deleted:
                        var deletedList = new List<string>();
                        var insertedList = new List<string>();

                        while (index < lines.Count)
                        {
                            if (lines[index].Type == ChangeType.Deleted)
                            {
                                deletedList.Add(lines[index].Text);
                                index++;
                            }
                            else if (lines[index].Type == ChangeType.Inserted)
                            {
                                insertedList.Add(lines[index].Text);
                                index++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        if (insertedList.Count > 0)
                        {
                            var alignedRaw = AlignRawTokensByRef(deletedList, insertedList);
                            alignedWords.AddRange(alignedRaw);
                        }
                        else
                        {
                            alignedWords.AddRange(deletedList);
                        }
                        break;
                }
            }
            return alignedWords;
        }
    }

    public class Word
    {
        public string WordText { get; set; }
        public string ErrorType { get; set; }
        public double AccuracyScore { get; set; }
        public double Duration { get; set; }

        public Word(string wordText, string errorType)
        {
            WordText = wordText;
            ErrorType = errorType;
            AccuracyScore = 0;
            Duration = 0;
        }

        public Word(string wordText, string errorType, double accuracyScore) : this(wordText, errorType)
        {
            AccuracyScore = accuracyScore;
        }

        public Word(string wordText, string errorType, double accuracyScore, double duration) : this(wordText, errorType, accuracyScore)
        {
            Duration = duration;
        }
    }

}
