//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Xml.Linq;

namespace MicrosoftSpeechSDKSamples
{
    // Scenario:
    //     In scenario like voice assistant or call center,  normally TTS can be integrated from service side
    //     The client app will talk to the middle layer service which will talk to ASR, dialog, and TTS.
    //     In this scenario, latency is one important metrics for better user experience.
    //
    // Best Practice:
    //      For server scenario synthesizing with high concurrency, we recommend two methods to reduce the latency.
    //      Firstly, reuse the synthesizers (e.g. use a synthesizer pool ) to reduce the connection establish latency.
    //      This is because new synthesizer instance need to take time to connect to the service. Reusing the instance can save time of connection.
    //      Secondly, use AudioOutputStream or synthesizing event to streaming receive the synthesized audio to lower the first byte latency.
    //      This middle layer service should send audio back to client app/device as soon as possible.
    //      On the client side, it can start playback when enough audio bytes has been received.


    /// <summary>
    /// This is synthesizer pool that manages synthesizer instances to be used by concurrent request.
    /// </summary>
    public class SynthesizerPool : IDisposable
    {
        private readonly Func<SpeechSynthesizer> _synthesizerGenerator;

        private readonly ConcurrentStack<SpeechSynthesizer> _synthesizerStack;
        private readonly int _maximumRetainedCapacity;

