//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace helloworld
{
    class Program
    {
        static void Main(string[] args)
        {
            const string usage = "Usage: helloworld <subscriptionKey> <service-region> [<path-to-wav-file>]\n";

            if (args.Length < 2)
            {
                // In Visual Studio, right-click the helloworld project in the Solution Explorer and add
                // your subscription key to Properties > Start options > Command line options.
                Console.WriteLine("Error: missing parameters");
                Console.Write(usage);
                Environment.Exit(1);
            }

            var subscriptionKey = args[0];
            var region = args[1];
            string filename = string.Empty;

            if (args.Length > 3)
            {
                Console.WriteLine("Error: too many parameters.");
                Console.Write(usage);
                Environment.Exit(1);
            }

            if (args.Length == 3)
            {
                filename = args[2];
            }

            var factory = SpeechFactory.FromSubscription(subscriptionKey, region);
            var recognizeFromFile = !string.IsNullOrEmpty(filename);

            SpeechRecognizer recognizer = null;

            try
            {
                if (recognizeFromFile)
                {
                    recognizer = factory.CreateSpeechRecognizerWithFileInput(filename);
                    Console.WriteLine("Recognizing from file...");
                }
                else
                {
                    recognizer = factory.CreateSpeechRecognizer();
                    Console.WriteLine("Say something...");
                }

                var result = recognizer.RecognizeAsync().GetAwaiter().GetResult();

                if (result.RecognitionStatus != RecognitionStatus.Recognized)
                {
                    Console.WriteLine($"Recognition status: {result.RecognitionStatus.ToString()}");
                    if (result.RecognitionStatus == RecognitionStatus.Canceled)
                    {
                        Console.WriteLine($"There was an error, reason {result.RecognitionFailureReason}");
                    }

                    Environment.Exit(1);
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.Text}");
                }
            }
            finally
            {
                // Dispose the recognizer if one was created.
                recognizer?.Dispose();
            }
        }
    }
}
