//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;


namespace MicrosoftSpeechSDKSamples
{
    public class SpeechRecognitionSamples
    {
        // List available embedded speech recognition models.
        public static void ListEmbeddedModels()
        {
            // Creates an instance of an embedded speech config.
            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            if (speechConfig == null)
            {
                return;
            }

            // Gets a list of models.
            var models = speechConfig.GetSpeechRecognitionModels();

            if (models.Count > 0)
            {
                Console.WriteLine("Models found:");
                foreach (var model in models)
                {
                    Console.WriteLine(model.Name);
                    Console.Write(" Locale(s): ");
                    foreach (var locale in model.Locales)
                    {
                        Console.Write($"{locale} ");
                    }
                    Console.WriteLine();
                    Console.WriteLine($" Path:      {model.Path}");
                }

                // To find a model that supports a specific locale, for example:
                /*
                string modelName = null;
                string modelLocale = "en-US";

                foreach (var model in models)
                {
                    if (model.Locales[0].Equals(modelLocale))
                    {
                        modelName = model.Name;
                        break;
                    }
                }

                if (!string.IsNullOrEmpty(modelName))
                {
                    Console.WriteLine($"Found {modelLocale} model: {modelName}");
                }
                */
            }
            else
            {
                Console.Error.WriteLine("No models found. Either the path is not valid or the format of model(s) is unknown.");
            }
        }


        private static async Task RecognizeSpeechAsync(SpeechRecognizer recognizer, bool useKeyword, bool waitForUser)
        {
            var recognitionEnd = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Subscribes to events.
            recognizer.Recognizing += (s, e) =>
            {
                // Intermediate result (hypothesis).
                if (e.Result.Reason == ResultReason.RecognizingSpeech)
                {
                    Console.WriteLine($"Recognizing:{e.Result.Text}");
                }
                else if (e.Result.Reason == ResultReason.RecognizingKeyword)
                {
                    // ignored
                }
            };

            recognizer.Recognized += (s, e) =>
            {
                if (e.Result.Reason == ResultReason.RecognizedKeyword)
                {
                    // Keyword detected, speech recognition will start.
                    Console.WriteLine($"KEYWORD: Text={e.Result.Text}");
                }
                else if (e.Result.Reason == ResultReason.RecognizedSpeech)
                {
                    // Final result. May differ from the last intermediate result.
                    Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");

                    // See where the result came from, cloud (online) or embedded (offline)
                    // speech recognition.
                    // This can change during a session where HybridSpeechConfig is used.
                    /*
                    Console.WriteLine($"Recognition backend: {e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_RecognitionBackend)}");
                    */

                    // Recognition results in JSON format.
                    //
                    // Offset and duration values are in ticks, where a single tick
                    // represents 100 nanoseconds or one ten-millionth of a second.
                    //
                    // To get word level detail, set the output format to detailed.
                    // See EmbeddedRecognitionFromWavFile() in this source file for
                    // a configuration example.
                    //
                    // If an embedded speech recognition model does not support word
                    // timing, the word offset and duration values are always 0, and the
                    // phrase offset and duration only indicate a time window inside of
                    // which the phrase appeared, not the accurate start and end of speech.
                    /*
                    string jsonResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                    Console.WriteLine($"JSON result: {jsonResult}");
                    */
                    // Word level detail (if enabled in speech config) can be also
                    // read directly from the result.
                    /*
                    var detailedResults = e.Result.Best();
                    foreach (var item in detailedResults)
                    {
                        foreach (var word in item.Words)
                        {
                            Console.WriteLine($"Word: \"{word.Word}\" | Offset: {word.Offset / 10000}ms | Duration: {word.Duration / 10000}ms");
                        }
                    }
                    */
                }
                else if (e.Result.Reason == ResultReason.NoMatch)
                {
                    // NoMatch occurs when no speech was recognized.
                    var reason = NoMatchDetails.FromResult(e.Result).Reason;
                    Console.WriteLine($"NO MATCH: Reason={reason}");
                }
            };

            recognizer.Canceled += (s, e) =>
            {
                Console.WriteLine($"CANCELED: Reason={e.Reason}");

                if (e.Reason == CancellationReason.Error)
                {
                    // NOTE: In case of an error, do not use the same recognizer for recognition anymore.
                    Console.Error.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                    Console.Error.WriteLine($"CANCELED: ErrorDetails=\"{e.ErrorDetails}\"");
                }
            };

            recognizer.SessionStarted += (s, e) =>
            {
                Console.WriteLine("Session started.");
            };

            recognizer.SessionStopped += (s, e) =>
            {
                Console.WriteLine("Session stopped.");
                recognitionEnd.TrySetResult(0);
            };

            if (useKeyword)
            {
                // Creates an instance of a keyword recognition model.
                var keywordModel = KeywordRecognitionModel.FromFile(Settings.GetKeywordModelFileName());

                // Starts the following routine:
                // 1. Listen for a keyword in input audio. There is no timeout.
                //    Speech that does not start with the keyword is ignored.
                // 2. If the keyword is spotted, start normal speech recognition.
                // 3. After a recognition result (that always includes at least
                //    the keyword), go back to step 1.
                // Steps 1-3 repeat until StopKeywordRecognitionAsync() is called.
                await recognizer.StartKeywordRecognitionAsync(keywordModel).ConfigureAwait(false);

                Console.WriteLine($"Say something starting with \"{Settings.GetKeywordPhrase()}\" (press Enter to stop)...");
                Console.ReadKey();

                // Stops recognition.
                await recognizer.StopKeywordRecognitionAsync().ConfigureAwait(false);
            }
            else
            {
                // The following lines run continuous recognition that listens for speech
                // in input audio and generates results until stopped. To run recognition
                // only once, replace this code block with
                //
                // var result = await recognizer.RecognizeOnceAsync().ConfigureAwait(false);
                //
                // and optionally check result.Reason and result.Text for the outcome
                // instead of doing it in event handlers.

                // Starts continuous recognition.
                await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                if (waitForUser)
                {
                    Console.WriteLine("Say something (press Enter to stop)...");
                    Console.ReadKey();
                }
                else
                {
                    Task.WaitAny(new[] { recognitionEnd.Task });
                }

                // Stops recognition.
                await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
            }
        }


