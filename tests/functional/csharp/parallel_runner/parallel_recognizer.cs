//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Test.EndToEnd
{
    public sealed class ParallelRecognizer
    {
        private readonly SpeechConfig config;
        private readonly uint numberOfThreads;
        private bool isContinuous;
        private bool verbose;

        private class RecognitionResult
        {
            public string SessionId { get; set; }
            public List<string> Text { get; set; }
            public string Error { get; set; }
            public bool ErrorHappened { get; set; }
        }

        public ParallelRecognizer(Uri endpoint, string subscriptionKey, uint numberOfThreads, bool verbose)
        {
            this.numberOfThreads = numberOfThreads;
            this.config = SpeechConfig.FromEndpoint(endpoint, subscriptionKey);
            this.isContinuous = true;
            this.verbose = verbose;
        }

        public void Recognize(ConcurrentQueue<string> inputFiles, string outputFolder, bool isContinuous)
        {
            this.isContinuous = isContinuous;
            var threads = new List<Thread>();
            var result = new ConcurrentDictionary<string, RecognitionResult>();
            var startTime = DateTime.Now;
            for (int i = 0; i < Math.Min(numberOfThreads, inputFiles.Count); i++)
            {
                var t = new Thread(() => this.RecognizeFiles(inputFiles, result, inputFiles.Count, startTime));
                threads.Add(t);
                t.Start();
            }

            // Wait till completion.
            threads.ForEach(t => t.Join());

            // Write out results.
            var succeeded = result.ToList().Where(r => !r.Value.ErrorHappened);
            var failed = result.ToList().Where(r => r.Value.ErrorHappened);

            if (verbose)
            {
                Console.WriteLine($"Flushing output of {succeeded.Count()} utterances.");
            }

            using (var sw = new StreamWriter(Path.Combine(outputFolder, "output.txt")))
            {
                foreach (var r in succeeded)
                {
                    sw.WriteLine(string.Join("\t", Path.GetFileNameWithoutExtension(r.Key), string.Join(" ", r.Value.Text.ToArray())));
                }
            }

            if (verbose)
            {
                Console.WriteLine($"Flushing errors of {failed.Count()} utterances.");
            }

            using (var sw = new StreamWriter(Path.Combine(outputFolder, "err.txt")))
            {
                foreach (var r in failed)
                {
                    sw.WriteLine(string.Join("\t", Path.GetFileNameWithoutExtension(r.Key), r.Value.SessionId + ": " + r.Value.Error));
                }
            }
        }

        private void RecognizeFiles(ConcurrentQueue<string> files, ConcurrentDictionary<string, RecognitionResult> result, int totalCount, DateTime startTime)
        {
            int counter = 0;
            while (files.Count > 0)
            {
                string file;
                if (!files.TryDequeue(out file))
                {
                    continue;
                }

                if (verbose)
                {
                    Console.WriteLine($"Thread {Thread.CurrentThread.ManagedThreadId} recognizing file '{file}'");
                }

                RecognizeSingleFile(file, result);

                if (verbose)
                {
                    Console.WriteLine($"Thread {Thread.CurrentThread.ManagedThreadId} finished recognition of '{file}'");
                }

                if (counter % 10 == 0)
                {
                    var eta = DateTime.Now.AddSeconds(((DateTime.Now - startTime).TotalSeconds / (totalCount - files.Count)) * files.Count);
                    Console.WriteLine($"Thread {Thread.CurrentThread.ManagedThreadId} completed {counter} utterances, estimated finish time {eta.ToShortTimeString()}");
                }

                Console.Write(".");
                counter++;
            }
        }

        private void RecognizeSingleFile(string filePath, ConcurrentDictionary<string, RecognitionResult> result)
        {
            try
            {
                var transcribed = this.isContinuous
                    ? RecognizeSingleFileContinuous(filePath)
                    : RecognizeSingleFileSingleShot(filePath);

                if (!result.TryAdd(filePath, transcribed))
                {
                    throw new InvalidOperationException($"Could not add data '{transcribed.Text.ToString()}:{transcribed.Error}' to the dictionary");
                }
            }
            catch (Exception e)
            {
                if (!result.TryAdd(filePath, new RecognitionResult { Error = e.ToString() }))
                {
                    throw new InvalidOperationException($"Could not add data {e.ToString()} to the dictionary");
                }
            }
        }

        private RecognitionResult RecognizeSingleFileContinuous(string filePath)
        {
            var stopRecognition = new TaskCompletionSource<int>();
            var result = new RecognitionResult { Text = new List<string>() };
            try
            {
                using (var audioInput = AudioConfig.FromWavFileInput(filePath))
                {
                    using (var recognizer = new SpeechRecognizer(config, audioInput))
                    {
                        recognizer.SessionStarted += (s, e) =>
                        {
                            result.SessionId = e.SessionId;
                            Console.WriteLine($"Recognition session started for file '{filePath}' with session id {result.SessionId}");
                        };

                        recognizer.Recognized += (s, e) =>
                        {
                            if (e.Result.Reason == ResultReason.RecognizedSpeech)
                            {
                                if (verbose)
                                {
                                    Console.WriteLine($"{filePath}: offset {e.Result.OffsetInTicks}, {e.Result.Text}");
                                }

                                result.Text.Add(e.Result.Text);
                            }

                            else if (e.Result.Reason == ResultReason.Canceled)
                            {
                                var cancellation = CancellationDetails.FromResult(e.Result);
                                if (cancellation != null && cancellation.Reason == CancellationReason.Error)
                                {
                                    result.Error = cancellation.ToString();
                                    result.ErrorHappened = true;

                                    Console.WriteLine($"Error recognizing '{filePath}' {cancellation.ErrorDetails}");
                                }
                            }
                        };

                        recognizer.Canceled += (s, e) =>
                        {
                            var cancellation = CancellationDetails.FromResult(e.Result);
                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                result.Error = cancellation.ToString();
                                result.ErrorHappened = true;

                                Console.WriteLine($"Error recognizing '{filePath}' {cancellation.ErrorDetails}");
                            }

                            stopRecognition.TrySetResult(0);
                        };

                        recognizer.SessionStopped += (s, e) =>
                        {
                            stopRecognition.TrySetResult(0);
                        };

                        recognizer.StartContinuousRecognitionAsync().Wait();
                        Task.WaitAny(new[] { stopRecognition.Task });
                        recognizer.StopContinuousRecognitionAsync().Wait();
                    }
                }
            }
            catch (Exception e)
            {
                result.Error = e.ToString();
                result.ErrorHappened = true;
            }
            return result;
        }

        private RecognitionResult RecognizeSingleFileSingleShot(string filePath)
        {
            var r = new RecognitionResult { Text = new List<string>() };
            try
            {
                using (var audioInput = AudioConfig.FromWavFileInput(filePath))
                {
                    using (var recognizer = new SpeechRecognizer(this.config, audioInput))
                    {
                        recognizer.SessionStarted += (s, e) =>
                        {
                            r.SessionId = e.SessionId;
                        };

                        bool shouldExit = false;
                        while(!shouldExit)
                        {
                            var result = recognizer.RecognizeOnceAsync().Result;
                            switch (result.Reason)
                            {
                                case ResultReason.Canceled:
                                    var cancellation = CancellationDetails.FromResult(result);
                                    if (cancellation.Reason == CancellationReason.Error)
                                    {
                                        r.Error = cancellation.ToString();
                                        r.ErrorHappened = true;
                                    }
                                    shouldExit = true;
                                    break;
                                case ResultReason.RecognizedSpeech:
                                    r.Text.Add(result.Text);
                                    break;
                            }
                        }
                    }
                }
            }
            catch (Exception e)
            {
                r.Error += e.ToString();
                r.ErrorHappened = true;
            }

            return r;
        }
    }
}
