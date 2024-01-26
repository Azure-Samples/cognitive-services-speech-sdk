//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;


namespace MicrosoftSpeechSDKSamples
{
    public class SpeechSynthesisSamples
    {
        // List available embedded speech synthesis voices.
        public static async Task ListEmbeddedVoicesAsync()
        {
            // Creates an instance of an embedded speech config.
            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            if (speechConfig == null)
            {
                return;
            }

            // Creates a speech synthesizer.
            using var synthesizer = new SpeechSynthesizer(speechConfig, null);

            // Gets a list of voices.
            using var result = await synthesizer.GetVoicesAsync("");

            if (result.Reason == ResultReason.VoicesListRetrieved)
            {
                Console.WriteLine("Voices found:");
                foreach (var voice in result.Voices)
                {
                    Console.WriteLine(voice.Name);
                    Console.WriteLine($" Gender: {voice.Gender}");
                    Console.WriteLine($" Locale: {voice.Locale}");
                    Console.WriteLine($" Path:   {voice.VoicePath}");
                }

                // To find a voice that supports a specific locale, for example:
                /*
                string voiceName = null;
                string voiceLocale = "en-US";

                foreach (var voice in result.Voices)
                {
                    if (voice.Locale.Equals(voiceLocale))
                    {
                        voiceName = voice.Name;
                        break;
                    }
                }

                if (!string.IsNullOrEmpty(voiceName))
                {
                    Console.WriteLine($"Found {voiceLocale} voice: {voiceName}");
                }
                */
            }
            else if (result.Reason == ResultReason.Canceled)
            {
                Console.Error.WriteLine($"CANCELED: ErrorDetails=\"{result.ErrorDetails}\"");
            }
        }


        private static async Task SynthesizeSpeechAsync(SpeechSynthesizer synthesizer)
        {
            // Subscribes to events.
            synthesizer.SynthesisStarted += (s, e) =>
            {
                Console.WriteLine("Synthesis started.");
            };

            synthesizer.Synthesizing += (s, e) =>
            {
                Console.WriteLine($"Synthesizing, received an audio chunk of {e.Result.AudioData.Length} bytes.");
            };

            synthesizer.WordBoundary += (s, e) =>
            {
                Console.Write($"Word \"{e.Text}\" | ");
                Console.Write($"Text offset {e.TextOffset} | ");
                // Unit of AudioOffset is tick (1 tick = 100 nanoseconds).
                Console.WriteLine($"Audio offset {(e.AudioOffset + 5000) / 10000}ms");
            };

            while (true)
            {
                // Receives text from console input.
                Console.WriteLine("Enter some text that you want to speak, or none for exit.");
                Console.Write("> ");

                string text = Console.ReadLine();
                if (string.IsNullOrEmpty(text))
                {
                    break;
                }

                // Synthesizes text to speech.
                using var result = await synthesizer.SpeakTextAsync(text);

                // To adjust the rate or volume, use SpeakSsmlAsync with prosody tags instead of SpeakTextAsync.
                /*
                var ssmlSynthBegin = "<speak version='1.0' xml:lang='en-US' xmlns='http://www.w3.org/2001/10/synthesis' xmlns:mstts='http://www.w3.org/2001/mstts'>";
                var ssmlSynthEnd = "</speak>";
                string ssml;

                // Range of rate is -100% to 100%, 100% means 2x faster.
                ssml = ssmlSynthBegin + "<prosody rate='50%'>" + text + "</prosody>" + ssmlSynthEnd;
                Console.WriteLine("Synthesizing with rate +50%");
                using var result1 = await synthesizer.SpeakSsmlAsync(ssml);

                // Range of volume is -100% to 100%, 100% means 2x louder.
                ssml = ssmlSynthBegin + "<prosody volume='50%'>" + text + "</prosody>" + ssmlSynthEnd;
                Console.WriteLine("Synthesizing with volume +50%");
                using var result2 = await synthesizer.SpeakSsmlAsync(ssml);
                */

                if (result.Reason == ResultReason.SynthesizingAudioCompleted)
                {
                    Console.WriteLine($"Synthesis completed for text \"{text}\"");

                    // See where the result came from, cloud (online) or embedded (offline)
                    // speech synthesis.
                    // This can change during a session where HybridSpeechConfig is used.
                    /*
                    Console.WriteLine($"Synthesis backend: {result.Properties.GetProperty(PropertyId.SpeechServiceResponse_SynthesisBackend)}");
                    */

                    // To save the output audio to a WAV file:
                    /*
                    using var audioDataStream = AudioDataStream.FromResult(result);
                    await audioDataStream.SaveToWaveFileAsync("SynthesizedSpeech.wav");
                    */

                    // To read the output audio for e.g. processing it in memory:
                    /*
                    audioDataStream.SetPosition(0); // reset the stream position

                    byte[] buffer = new byte[16000];
                    uint readBytes = 0;
                    uint totalBytes = 0;

                    while ((readBytes = audioDataStream.ReadData(buffer)) > 0)
                    {
                        Console.WriteLine($"Read {readBytes} bytes");
                        totalBytes += readBytes;
                    }
                    Console.WriteLine($"Total {totalBytes} bytes");
                    */
                }
                else if (result.Reason == ResultReason.Canceled)
                {
                    var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);
                    Console.WriteLine($"CANCELED: Reason={cancellation.Reason}");

                    if (cancellation.Reason == CancellationReason.Error)
                    {
                        Console.Error.WriteLine($"CANCELED: ErrorCode={cancellation.ErrorCode}");
                        Console.Error.WriteLine($"CANCELED: ErrorDetails=\"{cancellation.ErrorDetails}\"");
                    }
                }
            }
        }


        // Synthesizes speech using embedded speech config and the system default speaker device.
        public static void EmbeddedSynthesisToSpeaker()
        {
            var speechConfig = Settings.CreateEmbeddedSpeechConfig();
            using var audioConfig = AudioConfig.FromDefaultSpeakerOutput();

            using var synthesizer = new SpeechSynthesizer(speechConfig, audioConfig);
            SynthesizeSpeechAsync(synthesizer).Wait();
        }


        // Synthesizes speech using hybrid (cloud & embedded) speech config and the system default speaker device.
        public static void HybridSynthesisToSpeaker()
        {
            var speechConfig = Settings.CreateHybridSpeechConfig();
            using var audioConfig = AudioConfig.FromDefaultSpeakerOutput();

            using var synthesizer = new SpeechSynthesizer(speechConfig, audioConfig);
            SynthesizeSpeechAsync(synthesizer).Wait();
        }
    }
}