        // Reads audio samples from the source and writes them into a push stream.
        // Push stream can be used when input audio is not generated faster than it
        // can be processed (i.e. the generation of input is the limiting factor).
        // The application determines the rate of input data transfer.
        private static void PushStreamInputReader(PushAudioInputStream pushStream)
        {
            try
            {
                // In this example the input stream is a file. Modify the code to use
                // a non-file source (e.g. some audio API that returns data) as needed.
                using var file = File.OpenRead(Settings.GetSpeechRawAudioFileName());
                using var input = new BinaryReader(file);
                byte[] buffer = new byte[3200]; // 100ms of 16kHz 16-bit mono audio

                while (true)
                {
                    // Read audio data from the input stream to a data buffer.
                    var bytesRead = input.Read(buffer, 0, buffer.Length);

                    // Copy audio data from the data buffer into a push stream
                    // for the Speech SDK to consume.
                    // Data must NOT include any headers, only audio samples.
                    pushStream.Write(buffer, bytesRead);

                    if (bytesRead == 0)
                    {
                        input.Close();
                        break;
                    }
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine($"PushStreamInputReader: {e}");
            }
            finally
            {
                pushStream.Close();
            }
        }


        // Implements a pull stream callback that reads audio samples from the source.
        // Pull stream should be used when input audio may be generated faster than
        // it can be processed (i.e. the processing of input is the limiting factor).
        // The Speech SDK determines the rate of input data transfer.
        public sealed class PullStreamInputReader : PullAudioInputStreamCallback
        {
            private BinaryReader _input;

            public PullStreamInputReader()
            {
                // In this example the input stream is a file. Modify the code to use
                // a non-file source (e.g. audio API that returns data) as necessary.
                _input = new BinaryReader(File.OpenRead(Settings.GetSpeechRawAudioFileName()));
            }

            // This method is called to synchronously get data (at most 'size' bytes)
            // from the input stream.
            // It returns the number of bytes copied into the data buffer.
            // If there is no data, the method must wait until data becomes available
            // or return 0 to indicate the end of stream.
            public override int Read(byte[] buffer, uint size)
            {
                // Copy audio data from the input stream into a data buffer for the
                // Speech SDK to consume.
                // Data must NOT include any headers, only audio samples.
                return _input.Read(buffer, 0, (int)size);
            }

            // This method is called for cleanup of resources.
            protected override void Dispose(bool disposing)
            {
                if (disposed)
                {
                    return;
                }

                if (disposing)
                {
                    _input.Dispose();
                }

                disposed = true;
                base.Dispose(disposing);
            }

            private bool disposed = false;
        }


        // Recognizes speech using embedded speech config and the system default microphone device.
        public static void EmbeddedRecognitionFromMicrophone()
        {
            var useKeyword = false;
            var waitForUser = true;

            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            using var audioConfig = AudioConfig.FromDefaultMicrophoneInput();

            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);
            RecognizeSpeechAsync(recognizer, useKeyword, waitForUser).Wait();
        }


        // Recognizes speech using embedded speech config and the system default microphone device.
        // Recognition is triggered with a keyword.
        public static void EmbeddedRecognitionWithKeywordFromMicrophone()
        {
            var useKeyword = true;
            var waitForUser = true;

            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            using var audioConfig = AudioConfig.FromDefaultMicrophoneInput();

            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);
            RecognizeSpeechAsync(recognizer, useKeyword, waitForUser).Wait();
        }


