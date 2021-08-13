//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Net.Http;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;

namespace MicrosoftSpeechSDKSamples
{
    // For server scenario synthesizing with high concurrency, we recommend two methods to reduce the latency.
    // Firstly, reuse the synthesizers (e.g. use a synthesizer pool ) to reduce the connection establish latency;
    //          This is because new synthesizer instance need to take time to connect to the service. Reusing the instance can save time of conenction.
    // secondly, use AudioOutputStream or synthesizing event to streaming receive the synthesized audio to lower the first byte latency.

    public class SynthesizerPool : IDisposable
    {
        private readonly Func<SpeechSynthesizer> _synthesizerGenerator; 

        private readonly ConcurrentStack<SpeechSynthesizer> _synthesizerStack;
        private readonly int _initialCapacity;
        private readonly int _maximumRetainedCapacity;

        public SynthesizerPool(Func<SpeechSynthesizer> synthesizerGenerator, int initialCapacity = 2, int maximumRetainedCapacity = 64)
        {
            _synthesizerGenerator = synthesizerGenerator;
            _synthesizerStack = new ConcurrentStack<SpeechSynthesizer>();
            _initialCapacity = initialCapacity;
            _maximumRetainedCapacity = maximumRetainedCapacity;
            for (var i = 0; i < initialCapacity; i++)
            {
                Put(_synthesizerGenerator());
            }
        }

        public void Dispose()
        {
            foreach (var synthesizer in _synthesizerStack)
            {
                synthesizer.Dispose();
            }
        }

        public SpeechSynthesizer Get()
        {
            SpeechSynthesizer item;
            if (!_synthesizerStack.TryPop(out item))
            {
                Console.WriteLine("create a brand new synthesizer...");
                item = _synthesizerGenerator();
            }

            return item;
        }

        public void Put(SpeechSynthesizer item)
        {
            if (_synthesizerStack.Count < _maximumRetainedCapacity)
            {
                Console.WriteLine("put synthesizer to pool.");
                _synthesizerStack.Push(item);
            }
            else
            {
                Console.WriteLine("dispose a synthesizer.");
                item.Dispose();
            }
        }
    }

    public class SpeechSynthesisServerScenarioSample
    {
        // Specify your subscription key and service region (e.g., "westus").
        private const string subscriptionKey = "YourSubscriptionKey";
        private const string region = "YourServiceRegion";

        public static void SpeechSynthesizeWithPool()
        {
            var speechConfig = SpeechConfig.FromSubscription(subscriptionKey, region);

            // set your voice name
            speechConfig.SpeechSynthesisVoiceName = "en-US-JennyNeural";

            // use mp3 format to reduce network transfer payload 
            speechConfig.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Audio24Khz48KBitRateMonoMp3);

            // pool should be a single instance to handle all request
            var pool = new SynthesizerPool(() => new SpeechSynthesizer(speechConfig, null));
            var latencyList = new List<double>();
            var processingTimeList = new List<double>();

            for (var turn = 0; turn < 3; turn++)
            {
                Console.WriteLine("turn: {0}", turn);

                Parallel.For(0, 1, (i) =>
                {
                    var start = DateTime.Now;
                    var synthesizer = pool.Get();
                    bool first = true;

                    void SynthesizingEvent(object sender, SpeechSynthesisEventArgs eventArgs)
                    {
                        // receive streaming audio here.
                        if (!first) return;
                        Console.WriteLine("First byte latency: {0}", DateTime.Now - start);
                        first = false;
                        if (turn > 0)
                        {
                            // skip first turn which is used to warm up 
                            latencyList.Add((DateTime.Now - start).TotalMilliseconds);
                        }
                    }

                    synthesizer.Synthesizing += SynthesizingEvent;

                    var result = synthesizer.StartSpeakingTextAsync($"today is a nice day. {turn}{i}").Result;
                    try
                    {
                        // BUGBUG: the reason is started, even i don't set key or region right
                        if (result.Reason == ResultReason.SynthesizingAudioStarted)
                        {
                            using (var audioDataStream = AudioDataStream.FromResult(result))
                            {
                                // buffer block size can be adjusted based on scenario
                                byte[] buffer = new byte[4096];
                                uint filledSize = 0;

                                // read audio block in a loop here 
                                // if it is end of audio stream, it will return 0
                                // Question: if network error happens, it will throw exception??
                                while ((filledSize = audioDataStream.ReadData(buffer)) > 0)
                                {
                                    // Here you can save the audio or send the data to another pipeline in your service.
                                    Console.WriteLine($"{filledSize} bytes received. Handle the data buffer here");
                                }
                            }

                            if (turn > 0)
                            {
                                processingTimeList.Add((DateTime.Now - start).TotalMilliseconds);
                            }

                            synthesizer.Synthesizing -= SynthesizingEvent;
                            pool.Put(synthesizer);
                        }
                    }
                    catch (Exception)
                    {
                        synthesizer.Synthesizing -= SynthesizingEvent;
                        synthesizer.Dispose();
                    }
                    finally
                    {
                        if (result.Reason == ResultReason.Canceled)
                        {
                            var err = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine(err.ToString());
                        }
                    }
                });

                Thread.Sleep(2000);
            }

            latencyList.Sort();
            Console.WriteLine("Average latency {0} ms", latencyList.Average());
            Console.WriteLine("Max latency {0} ms", latencyList.Max());
            Console.WriteLine("Min latency {0} ms", latencyList.Min());
            Console.WriteLine("90% latency {0} ms", latencyList[Convert.ToInt32(latencyList.Count * 0.9)]);
            Console.WriteLine("95% latency {0} ms", latencyList[Convert.ToInt32(latencyList.Count * 0.95)]);

            processingTimeList.Sort();
            Console.WriteLine("\nAverage processing time {0} ms", processingTimeList.Average());
            Console.WriteLine("Max processing time {0} ms", processingTimeList.Max());
            Console.WriteLine("Min processing time {0} ms", processingTimeList.Min());
            Console.WriteLine("90% processing time {0} ms", processingTimeList[Convert.ToInt32(processingTimeList.Count * 0.9)]);
            Console.WriteLine("95% processing time {0} ms", processingTimeList[Convert.ToInt32(processingTimeList.Count * 0.95)]);

            Console.WriteLine("Press the Enter key to exit.");
            Console.ReadLine();
        }
    }
}
