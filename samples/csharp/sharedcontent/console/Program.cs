//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace MicrosoftSpeechSDKSamples
{
    class Program
    {
        private static readonly string choose = " Please choose one of the following samples:";
        private static readonly string mainPrompt = " Your choice (or 0 to exit): ";
        private static readonly string prompt = " Your choice (or 0 for main menu): ";
        private static readonly string exiting = "\n Exiting...";
        private static readonly string back = "\n Back to main menu.";
        private static readonly string invalid = "\n Invalid input, choose again.";
        private static readonly string done = "\n Done!";

        static void Main(string[] args)
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Speech recognition samples.");
                Console.WriteLine(" 2. Speech translation samples.");
                Console.WriteLine(" 3. Intent recognition samples.");
                Console.WriteLine(" 4. Speech synthesis samples.");
                Console.WriteLine(" 5. Meeting transcriber samples.");
                Console.WriteLine(" 6. Speech recognition with language detection enabled samples");
                Console.WriteLine(" 7. Standalone language detection samples.");
                Console.WriteLine(" 8. Speech recognition with Microsoft Audio Stack (MAS) samples.");
                Console.WriteLine(" 9. Diagnostics logging samples (trace logging).");
                Console.WriteLine(" a. Pronunciation assessment samples.");
                Console.WriteLine("");
                Console.Write(mainPrompt);

                x = Console.ReadKey();
                Console.WriteLine("");

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechRecognition();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechTranslation();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        IntentRecognition();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        SpeechSynthesis();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        MeetingTranscriber();
                        break;
                    case ConsoleKey.D6:
                    case ConsoleKey.NumPad6:
                        SpeechRecognitionWithLanguageDetectionEnabled();
                        break;
                    case ConsoleKey.D7:
                    case ConsoleKey.NumPad7:
                        LanguageDetectionSamples();
                        break;
                    case ConsoleKey.D8:
                    case ConsoleKey.NumPad8:
                        SpeechRecognitionWithMASEnabled();
                        break;
                    case ConsoleKey.D9:
                    case ConsoleKey.NumPad9:
                        SpeechDiagnosticsLogging();
                        break;
                    case ConsoleKey.A:
                        PronunciationAssessment();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(exiting);
                        break;
                    default:
                        Console.WriteLine(invalid);
                        break;
                }

            } while (x.Key != ConsoleKey.D0);
        }

        /// <summary>
        /// Speech recognition Sample.
        /// </summary>
        private static void SpeechRecognition()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Speech Recognition Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Speech recognition with microphone input.");
                Console.WriteLine(" 2. Speech recognition in the specified language and using detailed output format.");
                Console.WriteLine(" 3. Speech continuous recognition with file input.");
                Console.WriteLine(" 4. Speech recognition using customized model.");
                Console.WriteLine(" 5. Speech recognition with pull audio stream.");
                Console.WriteLine(" 6. Speech recognition with push audio stream.");
                Console.WriteLine(" 7. Speech recognition with keyword spotting.");
                Console.WriteLine(" 8. Speech continuous recognition using authorization token.");
                Console.WriteLine(" 9. Speech Recognition using Phrase Lists");
                Console.WriteLine(" a. Speech recognition with compressed input pull audio stream.");
                Console.WriteLine(" b. Speech recognition with compressed input push audio stream.");
                Console.WriteLine(" c. Keyword recognizer.");
                Console.WriteLine(" d. Speech recognition with file input and a switch to secondary region.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechRecognitionSamples.RecognitionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechRecognitionSamples.RecognitionWithLanguageAndDetailedOutputAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        SpeechRecognitionSamples.ContinuousRecognitionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        SpeechRecognitionSamples.RecognitionUsingCustomizedModelAsync().Wait();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        SpeechRecognitionSamples.RecognitionWithPullAudioStreamAsync().Wait();
                        break;
                    case ConsoleKey.D6:
                    case ConsoleKey.NumPad6:
                        SpeechRecognitionSamples.RecognitionWithPushAudioStreamAsync().Wait();
                        break;
                    case ConsoleKey.D7:
                    case ConsoleKey.NumPad7:
                        SpeechRecognitionSamples.ContinuousRecognitionWithKeywordSpottingAsync().Wait();
                        break;
                    case ConsoleKey.D8:
                    case ConsoleKey.NumPad8:
                        SpeechRecognitionWithTokenSample.ContinuousRecognitionWithAuthorizationTokenAsync().Wait();
                        break;
                    case ConsoleKey.D9:
                    case ConsoleKey.NumPad9:
                        SpeechRecognitionSamples.ContinuousRecognitionWithFileAndPhraseListsAsync().Wait();
                        break;
                    case ConsoleKey.A:
                        SpeechRecognitionSamples.SpeechRecognitionWithCompressedInputPullStreamAudio().Wait();
                        break;
                    case ConsoleKey.B:
                        SpeechRecognitionSamples.SpeechRecognitionWithCompressedInputPushStreamAudio().Wait();
                        break;
                    case ConsoleKey.C:
                        SpeechRecognitionSamples.KeywordRecognizer().Wait();
                        break;
                    case ConsoleKey.D:
                        SpeechRecognitionSamples.RecognitionOnceWithFileAsyncSwitchSecondaryRegion().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        //
        // Speech synthesis samples
        //
        private static void SpeechSynthesis()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Speech Synthesis Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Speech synthesis to speaker output.");
                Console.WriteLine(" 2. Speech synthesis with specified language.");
                Console.WriteLine(" 3. Speech synthesis with specified voice.");
                Console.WriteLine(" 4. Speech synthesis to wave file.");
                Console.WriteLine(" 5. Speech synthesis to mp3 file.");
                Console.WriteLine(" 6. Speech synthesis to pull audio output stream.");
                Console.WriteLine(" 7. Speech synthesis to push audio output stream.");
                Console.WriteLine(" 8. Speech synthesis to result.");
                Console.WriteLine(" 9. Speech synthesis to audio data stream.");
                Console.WriteLine(" a. Speech synthesis events.");
                Console.WriteLine(" b. Speech synthesis with word boundary event.");
                Console.WriteLine(" c. Speech synthesis with viseme event.");
                Console.WriteLine(" d. Speech synthesis with bookmark event.");
                Console.WriteLine(" e. Speech synthesis using authorization token.");
                Console.WriteLine(" f. Speech synthesis in server scenario.");
                Console.WriteLine(" g. Speech synthesis with source language auto detection.");
                Console.WriteLine(" h. Speech synthesis using custom voice.");
                Console.WriteLine(" i. Get available speech synthesis voices.");
                Console.WriteLine(" j. Speech synthesis to speaker output with fallback to secondary region.");
                Console.WriteLine(" k. Speech synthesis to speaker output with custom voice and fallback to standard voice.");
                Console.WriteLine(" l. Speech synthesis to speaker output with custom voice and fallback to secondary region.");
                Console.WriteLine(" m. Speech synthesis from a long text file into mp3 file");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechSynthesisSamples.SynthesisToSpeakerAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechSynthesisSamples.SynthesisWithLanguageAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        SpeechSynthesisSamples.SynthesisWithVoiceAsync().Wait();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        SpeechSynthesisSamples.SynthesisToWaveFileAsync().Wait();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        SpeechSynthesisSamples.SynthesisToMp3FileAsync().Wait();
                        break;
                    case ConsoleKey.D6:
                    case ConsoleKey.NumPad6:
                        SpeechSynthesisSamples.SynthesisToPullAudioOutputStreamAsync().Wait();
                        break;
                    case ConsoleKey.D7:
                    case ConsoleKey.NumPad7:
                        SpeechSynthesisSamples.SynthesisToPushAudioOutputStreamAsync().Wait();
                        break;
                    case ConsoleKey.D8:
                    case ConsoleKey.NumPad8:
                        SpeechSynthesisSamples.SynthesisToResultAsync().Wait();
                        break;
                    case ConsoleKey.D9:
                    case ConsoleKey.NumPad9:
                        SpeechSynthesisSamples.SynthesisToAudioDataStreamAsync().Wait();
                        break;
                    case ConsoleKey.A:
                        SpeechSynthesisSamples.SynthesisEventsAsync().Wait();
                        break;
                    case ConsoleKey.B:
                        SpeechSynthesisSamples.SynthesisWordBoundaryEventAsync().Wait();
                        break;
                    case ConsoleKey.C:
                        SpeechSynthesisSamples.SynthesisVisemeEventAsync().Wait();
                        break;
                    case ConsoleKey.D:
                        SpeechSynthesisSamples.SynthesisBookmarkEventAsync().Wait();
                        break;
                    case ConsoleKey.E:
                        SpeechSynthesisWithTokenSample.SpeechSynthesisWithAuthorizationTokenAsync().Wait();
                        break;
                    case ConsoleKey.F:
                        SpeechSynthesisServerScenarioSample.SpeechSynthesizeWithPool();
                        break;
                    case ConsoleKey.G:
                        SpeechSynthesisSamples.SynthesisWithAutoDetectSourceLanguageAsync().Wait();
                        break;
                    case ConsoleKey.H:
                        SpeechSynthesisSamples.SynthesisUsingCustomVoiceAsync().Wait();
                        break;
                    case ConsoleKey.I:
                        SpeechSynthesisSamples.SynthesisGetAvailableVoicesAsync().Wait();
                        break;
                    case ConsoleKey.J:
                        SpeechSynthesisSamples.SynthesizeOnceToSpeakerAsyncSwitchSecondaryRegion().Wait();
                        break;
                    case ConsoleKey.K:
                        SpeechSynthesisSamples.SynthesizeOnceUseCustomVoiceToSpeakerAsyncSwitchPlatformVoice().Wait();
                        break;
                    case ConsoleKey.L:
                        SpeechSynthesisSamples.SynthesizeOnceUseCustomVoiceToSpeakerAsyncSwitchSecondaryRegion().Wait();
                        break;
                    case ConsoleKey.M:
                        SpeechSynthesisSamples.SynthesisFileToMp3FileAsync().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        private static void SpeechTranslation()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Speech Translation Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Translation with microphone input.");
                Console.WriteLine(" 2. Translation with file input.");
                Console.WriteLine(" 3. Translation with audio stream.");
                Console.WriteLine(" 4. Translation with compressed input push audio stream.");
                Console.WriteLine(" 5. Translation with language detection samples.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        TranslationSamples.TranslationWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        TranslationSamples.TranslationWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        TranslationSamples.TranslationWithAudioStreamAsync().Wait();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        TranslationSamples.TranslationWithFileCompressedInputAsync().Wait();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        TranslationWithLanguageDetectionEnabled();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        private static void IntentRecognition()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Intent Recognition Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Intent recognition with microphone input.");
                Console.WriteLine(" 2. Intent continuous recognition with file input.");
                Console.WriteLine(" 3. Intent recognition in the specified language with microphone input.");
                Console.WriteLine(" 4. Intent recognition from default microphone and pattern matching.");
                Console.WriteLine(" 5. Intent recognition with keyword spotting from default microphone and pattern matching.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        IntentRecognitionSamples.RecognitionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        IntentRecognitionSamples.ContinuousRecognitionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        IntentRecognitionSamples.RecognitionWithMicrophoneUsingLanguageAsync().Wait();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        IntentRecognitionSamples.IntentPatternMatchingWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        IntentRecognitionSamples.IntentPatternMatchingWithMicrophoneAndKeywordSpottingAsync().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        private static void MeetingTranscriber()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Meeting Transcriber Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Remote Meeting for an async job.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        RemoteMeetingSamples.RemoteMeetingWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        //
        // Speech Recognition with Language Detection feature enabled
        //
        private static void SpeechRecognitionWithLanguageDetectionEnabled()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Speech Recognition With Spoken Language Detection Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Single utterance recognition with microphone input.");
                Console.WriteLine(" 2. Continuous recognition from multi-lingual input WAV file.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechRecognitionWithLanguageIdSamples.RecognitionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechRecognitionWithLanguageIdSamples.MultiLingualRecognitionWithUniversalV2Endpoint().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        //
        // Standalone Language Detection (SourceLanguageRecognizer) samples
        //
        private static void LanguageDetectionSamples()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Stand-alone Spoken Language Detection Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Single utterance language detection, with microphone input.");
                Console.WriteLine(" 2. Single utterance language detection, with WAV file input.");
                Console.WriteLine(" 3. Continuous language detection with multi-lingual WAV file input.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        StandaloneLanguageDetectionSamples.LanguageDetectionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        StandaloneLanguageDetectionSamples.LanguageDetectionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        StandaloneLanguageDetectionSamples.ContinuousLanguageDetectionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        //
        // Speech Translation with language detection enabled
        //
        private static void TranslationWithLanguageDetectionEnabled()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Speech Translation with Spoken Language Detection Samples:");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Translation with microphone input.");
                Console.WriteLine(" 2. Translation with multi-lingual audio input.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        TranslationSamples.TranslationWithMicrophoneAsync_withLanguageDetectionEnabled().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        TranslationSamples.TranslationWithMultiLingualFileAsync_withLanguageDetectionEnabled().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        //
        // Speech recognition with Microsoft Audio Stack (MAS) enabled
        //
        private static void SpeechRecognitionWithMASEnabled()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Speech Recognition with Microsoft Audio Stack (MAS) Samples");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Speech recognition from default microphone.");
                Console.WriteLine(" 2. Speech recognition from a microphone with pre-defined microphone array geometry specified.");
                Console.WriteLine(" 3. Speech recognition from multi-channel file with custom microphone array geometry specified.");
                Console.WriteLine(" 4. Speech recognition from pull stream with custom set of enhancements enabled.");
                Console.WriteLine(" 5. Speech recognition from push stream with beamforming angles specified.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechRecognitionSamples.ContinuousRecognitionFromDefaultMicrophoneWithMASEnabled().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechRecognitionSamples.RecognitionFromMicrophoneWithMASEnabledAndPresetGeometrySpecified().Wait();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        SpeechRecognitionSamples.ContinuousRecognitionFromMultiChannelFileWithMASEnabledAndCustomGeometrySpecified().Wait();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        SpeechRecognitionSamples.RecognitionFromPullStreamWithSelectMASEnhancementsEnabled().Wait();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        SpeechRecognitionSamples.ContinuousRecognitionFromPushStreamWithMASEnabledAndBeamformingAnglesSpecified().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        //
        // Shows how to enabled Speech SDK trace logging in different ways. Microsoft may ask you to collect logs
        // in order to investigate an issue you reported
        //
        private static void SpeechDiagnosticsLogging()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Diagnostics logging samples (trace logging)");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Logging to file (without a filter)");
                Console.WriteLine(" 2. Logging to file (with filter)");
                Console.WriteLine(" 3. Subscribe to logging event (without a filter)");
                Console.WriteLine(" 4. Subscribe to logging event (with filter)");
                Console.WriteLine(" 5. Logging to memory buffer (with or without filter)");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechDiagnosticsLoggingSamples.FileLoggerWithoutFilter();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechDiagnosticsLoggingSamples.FileLoggerWithFilter();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        SpeechDiagnosticsLoggingSamples.EventLoggerWithoutFilter();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        SpeechDiagnosticsLoggingSamples.EventLoggerWithFilter();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        SpeechDiagnosticsLoggingSamples.MemoryLoggerWithOrWithoutFilter();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }

        //
        // Shows pronunciation assessment samples.
        // See more information at https://aka.ms/csspeech/pa
        //
        private static void PronunciationAssessment()
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Pronunciation Assessment");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Pronunciation assessment with microphone input.");
                Console.WriteLine(" 2. Pronunciation assessment with stream input.");
                Console.WriteLine(" 3. Pronunciation assessment configured with json.");
                Console.WriteLine(" 4. Pronunciation assessment continuous with file.");
                Console.WriteLine(" 5. Pronunciation assessment with content assessment.");
                Console.WriteLine("");
                Console.Write(prompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechRecognitionSamples.PronunciationAssessmentWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechRecognitionSamples.PronunciationAssessmentWithStream();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        SpeechRecognitionSamples.PronunciationAssessmentConfiguredWithJson().Wait();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        SpeechRecognitionSamples.PronunciationAssessmentContinuousWithFile().Wait();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        SpeechRecognitionSamples.PronunciationAssessmentWithContentAssessment().Wait();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(back);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0);
        }
    }
}