        // Recognizes speech using embedded speech config and a WAV file.
        public static void EmbeddedRecognitionFromWavFile()
        {
            var useKeyword = false;
            var waitForUser = false;

            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            using var audioConfig = AudioConfig.FromWavFileInput(Settings.GetSpeechWavAudioFileName());

            // Enables detailed results (from PropertyId.SpeechServiceResponse_JsonResult).
            // See the event handler of 'Recognized' event for how to read the JSON result.
            /*
            speechConfig.SpeechRecognitionOutputFormat = OutputFormat.Detailed;
            */

            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);
            RecognizeSpeechAsync(recognizer, useKeyword, waitForUser).Wait();
        }


        // Recognizes speech using embedded speech config and a push stream.
        public static void EmbeddedRecognitionFromPushStream()
        {
            var useKeyword = false;
            var waitForUser = false;

            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            using var audioFormat = AudioStreamFormat.GetWaveFormatPCM(Settings.GetEmbeddedSpeechSamplesPerSecond(), Settings.GetEmbeddedSpeechBitsPerSample(), Settings.GetEmbeddedSpeechChannels());
            using var pushStream = AudioInputStream.CreatePushStream(audioFormat);
            using var audioConfig = AudioConfig.FromStreamInput(pushStream);

            // Push data into the stream in another thread.
            var pushStreamThread = new Thread(() => PushStreamInputReader(pushStream));
            pushStreamThread.Start();

            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);
            RecognizeSpeechAsync(recognizer, useKeyword, waitForUser).Wait();

            pushStreamThread.Join();
        }


        // Recognizes speech using embedded speech config and a pull stream.
        public static void EmbeddedRecognitionFromPullStream()
        {
            var useKeyword = false;
            var waitForUser = false;

            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            using var audioFormat = AudioStreamFormat.GetWaveFormatPCM(Settings.GetEmbeddedSpeechSamplesPerSecond(), Settings.GetEmbeddedSpeechBitsPerSample(), Settings.GetEmbeddedSpeechChannels());
            using var pullStreamCallback = new PullStreamInputReader();
            using var pullStream = new PullAudioInputStream(pullStreamCallback, audioFormat);
            using var audioConfig = AudioConfig.FromStreamInput(pullStream);

            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);
            RecognizeSpeechAsync(recognizer, useKeyword, waitForUser).Wait();
        }


        // Recognizes speech using hybrid (cloud & embedded) speech config and the system default microphone device.
        public static void HybridRecognitionFromMicrophone()
        {
            var useKeyword = false;
            var waitForUser = true;

            var speechConfig = Settings.CreateHybridSpeechConfig();
            using var audioConfig = AudioConfig.FromDefaultMicrophoneInput();

            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);
            RecognizeSpeechAsync(recognizer, useKeyword, waitForUser).Wait();
        }


        // Measures the device performance when running embedded speech recognition.
        public static async Task EmbeddedSpeechRecognitionPerformanceTest()
        {
            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            using var audioConfig = AudioConfig.FromWavFileInput(Settings.GetPerfTestAudioFileName());

            // Enables performance metrics to be included with recognition results.
            speechConfig.SetProperty(PropertyId.EmbeddedSpeech_EnablePerformanceMetrics, "true");

            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);

            var recognitionEnd = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);
            int resultCount = 0;

            // Subscribes to events.
            recognizer.SpeechStartDetected += (s, e) =>
            {
                Console.WriteLine("Processing, please wait...");
            };

            recognizer.Recognized += (s, e) =>
            {
                if (e.Result.Reason == ResultReason.RecognizedSpeech)
                {
                    resultCount++;
                    Console.WriteLine($"[{resultCount}] RECOGNIZED: Text={e.Result.Text}");

                    // Recognition results in JSON format.
                    string jsonResult = e.Result.Properties.GetProperty(PropertyId.SpeechServiceResponse_JsonResult);
                    JObject json = JObject.Parse(jsonResult);

                    if (json.ContainsKey("PerformanceCounters"))
                    {
                        var perfCounters = json["PerformanceCounters"];
                        Console.WriteLine($"[{resultCount}] PerformanceCounters: {perfCounters.ToString(Formatting.Indented)}");
                    }
                    else
                    {
                        Console.Error.WriteLine("ERROR: No performance counters data found.");
                    }
                }
            };

            recognizer.Canceled += (s, e) =>
            {
                if (e.Reason == CancellationReason.Error)
                {
                    Console.Error.WriteLine($"CANCELED: ErrorCode={e.ErrorCode} ErrorDetails={e.ErrorDetails}");
                }
            };

            recognizer.SessionStopped += (s, e) =>
            {
                Console.WriteLine("All done! Please go to https://aka.ms/embedded-speech for information on how to evaluate the results.");
                recognitionEnd.TrySetResult(0);
            };

            // Runs continuous recognition.
            await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);
            Task.WaitAny(new[] { recognitionEnd.Task });
            await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
        }
    }
}
