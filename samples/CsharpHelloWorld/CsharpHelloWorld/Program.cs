using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;

namespace CsharpHelloWorld
{
    class Program
    {
        static void Main(string[] args)
        {
            const string usage = "Usage: CsharpHelloWorld <subscriptionKey> [<path-to-file>]\n";

            if (args.Length == 0)
            {
                // In Visual Studio, right-click the CsharpHelloWorld project in the Solution Explorer and add
                // your subscription key to Properties > Start options > Command line options.
                Console.WriteLine("Error: missing speech subscription key");
                Console.Write(usage);
                Environment.Exit(1);
            }

            var subscriptionKey = args[0];
            string filename = string.Empty;

            if (2 < args.Length)
            {
                Console.WriteLine("Error: too many parameters.");
                Console.Write(usage);
                Environment.Exit(1);
            }

            if (args.Length == 2)
            {
                filename = args[1];
            }

            var factory = SpeechFactory.FromSubscription(subscriptionKey, "");
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
                    Console.WriteLine($"There was an error, status {result.RecognitionStatus}, reason {result.RecognitionFailureReason}");
                    Environment.Exit(1);
                }
                else
                {
                    Console.WriteLine($"We recognized: {result.RecognizedText}");

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
