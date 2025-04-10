//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;

namespace MicrosoftSpeechSDKSamples
{
    public class SpeechSynthesisSamples
    {
        // Speech synthesis to the default speaker.
        public static async Task SynthesisToSpeakerAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer using the default speaker as audio output.
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to speaker.
                    Console.WriteLine("Enter some text that you want to speak, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized to speaker for text [{text}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis in the specified spoken language.
        public static async Task SynthesisWithLanguageAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Sets the synthesis language.
            // The full list of supported language can be found here:
            // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
            var language = "de-DE";
            config.SpeechSynthesisLanguage = language;

            // Creates a speech synthesizer for the specified language, using the default speaker as audio output.
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to speaker.
                    Console.WriteLine("Enter some text that you want to speak, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized to speaker for text [{text}] with language [{language}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis in the specified voice.
        public static async Task SynthesisWithVoiceAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Sets the voice name.
            // e.g. "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)".
            // The full list of supported voices can be found here:
            // https://aka.ms/csspeech/voicenames
            // And, you can try GetVoicesAsync method to get all available voices (see SynthesisGetAvailableVoicesAsync() sample below).
            var voice = "Microsoft Server Speech Text to Speech Voice (en-US, JennyNeural)";
            config.SpeechSynthesisVoiceName = voice;

            // Creates a speech synthesizer for the specified voice, using the default speaker as audio output.
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to speaker.
                    Console.WriteLine("Enter some text that you want to speak, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized to speaker for text [{text}] with voice [{voice}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis using Custom Voice (https://aka.ms/customvoice)
        public static async Task SynthesisUsingCustomVoiceAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");
            // Replace with the endpoint id of your Custom Voice model.
            config.EndpointId = "YourEndpointId";
            // Replace with the voice name of your Custom Voice model.
            config.SpeechSynthesisVoiceName = "YourVoiceName";

            // Creates a speech synthesizer for Custom Voice, using the default speaker as audio output.
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to speaker.
                    Console.WriteLine("Enter some text that you want to speak, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized to speaker for text [{text}].");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis to wave file.
        public static async Task SynthesisToWaveFileAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer using file as audio output.
            // Replace with your own audio file name.
            var fileName = "outputaudio.wav";
            using (var fileOutput = AudioConfig.FromWavFileOutput(fileName))
            using (var synthesizer = new SpeechSynthesizer(config, fileOutput))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to wave file.
                    Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized for text [{text}], and the audio was saved to [{fileName}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis to MP3 file.
        public static async Task SynthesisToMp3FileAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Sets the synthesis output format.
            // The full list of supported format can be found here:
            // https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-text-to-speech#audio-outputs
            config.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Audio16Khz32KBitRateMonoMp3);

            // Creates a speech synthesizer using file as audio output.
            // Replace with your own audio file name.
            var fileName = "outputaudio.mp3";
            using (var fileOutput = AudioConfig.FromWavFileOutput(fileName))
            using (var synthesizer = new SpeechSynthesizer(config, fileOutput))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to mp3 file.
                    Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized for text [{text}], and the audio was saved to [{fileName}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis a plain text file to MP3 file. It can support a long text file with more than 10 minutes limit.
        // the audio output will be put into one file that can be played continuously  
        public static async Task SynthesisFileToMp3FileAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Sets the synthesis output format.
            // The full list of supported format can be found here:
            // https://docs.microsoft.com/azure/cognitive-services/speech-service/rest-text-to-speech#audio-outputs
            config.SetSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Audio16Khz32KBitRateMonoMp3);

            // here is plain text file. A simple logic is to split by lines which is a paragraph. 
            // we assume a paragraph won't exceed 10 min limit here. 
            string[] paragraphs = System.IO.File.ReadAllLines("test.txt");
            int RetryCount = 10;
            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();

            // Creates a speech synthesizer using file as audio output.
            // Replace with your own audio file name.
            var fileName = "outputaudio.mp3";
            using (var fileOutput = AudioConfig.FromWavFileOutput(fileName))
            using (var synthesizer = new SpeechSynthesizer(config, fileOutput))
            {
                foreach (string pargraph in paragraphs)
                {
                    int retry = RetryCount;
                    while (retry > 0)
                    {
                        using (var result = await synthesizer.SpeakTextAsync(pargraph))
                        {
                            if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                            {
                                Console.WriteLine($"success on {pargraph} {result.ResultId} in {sw.ElapsedMilliseconds} msec");
                                break;
                            }
                            else if (result.Reason == ResultReason.Canceled)
                            {
                                Console.WriteLine($"failed on {pargraph} {result.ResultId}");
                                var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                                Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                                if (cancellation.Reason == CancellationReason.Error)
                                {
                                    Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                    Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                    Console.WriteLine($"CANCELED: Did you update the subscription info?");
                                }
                            }

                            retry--;
                            Console.WriteLine("retrying again...");
                        }
                    }
                }
            }
        }

        // Speech synthesis to pull audio output stream.
        public static async Task SynthesisToPullAudioOutputStreamAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates an audio out stream.
            using (var stream = AudioOutputStream.CreatePullStream())
            {
                // Creates a speech synthesizer using audio stream output.
                using (var streamConfig = AudioConfig.FromStreamOutput(stream))
                using (var synthesizer = new SpeechSynthesizer(config, streamConfig))
                {
                    while (true)
                    {
                        // Receives a text from console input and synthesize it to pull audio output stream.
                        Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                        Console.Write("> ");
                        string text = Console.ReadLine();
                        if (string.IsNullOrEmpty(text))
                        {
                            break;
                        }

                        using (var result = await synthesizer.SpeakTextAsync(text))
                        {
                            if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                            {
                                Console.WriteLine($"Speech synthesized for text [{text}], and the audio was written to output stream.");
                            }
                            else if (result.Reason == ResultReason.Canceled)
                            {
                                var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                                Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                                if (cancellation.Reason == CancellationReason.Error)
                                {
                                    Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                    Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                    Console.WriteLine($"CANCELED: Did you update the subscription info?");
                                }
                            }
                        }
                    }
                }

                // Reads(pulls) data from the stream
                byte[] buffer = new byte[32000];
                uint filledSize = 0;
                uint totalSize = 0;
                while ((filledSize = stream.Read(buffer)) > 0)
                {
                    Console.WriteLine($"{filledSize} bytes received.");
                    totalSize += filledSize;
                }

                Console.WriteLine($"Totally {totalSize} bytes received.");
            }
        }

        // Speech synthesis to push audio output stream.
        // This sample will also print the first byte latency of the request
        public static async Task SynthesisToPushAudioOutputStreamAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates an instance of a customer class inherited from PushAudioOutputStreamCallback
            var callback = new PushAudioOutputStreamSampleCallback();

            // Creates an audio out stream from the callback.
            using (var stream = AudioOutputStream.CreatePushStream(callback))
            {
                // Creates a speech synthesizer using audio stream output.
                using (var streamConfig = AudioConfig.FromStreamOutput(stream))
                using (var synthesizer = new SpeechSynthesizer(config, streamConfig))
                {
                    while (true)
                    {
                        // Receives a text from console input and synthesize it to push audio output stream.
                        Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                        Console.Write("> ");
                        string text = Console.ReadLine();
                        if (string.IsNullOrEmpty(text))
                        {
                            break;
                        }

                        using (var result = await synthesizer.SpeakTextAsync(text))
                        {
                            if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                            {
                                Console.WriteLine($"Speech synthesized for text [{text}], and the audio was written to output stream. first byte latency: {callback.GetLatency()}");
                            }
                            else if (result.Reason == ResultReason.Canceled)
                            {
                                var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                                Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                                if (cancellation.Reason == CancellationReason.Error)
                                {
                                    Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                    Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                    Console.WriteLine($"CANCELED: Did you update the subscription info?");
                                }
                            }
                        }
                    }
                }

                Console.WriteLine($"Totally {callback.GetAudioData().Length} bytes received.");
            }
        }

