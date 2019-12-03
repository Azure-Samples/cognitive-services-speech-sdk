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
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Sets the voice name.
            // e.g. "Microsoft Server Speech Text to Speech Voice (en-US, JessaRUS)"
            // The full list of supported voices can be found here:
            // https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
            var voice = "Microsoft Server Speech Text to Speech Voice (en-US, BenjaminRUS)";
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

        // Speech synthesis to wave file.
        public static async Task SynthesisToWaveFileAsync()
        {
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            // The default language is "en-us".
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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

        // Speech synthesis to pull audio output stream.
        public static async Task SynthesisToPullAudioOutputStreamAsync()
        {
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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
        public static async Task SynthesisToPushAudioOutputStreamAsync()
        {
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

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

                Console.WriteLine($"Totally {callback.GetAudioData().Length} bytes received.");
            }
        }

        // Gets synthesized audio data from result.
        public static async Task SynthesisToResultAsync()
        {
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null))
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
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null))
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
                                // Reset the stream position to the beginnging since saving to file puts the postion to end
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
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null))
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
            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").
            var config = SpeechConfig.FromSubscription("YourSubscriptionKey", "YourServiceRegion");

            // Creates a speech synthesizer with a null output stream.
            // This means the audio output data will not be written to any stream.
            // You can just get the audio from the result.
            using (var synthesizer = new SpeechSynthesizer(config, null))
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
    }
}
