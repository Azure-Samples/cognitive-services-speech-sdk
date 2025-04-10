//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <code>
using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace helloworld
{
    class Program
    {
        public static async Task SynthesisToSpeakerAsync()
        {
            // To support Chinese Characters on Windows platform
            if (Environment.OSVersion.Platform == PlatformID.Win32NT)
            {
                Console.InputEncoding = System.Text.Encoding.Unicode;
                Console.OutputEncoding = System.Text.Encoding.Unicode;
            }

            // Creates an instance of a speech config with specified endpoint and subscription key.
            // Replace with your own endpoint and subscription key.
            // The default language is "en-us".
            var config = SpeechConfig.FromEndpoint(new Uri("https://YourServiceRegion.api.cognitive.microsoft.com"), "YourSubscriptionKey");

            // Set the voice name, refer to https://aka.ms/speech/voices/neural for full list.
            config.SpeechSynthesisVoiceName = "en-US-AriaNeural";

            // Creates a speech synthesizer using the default speaker as audio output.
            using (var synthesizer = new SpeechSynthesizer(config))
            {
                // Receive a text from console input and synthesize it to speaker.
                Console.WriteLine("Type some text that you want to speak...");
                Console.Write("> ");
                string text = Console.ReadLine();

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

                // This is to give some time for the speaker to finish playing back the audio
                Console.WriteLine("Press any key to exit...");
                Console.ReadKey();
            }
        }

        static async Task Main()
        {
            await SynthesisToSpeakerAsync();
        }
    }
}
// </code>
