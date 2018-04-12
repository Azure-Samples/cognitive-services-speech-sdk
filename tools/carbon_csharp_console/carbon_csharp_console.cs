//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace CarbonSamples
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: carbon_csharp_console mode(speech|intent) key audioinput(mic|filename) model:modelId|lang:language|endpoint:url");
                Environment.Exit(1);
            }

            string keySpeech = null;
            string fileName = null;
            bool useBaseModel = true;
            bool useEndpoint = false;
            bool isSpeechReco = false;
            bool isIntentReco = false;
            string lang = null;
            string modelId = null;
            string endpoint = null;

            if (args.Length >= 2)
            {
                var modeStr = args[0];
                if (string.Compare(modeStr, "speech", true) == 0)
                {
                    isSpeechReco = true;
                }
                else if (string.Compare(modeStr, "intent", true) == 0)
                {
                    isIntentReco = true;
                }
                else
                {
                    throw new InvalidOperationException("The specified mode is not supported: " + modeStr);
                }

                keySpeech = args[1];
            }

            Debug.Assert(isSpeechReco || isIntentReco);
            Debug.Assert(keySpeech != null);

            if (args.Length >= 3)
            {
                var audioInputStr = args[2];

                if (string.Compare(audioInputStr, "mic", true) == 0)
                {
                    fileName = null;
                }
                else
                {
                    fileName = audioInputStr;
                }
            }

            if (args.Length >= 4)
            {
                var paraStr = args[3];
                if (paraStr.ToLower().StartsWith("lang:"))
                {
                    useBaseModel = true;
                    var index = paraStr.IndexOf(':');
                    if (index == -1)
                    {
                        throw new IndexOutOfRangeException("no language is specified.");
                    }
                    lang = paraStr.Substring(index + 1);
                    if (String.IsNullOrEmpty(lang))
                    {
                        throw new IndexOutOfRangeException("no language is specified.");
                    }
                }
                else if (paraStr.ToLower().StartsWith("model:"))
                {
                    useBaseModel = false;
                    var index = paraStr.IndexOf(':');
                    if (index == -1)
                    {
                        throw new IndexOutOfRangeException("no model is specified.");
                    }
                    modelId = paraStr.Substring(index + 1);
                    if (String.IsNullOrEmpty(modelId))
                    {
                        throw new IndexOutOfRangeException("no model is specified.");
                    }
                }
                else if (paraStr.ToLower().StartsWith("endpoint:"))
                {
                    useEndpoint = true;
                    var index = paraStr.IndexOf(':');
                    if (index == -1)
                    {
                        throw new IndexOutOfRangeException("no endpoint is specified.");
                    }
                    endpoint = paraStr.Substring(index + 1);
                    if (String.IsNullOrEmpty(endpoint))
                    {
                        throw new IndexOutOfRangeException("no endpoint is specified.");
                    }
                }
                else
                {
                    throw new InvalidOperationException("Only the following value is allowed: lang:languageor, model:modleId, endpoint:url.");
                }
            }

            if (isSpeechReco)
            {
                if (useEndpoint)
                {
                    Console.WriteLine("=============== Run speech recognoition samples by specifying endpoint. ===============");
                    SpeechRecognitionSamples.SpeechRecognitionByEndpointAsync(keySpeech, endpoint, fileName).Wait();
                }
                else
                {
                    if (useBaseModel)
                    {
                        Console.WriteLine("=============== Run speech recognition samples using base model. ===============");
                        SpeechRecognitionSamples.SpeechRecognitionBaseModelAsync(keySpeech, fileName).Wait();
                    }
                    else
                    {
                        Console.WriteLine("=============== Skip CRIS model for now. =======");
                        //Console.WriteLine("=============== Run speech recognition samples using customized model. ===============");
                        //SpeechRecognitionSamples.SpeechRecognitionCustomizedModelAsync(keySpeech, modelId, fileName).Wait();
                    }
                }
            }
            else if (isIntentReco)
            {
                if (useEndpoint)
                {
                    Console.WriteLine("=============== Run intent recognoition samples by specifying endpoint. ===============");
                    IntentRecognitionSamples.IntentRecognitionByEndpointAsync(keySpeech, endpoint, fileName).Wait();
                }
                else
                {
                    if (useBaseModel)
                    {
                        Console.WriteLine("=============== Run intent recognoition samples using base speech model. ===============");
                        IntentRecognitionSamples.IntentRecognitionBaseModelAsync(keySpeech, fileName).Wait();
                    }
                    else
                    {
                        Console.WriteLine("=============== Skip CRIS model for now. =======");
                        //Console.WriteLine("=============== Run intent recognoition samples using customozed speech model. ===============");
                        //IntentRecognitionSamples.IntentRecognitionCustomizedModelAsync(keySpeech, modelId, fileName).Wait();
                    }
                }
            }
        }
    }
}