        public SynthesizerPool(Func<SpeechSynthesizer> synthesizerGenerator, int initialCapacity = 2, int maximumRetainedCapacity = 100)
        {
            _synthesizerGenerator = synthesizerGenerator;
            _synthesizerStack = new ConcurrentStack<SpeechSynthesizer>();
            _maximumRetainedCapacity = maximumRetainedCapacity;

            Console.WriteLine($"Create initial {initialCapacity} synthesizer and warm up");
            for (var i = 0; i < initialCapacity; i++)
            {
                SpeechSynthesizer item = _synthesizerGenerator();

                // warm up synthesizer
                item.SpeakTextAsync("1").Wait();
                Put(item);
            }

            Console.WriteLine("Pool created!");
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

    /// <summary>
    /// a synthesizer class that can init once, handle concurrent request
    /// </summary>
    public class SynthesisServer
    {
        private SynthesizerPool pool;
        private SpeechConfig speechConfig;
        private List<double> latencyList;
        private List<double> processingTimeList;

        public SynthesisServer(string subscription, string region, string voiceName, SpeechSynthesisOutputFormat outputFormat, int concurrency)
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // set your voice name
            speechConfig.SpeechSynthesisVoiceName = voiceName;

            // use mp3 format to reduce network transfer payload
            speechConfig.SetSpeechSynthesisOutputFormat(outputFormat);

            // pool should be a single instance to handle all request. In real scenario, this could be put as a static class member
            pool = new SynthesizerPool(() => new SpeechSynthesizer(speechConfig, null), concurrency);
            latencyList = new List<double>();
            processingTimeList = new List<double>();
        }

        /// <summary>
        /// Generates SSML.
        /// </summary>
        /// <param name="locale">The locale.</param>
        /// <param name="gender">The gender.</param>
        /// <param name="name">The voice name.</param>
        /// <param name="text">The text input.</param>
        private string GenerateSsml(string locale, string gender, string name, string text)
        {
            var ssmlDoc = new XDocument(
                              new XElement("speak",
                                  new XAttribute("version", "1.0"),
                                  new XAttribute(XNamespace.Xml + "lang", locale),
                                  new XElement("voice",
                                      new XAttribute(XNamespace.Xml + "lang", locale),
                                      new XAttribute(XNamespace.Xml + "gender", gender),
                                      new XAttribute("name", name),
                                      text)));
            return ssmlDoc.ToString();
        }

        public void Synthesize(string text)
        {
            var start = DateTime.Now;
            var synthesizer = pool.Get();
            var ssml = GenerateSsml("en-US", "Female", speechConfig.SpeechSynthesisVoiceName, text);
            bool first = true;


            void SynthesizingEvent(object sender, SpeechSynthesisEventArgs eventArgs)
            {
                // receive streaming audio here.
                if (!first)
                {
                    return;
                }

                Console.WriteLine("First byte latency: {0}", DateTime.Now - start);
                first = false;
                latencyList.Add((DateTime.Now - start).TotalMilliseconds);
            }

            void SynthesizerSynthesisCanceled(object sender, SpeechSynthesisEventArgs e)
            {
                var cancellation = SpeechSynthesisCancellationDetails.FromResult(e.Result);
                Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                if (cancellation.Reason == CancellationReason.Error)
                {
                    Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                    Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                    Console.WriteLine($"CANCELED: Did you update the subscription info?");
                }
            }

            synthesizer.Synthesizing += SynthesizingEvent;
            synthesizer.SynthesisCanceled += SynthesizerSynthesisCanceled;

            var result = synthesizer.StartSpeakingSsmlAsync(ssml).Result;
            try
            {
                if (result.Reason == ResultReason.SynthesizingAudioStarted)
                {
                    uint totalSize = 0;
                    using (var audioDataStream = AudioDataStream.FromResult(result))
                    {
                        // buffer block size can be adjusted based on scenario
                        byte[] buffer = new byte[4096];
                        uint filledSize = 0;

                        // read audio block in a loop here
                        // if it is end of audio stream, it will return 0
                        // if there are error happening,  the cancel event will be called.
                        while ((filledSize = audioDataStream.ReadData(buffer)) > 0)
                        {
                            // Here you can save the audio or send the data to another pipeline in your service.
                            Console.WriteLine($"{filledSize} bytes received. Handle the data buffer here");

                            totalSize += filledSize;
                        }
                    }

                    if (totalSize > 0)
                    {
                        processingTimeList.Add((DateTime.Now - start).TotalMilliseconds);
                    }

                    synthesizer.Synthesizing -= SynthesizingEvent;
                    synthesizer.SynthesisCanceled -= SynthesizerSynthesisCanceled;
                    pool.Put(synthesizer);
                }
            }
            catch (Exception)
            {
                synthesizer.SynthesisCanceled -= SynthesizerSynthesisCanceled;
                synthesizer.Synthesizing -= SynthesizingEvent;
                synthesizer.Dispose();
            }
            finally
            {
            }
        }

        public void DumpStats()
        {
            if (latencyList.Count > 0)
            {
                latencyList.Sort();
                Console.WriteLine("Average latency {0} ms", latencyList.Average());
                Console.WriteLine("Max latency {0} ms", latencyList.Max());
                Console.WriteLine("Min latency {0} ms", latencyList.Min());
                Console.WriteLine("90% latency {0} ms", latencyList[Math.Min(Convert.ToInt32(latencyList.Count * 0.9), latencyList.Count - 1)]);
                Console.WriteLine("95% latency {0} ms", latencyList[Math.Min(Convert.ToInt32(latencyList.Count * 0.95), latencyList.Count - 1)]);

                processingTimeList.Sort();
                Console.WriteLine("\nAverage processing time {0} ms", processingTimeList.Average());
                Console.WriteLine("Max processing time {0} ms", processingTimeList.Max());
                Console.WriteLine("Min processing time {0} ms", processingTimeList.Min());
                Console.WriteLine("90% processing time {0} ms", processingTimeList[Math.Min(Convert.ToInt32(processingTimeList.Count * 0.9), processingTimeList.Count - 1)]);
                Console.WriteLine("95% processing time {0} ms", processingTimeList[Math.Min(Convert.ToInt32(processingTimeList.Count * 0.95), processingTimeList.Count - 1)]);
            }
            else
            {
                Console.WriteLine("Something wrong! No latency data!");
            }
        }
    }

    public class SpeechSynthesisServerScenarioSample
    {
        // Specify your subscription key and service region (e.g., "westus").
        private const string subscriptionKey = "YourSubscriptionKey";
        private const string region = "YourServiceRegion";

        private const int concurrency = 64;
        public static void SpeechSynthesizeWithPool()
        {
            SynthesisServer server = new SynthesisServer(subscriptionKey, region,
                    "en-US-JennyNeural", SpeechSynthesisOutputFormat.Audio24Khz48KBitRateMonoMp3, concurrency);

            for (var turn = 0; turn < 3; turn++)
            {
                Console.WriteLine("turn: {0}", turn);

                Parallel.For(0, concurrency, (i) =>
                {
                    server.Synthesize($"today is a nice day. {turn}{i}");
                });

                Thread.Sleep(2000);
            }

            server.DumpStats();
            Console.WriteLine("Press the Enter key to exit.");
            Console.ReadLine();
        }
    }
}
