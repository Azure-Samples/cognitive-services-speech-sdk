//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace MicrosoftSpeechSDKSamples
{
    class Program
    {
        static void Main(string[] args)
        {

            Console.WriteLine("1. Speech recognition with microphone input.");
            Console.WriteLine("2. Speech recognition in the specified language and using detailed output format.");
            Console.WriteLine("3. Speech continuous recognition with file input.");
            Console.WriteLine("4. Speech recognition using customized model.");
            Console.WriteLine("5. Speech recognition with pull audio stream.");
            Console.WriteLine("6. Speech recognition with push audio stream.");
            Console.WriteLine("7. Translation with microphone input.");
            Console.WriteLine("8. Translation with file input.");
            Console.WriteLine("9. Translation with audio stream.");
            Console.WriteLine("A. Speech continuous recognition using authorization token.");
            Console.WriteLine("B. Intent recognition with microphone input.");
            Console.WriteLine("C. Intent continuous recognition with file input.");
            Console.WriteLine("D. Intent recognition in the specified language with microphone input.");

            Console.Write("Your choice (0: Stop.): ");

            ConsoleKeyInfo x;
            do
            {
                x = Console.ReadKey();
                Console.WriteLine("");
                switch (x.Key)
                {
                    case ConsoleKey.D1:
                        SpeechRecognitionSamples.RecognitionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                        SpeechRecognitionSamples.RecognitionWithLanguageAndDetailedOutputAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                        SpeechRecognitionSamples.ContinuousRecognitionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D4:
                        SpeechRecognitionSamples.RecognitionUsingCustomizedModelAsync().Wait();
                        break;
                    case ConsoleKey.D5:
                        SpeechRecognitionSamples.RecognitionWithPullAudioStreamAsync().Wait();
                        break;
                    case ConsoleKey.D6:
                        SpeechRecognitionSamples.RecognitionWithPushAudioStreamAsync().Wait();
                        break;
                    case ConsoleKey.D7:
                        TranslationSamples.TranslationWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D8:
                        TranslationSamples.TranslationWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D9:
                        TranslationSamples.TranslationWithAudioStreamAsync().Wait();
                        break;
                    case ConsoleKey.A:
                        SpeechRecognitionWithTokenSample.ContinuousRecognitionWithAuthorizationTokenAsync().Wait();
                        break;
                    case ConsoleKey.B:
                        IntentRecognitionSamples.RecognitionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.C:
                        IntentRecognitionSamples.ContinuousRecognitionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D:
                        IntentRecognitionSamples.RecognitionWithMicrophoneUsingLanguageAsync().Wait();
                        break;
                    case ConsoleKey.D0:
                        Console.WriteLine("Exiting...");
                        break;
                    default:
                        Console.WriteLine("Invalid input.");
                        break;
                }
                Console.WriteLine("\nRecognition done. Your Choice (0: Stop): ");
            } while (x.Key != ConsoleKey.D0);
        }
    }
}
