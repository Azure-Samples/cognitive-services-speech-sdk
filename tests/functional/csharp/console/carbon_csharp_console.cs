//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Diagnostics;

namespace MicrosoftSpeechSDKSamples
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 3)
            {
                Console.WriteLine("Usage: carbon_csharp_console mode(speech|intent|translation:cont|single) key(key|token:key) region audioinput(mic|filename|stream:file) model:modelId|lang:language|endpoint:url");
                Environment.Exit(1);
            }

            string subKey = null;
            string region = null;
            string fileName = null;
            bool useToken = false;
            bool useBaseModel = true;
            bool useEndpoint = false;
            bool isSpeechReco = false;
            bool isIntentReco = false;
            bool isTranslation = false;
            string lang = null;
            string modelId = null;
            string endpoint = null;
            bool useStream = false;
            bool useContinuousRecognition = false;

            if (args.Length >= 2)
            {
                var index = args[0].IndexOf(':');
                var modeStr = (index == -1) ? args[0] : args[0].Substring(0, index);
                if (string.Compare(modeStr, "speech", true) == 0)
                {
                    isSpeechReco = true;
                }
                else if (string.Compare(modeStr, "intent", true) == 0)
                {
                    isIntentReco = true;
                }
                else if (string.Compare(modeStr, "translation", true) == 0)
                {
                    isTranslation = true;
                }
                else
                {
                    throw new InvalidOperationException("The specified mode is not supported: " + modeStr);
                }

                if (index != -1)
                {
                    if (isIntentReco || isTranslation)
                    {
                        throw new InvalidOperationException("Only speech reco supports selection of continuous or singleshot");
                    }

                    var str = args[0].Substring(index + 1);
                    if (string.Compare(str, "cont", true) == 0)
                    {
                        useContinuousRecognition = true;
                    }
                    else if (string.Compare(str, "single", true) == 0)
                    {
                        useContinuousRecognition = false;
                    }
                    else
                    {
                        throw new ArgumentException("only cont or single is supported.");
                    }
                }
            }

            if (args[1].ToLower().StartsWith("token:"))
            {
                var index = args[1].IndexOf(':');
                if (index == -1)
                {
                    throw new IndexOutOfRangeException("no key is specified.");
                }
                subKey = args[1].Substring(index + 1);
                useToken = true;
            }
            else
            {
                subKey = args[1];
            }

            Trace.Assert(isSpeechReco || isIntentReco || isTranslation);
            Trace.Assert(subKey != null);
            if (useToken && (isIntentReco || isTranslation))
            {
                throw new InvalidOperationException("The specified mode is not supported with authorization token: " + args[0]);
            }

            region = args[2];
            if (string.IsNullOrEmpty(region))
            {
                throw new ArgumentException("region may not be empty");
            }

            if (args.Length >= 4)
            {
                var audioInputStr = args[3];

                if (string.Compare(audioInputStr, "mic", true) == 0)
                {
                    fileName = null;
                }
                else if (audioInputStr.ToLower().StartsWith("stream:"))
                {
                    useStream = true;
                    var index = audioInputStr.IndexOf(':');
                    if (index == -1)
                    {
                        throw new IndexOutOfRangeException("No file name specified as stream input.");
                    }
                    fileName = audioInputStr.Substring(index + 1);
                    if (String.IsNullOrEmpty(fileName))
                    {
                        throw new IndexOutOfRangeException("No file name specified as stream input.");
                    }
                }
                else
                {
                    fileName = audioInputStr;
                }
            }

            if (args.Length >= 5)
            {
                var paraStr = args[4];
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
                    if (useToken)
                    {
                        throw new InvalidOperationException("Recognition with endpoint is not supported with authorization token.");
                    }

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
                    throw new InvalidOperationException("Only the following values are allowed: lang:language, model:modelId, endpoint:url.");
                }
            }

            if (isSpeechReco)
            {
                if (useEndpoint)
                {
                    Console.WriteLine("=============== Run speech recognition samples by specifying endpoint. ===============");
                    SpeechRecognitionSamples.SpeechRecognitionByEndpointAsync(subKey, endpoint, lang: lang, model: modelId, fileName: fileName, useStream: useStream, useContinuousRecognition: useContinuousRecognition).Wait();
                }
                else
                {
                    if (useBaseModel)
                    {
                        Console.WriteLine("=============== Run speech recognition samples using base model. ===============");
                        SpeechRecognitionSamples.SpeechRecognitionBaseModelAsync(subKey, region: region, lang: lang, fileName: fileName, useStream: useStream, useToken: useToken, useContinuousRecognition: useContinuousRecognition).Wait();
                    }
                    else
                    {
                        Console.WriteLine("=============== Run speech recognition samples using customized model. ===============");
                        SpeechRecognitionSamples.SpeechRecognitionCustomizedModelAsync(subKey, region, lang, modelId, fileName, useStream: useStream, useToken: useToken, useContinuousRecognition: useContinuousRecognition).Wait();
                    }
                }
            }
            else if (isIntentReco)
            {
                if (useEndpoint)
                {
                    Console.WriteLine("=============== Run intent recognition samples by specifying endpoint. ===============");
                    IntentRecognitionSamples.IntentRecognitionByEndpointAsync(subKey, endpoint, fileName).Wait();
                }
                else
                {
                    if (useBaseModel)
                    {
                        Console.WriteLine("=============== Run intent recognition samples using base speech model. ===============");
                        IntentRecognitionSamples.IntentRecognitionBaseModelAsync(subKey, region, fileName).Wait();
                    }
                    else
                    {
                        Console.WriteLine("=============== Intent recognition with CRIS model is not supported yet. ===============");
                    }
                }
            }
            else if (isTranslation)
            {
                if (useEndpoint)
                {
                    Console.WriteLine("=============== Run translation samples by specifying endpoint. ===============");
                    TranslationSamples.TranslationByEndpointAsync(subKey, endpoint, fileName, useStream: useStream).Wait();
                }
                else
                {
                    if (useBaseModel)
                    {
                        Console.WriteLine("=============== Run translationsamples using base speech model. ===============");
                        TranslationSamples.TranslationBaseModelAsync(subKey, fileName: fileName, region: region, useStream: useStream).Wait();
                    }
                    else
                    {
                        Console.WriteLine("=============== Translation using CRIS model is not supported yet. ===============");
                    }
                }
            }
        }

        static void UnhandledExceptionTrapper(object sender, UnhandledExceptionEventArgs e) {
            Console.WriteLine(e.ExceptionObject.ToString());
            Environment.Exit(1);
        }
    }
}

