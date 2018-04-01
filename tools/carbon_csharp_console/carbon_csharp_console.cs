//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Threading.Tasks;

namespace CarbonSamples
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("Usage: carbon_csharp_console speechKey mic|file model:modelId|lang:language");
                Environment.Exit(1);
            }

            var keySpeech = args[0];
            string fileName = null;
            bool useBaseModel = true;
            string lang = null;
            string modelId = null;

            if (args.Length >= 2)
            {
                if (string.Compare(args[1], "mic", true) == 0)
                {
                    fileName = null;
                }
                else
                {
                    fileName = args[1];
                }
            }

            if (args.Length >= 3)
            {
                if (args[2].ToLower().StartsWith("lang:"))
                {
                    useBaseModel = true;
                    var index = args[2].IndexOf(':');
                    if (index == -1)
                    {
                        throw new IndexOutOfRangeException("no language is specified.");
                    }
                    lang = args[2].Substring(index + 1);
                    if (String.IsNullOrEmpty(lang))
                    {
                        throw new IndexOutOfRangeException("no language is specified.");
                    }
                }
                else if (args[2].ToLower().StartsWith("model:"))
                {
                    useBaseModel = false;
                    var index = args[2].IndexOf(':');
                    if (index == -1)
                    {
                        throw new IndexOutOfRangeException("no model is specified.");
                    }
                    modelId = args[2].Substring(index + 1);
                    if (String.IsNullOrEmpty(modelId))
                    {
                        throw new IndexOutOfRangeException("no model is specified.");
                    }
                }
                else
                {
                    throw new InvalidOperationException("Only lang:language or model:modleId is allowed.");
                }
            }


            if (useBaseModel)
            {
                Console.WriteLine("=============== Run speech recognition samples using base model. ===============");
                SpeechRecognitionSamples.SpeechRecognitionBaseModelAsync(keySpeech, fileName).Wait();

                Console.WriteLine("=============== Run intent recognoition samples using base speech model. ===============");
                IntentRecognitionSamples.IntentRecognitionBaseModelAsync(keySpeech, fileName).Wait();
            }
            else
            {
                Console.WriteLine("=============== Run speech recognition samples using customized model. ===============");
                SpeechRecognitionSamples.SpeechRecognitionCustomizedModelAsync(keySpeech, modelId, fileName).Wait();

                Console.WriteLine("=============== Run intent recognoition samples using customozed speech model. ===============");
                IntentRecognitionSamples.IntentRecognitionCustomizedModelAsync(keySpeech, modelId, fileName).Wait();
            }
        }
    }
}
