//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace MicrosoftSpeechSDKSamples
{
    class Program
    {
        private static readonly string choose = " Please choose one of the following samples:";
        private static readonly string mainPrompt = " Your choice (or 0 to exit): ";
        private static readonly string exiting = "\n Exiting...";
        private static readonly string invalid = "\n Invalid input, choose again.";
        private static readonly string done = "\n Done!";

        //
        // Shows pronunciation assessment samples.
        // See more information at https://aka.ms/csspeech/pa
        //
        static void Main(string[] args)
        {
            ConsoleKeyInfo x;

            do
            {
                Console.WriteLine("");
                Console.WriteLine(" Speech SDK - Pronunciation Assessment");
                Console.WriteLine("");
                Console.WriteLine(choose);
                Console.WriteLine("");
                Console.WriteLine(" 1. Pronunciation assessment with microphone input.");
                Console.WriteLine(" 2. Pronunciation assessment with stream input.");
                Console.WriteLine(" 3. Pronunciation assessment configured with json.");
                Console.WriteLine(" 4. Pronunciation assessment continuous with file.");
                Console.WriteLine(" 5. Pronunciation assessment with Microsoft Audio Stack.");
                Console.WriteLine(" 6. Pronunciation assessment with REST API.");
                Console.WriteLine("");
                Console.Write(mainPrompt);

                x = Console.ReadKey();
                Console.WriteLine("\n");
                bool sampleWasRun = true;

                switch (x.Key)
                {
                    case ConsoleKey.D1:
                    case ConsoleKey.NumPad1:
                        SpeechRecognitionSamples.PronunciationAssessmentWithMicrophoneAsync().Wait();
                        break;
                    case ConsoleKey.D2:
                    case ConsoleKey.NumPad2:
                        SpeechRecognitionSamples.PronunciationAssessmentWithStream();
                        break;
                    case ConsoleKey.D3:
                    case ConsoleKey.NumPad3:
                        SpeechRecognitionSamples.PronunciationAssessmentConfiguredWithJson().Wait();
                        break;
                    case ConsoleKey.D4:
                    case ConsoleKey.NumPad4:
                        SpeechRecognitionSamples.PronunciationAssessmentContinuousWithFile().Wait();
                        break;
                    case ConsoleKey.D5:
                    case ConsoleKey.NumPad5:
                        SpeechRecognitionSamples.PronunciationAssessmentWithMas().Wait();
                        break;
                    case ConsoleKey.D6:
                    case ConsoleKey.NumPad6:
                        SpeechRecognitionSamples.PronunciationAssessmentWithRestApi().GetAwaiter().GetResult();
                        break;
                    case ConsoleKey.D0:
                    case ConsoleKey.NumPad0:
                        Console.WriteLine(exiting);
                        sampleWasRun = false;
                        break;
                    default:
                        Console.WriteLine(invalid);
                        sampleWasRun = false;
                        break;
                }

                if (sampleWasRun) Console.WriteLine(done);

            } while (x.Key != ConsoleKey.D0 && x.Key != ConsoleKey.NumPad0);
        }
    }
}
