//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;

namespace CarbonSamples
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length <= 0)
            {
                Console.WriteLine("Usage: carbon_csharp_console wavfile");
                Environment.Exit(1);
            }

            var speechRecognizer = Carbon.RecognizerFactory.CreateSpeechRecognizerWithFileInput(args[0]);

            var result = speechRecognizer.Recognize();

            Console.WriteLine("Result: Id:" + result.ResultId + "Reason: " + result.Reason + "Text: " + result.Text);
        }
    }
}
