//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech.Recognition;

namespace MicrosoftSpeechSDKSamples
{
    class Program
    {
        static void Main(string[] args)
        {

            Console.WriteLine("1. Speech recognition with microphone input.");
            Console.WriteLine("2. Speech recognition with file input.");
            Console.WriteLine("3. Speech recognition using customized model.");
            Console.WriteLine("4. Speech continuous Recognition using events.");
            Console.WriteLine("5. Translation with microphone input.");
            Console.WriteLine("6. Translation with file input.");
            Console.WriteLine("7. Translation continuous Recognition using events.");

            Console.Write("Your choice: (0: Stop.) ");

            ConsoleKeyInfo x;
            do
            {
                x = Console.ReadKey();
                switch (x.Key)
                {
                    case ConsoleKey.D1:
                        SpeechRecognitionSamples.RecognitionWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                        SpeechRecognitionSamples.RecognitionWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D3:
                        SpeechRecognitionSamples.RecognitionUsingCustomizedModelAsync().Wait();
                        break;
                    case ConsoleKey.D4:
                        SpeechRecognitionSamples.ContinuousRecognitionAsync().Wait();
                        break;
                    case ConsoleKey.D5:
                        TranslationSamples.TranslationWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D6:
                        TranslationSamples.TranslationWithFileAsync().Wait();
                        break;
                    case ConsoleKey.D7:
                        TranslationSamples.TranslationContinuousRecognitionAsync().Wait();
                        break;
                    case ConsoleKey.D0:
                        Console.WriteLine("Exiting...");
                        break;
                    default:
                        Console.WriteLine("Invalid input.");
                        break;
                }
                Console.WriteLine("Recognition done. Your Choice:");
            } while (x.Key != ConsoleKey.D0);
        }
    }
}
