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
            const string usage = "Usage: CsharpHelloWorld <subscriptionKey> <service-region> [<path-to-wav-file>]\n";

            if (args.Length < 2)
            {
                // In Visual Studio, right-click the CsharpHelloWorld project in the Solution Explorer and add
                // your subscription key to Properties > Start options > Command line options.
                Console.WriteLine("Error: missing parameters");
                Console.Write(usage);
                Environment.Exit(1);
            }

            var subscriptionKey = args[0];
            var region = args[1];
            string filename = string.Empty;

            if (3 < args.Length)
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
                    Console.WriteLine($"There was an error, status {result.RecognitionStatus.ToString()}, reason {result.RecognitionFailureReason}");
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