        // Gets synthesized audio data from result.
        public static async Task SynthesisToResultAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null as AudioConfig))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to result.
                    Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized for text [{text}].");
                            var audioData = result.AudioData;
                            Console.WriteLine($"{audioData.Length} bytes of audio data received for text [{text}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis to audio data stream.
        public static async Task SynthesisToAudioDataStreamAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null as AudioConfig))
            {
                while (true)
                {
                    // Receives a text from console input and synthesize it to result.
                    Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized for text [{text}].");

                            using (var audioDataStream = AudioDataStream.FromResult(result))
                            {
                                // You can save all the data in the audio data stream to a file
                                string fileName = "outputaudio.wav";
                                await audioDataStream.SaveToWaveFileAsync(fileName);
                                Console.WriteLine($"Audio data for text [{text}] was saved to [{fileName}]");

                                // You can also read data from audio data stream and process it in memory
                                // Reset the stream position to the beginning since saving to file puts the position at the end
                                audioDataStream.SetPosition(0);

                                byte[] buffer = new byte[16000];
                                uint totalSize = 0;
                                uint filledSize = 0;

                                while ((filledSize = audioDataStream.ReadData(buffer)) > 0)
                                {
                                    Console.WriteLine($"{filledSize} bytes received.");
                                    totalSize += filledSize;
                                }

                                Console.WriteLine($"{totalSize} bytes of audio data received for text [{text}]");
                            }
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis events.
        public static async Task SynthesisEventsAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null as AudioConfig))
            {
                // Subscribes to events
                synthesizer.SynthesisStarted += (s, e) =>
                {
                    Console.WriteLine("Synthesis started.");
                };

                synthesizer.Synthesizing += (s, e) =>
                {
                    Console.WriteLine($"Synthesizing event received with audio chunk of {e.Result.AudioData.Length} bytes.");
                };

                synthesizer.SynthesisCompleted += (s, e) =>
                {
                    Console.WriteLine("Synthesis completed.");
                };

                while (true)
                {
                    // Receives a text from console input and synthesize it to result.
                    Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized for text [{text}].");
                            var audioData = result.AudioData;
                            Console.WriteLine($"{audioData.Length} bytes of audio data received for text [{text}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis word boundary event.
        public static async Task SynthesisWordBoundaryEventAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null as AudioConfig))
            {
                // Subscribes to word boundary event
                synthesizer.WordBoundary += (s, e) =>
                {
                    // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
                    Console.WriteLine($"Word boundary event received. Audio offset: " +
                        $"{(e.AudioOffset + 5000) / 10000}ms, text offset: {e.TextOffset}, word length: {e.WordLength}.");
                };

                while (true)
                {
                    // Receives a text from console input and synthesize it to result.
                    Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized for text [{text}].");
                            var audioData = result.AudioData;
                            Console.WriteLine($"{audioData.Length} bytes of audio data received for text [{text}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis viseme event.
        public static async Task SynthesisVisemeEventAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null as AudioConfig))
            {
                // Subscribes to viseme received event
                synthesizer.VisemeReceived += (s, e) =>
                {
                    // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
                    Console.WriteLine($"Viseme event received. Audio offset: " +
                        $"{e.AudioOffset / 10000}ms, viseme id: {e.VisemeId}.");
                };

                while (true)
                {
                    // Receives a text from console input and synthesize it to result.
                    Console.WriteLine("Enter some text that you want to synthesize, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized for text [{text}].");
                            var audioData = result.AudioData;
                            Console.WriteLine($"{audioData.Length} bytes of audio data received for text [{text}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis bookmark event.
        public static async Task SynthesisBookmarkEventAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null as AudioConfig))
            {
                // Subscribes to bookmark reached event
                synthesizer.BookmarkReached += (s, e) =>
                {
                    // The unit of e.AudioOffset is tick (1 tick = 100 nanoseconds), divide by 10,000 to convert to milliseconds.
                    Console.WriteLine($"Bookmark reached. Audio offset: " +
                        $"{e.AudioOffset / 10000}ms, bookmark text: {e.Text}.");
                };

                Console.WriteLine("Press Enter to start synthesizing.");
                Console.ReadLine();

                // Bookmark tag is needed in the SSML, e.g.
                var ssml = "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts'><voice name='Microsoft Server Speech Text to Speech Voice (en-US, AriaNeural)'><bookmark mark='bookmark_one'/> one. <bookmark mark='bookmark_two'/> two. three. four.</voice></speak>";

                using (var result = await synthesizer.SpeakSsmlAsync(ssml))
                {
                    if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                    {
                        Console.WriteLine("Speech synthesized.");
                    }
                    else if (result.Reason == ResultReason.Canceled)
                    {
                        var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                        Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                        if (cancellation.Reason == CancellationReason.Error)
                        {
                            Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                            Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                            Console.WriteLine($"CANCELED: Did you update the subscription info?");
                        }
                    }
                }
            }
        }

        // Speech synthesis with auto detection for source language
        // Note: this is a preview feature, which might be updated in future versions.
        public static async Task SynthesisWithAutoDetectSourceLanguageAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates an instance of AutoDetectSourceLanguageConfig with open languages range
            var autoDetectSourceLanguageConfig = AutoDetectSourceLanguageConfig.FromOpenRange();

            // Creates a speech synthesizer with auto detection for source language, using the default speaker as audio output.
            using (var synthesizer = new SpeechSynthesizer(config, autoDetectSourceLanguageConfig,
                AudioConfig.FromDefaultSpeakerOutput()))
            {
                while (true)
                {
                    // Receives a multi lingual text from console input and synthesize it to speaker.
                    // For example, you can input "Bonjour le monde. Hello world.", then you will hear "Bonjour le monde."
                    // spoken in a French voice and "Hello world." in an English voice.
                    Console.WriteLine("Enter some text that you want to speak, or enter empty text to exit.");
                    Console.Write("> ");
                    string text = Console.ReadLine();
                    if (string.IsNullOrEmpty(text))
                    {
                        break;
                    }

                    using (var result = await synthesizer.SpeakTextAsync(text))
                    {
                        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                        {
                            Console.WriteLine($"Speech synthesized to speaker for text [{text}]");
                        }
                        else if (result.Reason == ResultReason.Canceled)
                        {
                            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                            Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                            if (cancellation.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails=[{cancellation.ErrorDetails}]");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }
                        }
                    }
                }
            }
        }

        // Speech synthesis get available voices
        public static async Task SynthesisGetAvailableVoicesAsync()
        {
            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Creates a speech synthesizer
            using (var synthesizer = new SpeechSynthesizer(config, null as AudioConfig))
            {
                Console.WriteLine("Enter a locale in BCP-47 format (e.g. en-US) that you want to get the voices of, or enter empty to get voices in all locales.");
                Console.Write("> ");
                string text = Console.ReadLine();

                using (var result = await synthesizer.GetVoicesAsync(text))
                {
                    if (result.Reason == ResultReason.VoicesListRetrieved)
                    {
                        Console.WriteLine("Voices successfully retrieved, they are:");
                        foreach (var voice in result.Voices)
                        {
                            Console.WriteLine(voice.Name);
                        }
                    }
                    else if (result.Reason == ResultReason.Canceled)
                    {
                        Console.WriteLine($"CANCELED: ErrorDetails=[{result.ErrorDetails}]");
                        Console.WriteLine($"CANCELED: Did you update the subscription info?");
                    }
                }
            }
        }

        private static async Task<SpeechSynthesisResult> SynthesizeOnceAsyncInternal(string key, string region,
            string endpointId = null, string voiceName = null, string text = "Hello World!")
        {
            SpeechSynthesisResult synthesisResult = null;
            var config = SpeechConfig.FromEndpoint(new Uri($"https://{region}.api.cognitive.microsoft.com"), key);
            if (!string.IsNullOrEmpty(endpointId))
            {
                config.EndpointId = endpointId;
            }

            if (!string.IsNullOrEmpty(voiceName))
            {
                config.SpeechSynthesisVoiceName = voiceName;
            }

            // Creates a speech synthesizer using the default speaker as audio output.
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                synthesisResult = await synthesizer.SpeakTextAsync(text).ConfigureAwait(false);
            }


            return synthesisResult;
        }

        // Speech synthesis with backup subscription region.
        public static async Task SynthesizeOnceToSpeakerAsyncSwitchSecondaryRegion()
        {
            // Create a speech resource with primary subscription key and service region.
            // Also create a speech resource with secondary subscription key and service region
            SpeechSynthesisResult synthesisResult = await SynthesizeOnceAsyncInternal("YourPrimarySubscriptionKey", "YourPrimaryServiceRegion" );
            if (synthesisResult.Reason == ResultReason.Canceled)
            {
                SpeechSynthesisCancellationDetails details = SpeechSynthesisCancellationDetails.FromResult(synthesisResult);
                if (details.ErrorCode == CancellationErrorCode.ConnectionFailure
                    || details.ErrorCode == CancellationErrorCode.ServiceUnavailable
                    || details.ErrorCode == CancellationErrorCode.ServiceTimeout)
                {
                    synthesisResult = await SynthesizeOnceAsyncInternal("YourSecondarySubscriptionKey", "YourSecondaryServiceRegion");
                }
            }
        }

        // Custom voice with backup
        // If custom voice service is unavailable, you can fallback to a standard platform voice.
        public static async Task SynthesizeOnceUseCustomVoiceToSpeakerAsyncSwitchPlatformVoice()
        {
            // Create a custom voice speech resource with subscription key, service region, endpoint ID and voice name.
            SpeechSynthesisResult synthesisResult = await SynthesizeOnceAsyncInternal("YourSubscriptionKey",
                "YourServiceRegion", "YourEndpointId", "YourCustomVoiceName");
            if (synthesisResult.Reason == ResultReason.Canceled)
            {
                SpeechSynthesisCancellationDetails details = SpeechSynthesisCancellationDetails.FromResult(synthesisResult);
                if (details.ErrorCode == CancellationErrorCode.ConnectionFailure
                    || details.ErrorCode == CancellationErrorCode.ServiceUnavailable
                    || details.ErrorCode == CancellationErrorCode.ServiceTimeout
                    || details.ErrorDetails.Contains("Error code: 1007"))
                {
                    // Synthesize using a standard platform voice, e.g. en-US-JennyNeural
                    synthesisResult = await SynthesizeOnceAsyncInternal("YourSubscriptionKey", "YourServiceRegion", null, "YourPlatformVoiceName");
                }
            }
        }

        // Custom voice with backup.
        // If custom voice service is unavailable, you can fallback to custom voice service in another region.
        public static async Task SynthesizeOnceUseCustomVoiceToSpeakerAsyncSwitchSecondaryRegion()
        {
            // Create a custom voice resource with primary subscription key and service region.
            // Also create a speech resource with secondary subscription key and service region.
            // Copy Custom Voice model from primary region to secondary region and deploy.
            SpeechSynthesisResult synthesisResult = await SynthesizeOnceAsyncInternal("YourPrimarySubscriptionKey",
                "YourPrimaryServiceRegion", "YourEndpointIdOnPrimaryRegion", "YourCustomVoiceName");
            if (synthesisResult.Reason == ResultReason.Canceled)
            {
                SpeechSynthesisCancellationDetails details = SpeechSynthesisCancellationDetails.FromResult(synthesisResult);
                if (details.ErrorCode == CancellationErrorCode.ConnectionFailure
                    || details.ErrorCode == CancellationErrorCode.ServiceUnavailable
                    || details.ErrorCode == CancellationErrorCode.ServiceTimeout
                    || details.ErrorDetails.Contains("Error code: 1007"))
                {
                    // Synthesize using same custom voice model in secondary region.
                    synthesisResult = await SynthesizeOnceAsyncInternal("YourSecondarySubscriptionKey", "YourSecondaryServiceRegion",
                        "YourEndpointIdOnSecondaryRegion", "YourCustomVoiceName");
                }
            }
        }
    }
}
