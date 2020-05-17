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
            return _synthesizerStack.TryPop(out SpeechSynthesizer item) ? item : _synthesizerGenerator();
        }

        public void Put(SpeechSynthesizer item)
        {
            if (_synthesizerStack.Count < _maximumRetainedCapacity)
            {
                _synthesizerStack.Push(item);
            }
            else
            {
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
            var pool = new SynthesizerPool(() => new SpeechSynthesizer(speechConfig, null));
            var latencyList = new List<double>();
            var processingTimeList = new List<double>();

            for (var turn = 0; turn < 3; turn++)
            {
                Console.WriteLine("turn: {0}", turn);

                Parallel.For(0, 64, (i) =>
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
                            latencyList.Add((DateTime.Now - start).TotalMilliseconds);
                        }
                    }

                    synthesizer.Synthesizing += SynthesizingEvent;

                    var result = synthesizer.SpeakTextAsync($"today is a nice day. {turn}{i}").Result;
                    if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                    {
                        if (turn > 0)
                        {
                            processingTimeList.Add((DateTime.Now - start).TotalMilliseconds);
                        }
                        synthesizer.Synthesizing -= SynthesizingEvent;
                        pool.Put(synthesizer);
                    }
                    else
                    {
                        var err = SpeechSynthesisCancellationDetails.FromResult(result);
                        Console.WriteLine(err.ToString());
                        synthesizer.Dispose();
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
