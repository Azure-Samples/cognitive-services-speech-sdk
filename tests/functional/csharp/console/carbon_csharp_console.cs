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
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: carbon_csharp_console mode(speech|intent|translation) key audioinput(mic|filename) model:modelId|lang:language|endpoint:url");
                Environment.Exit(1);
            }
           
            string subKey = null;
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
                else if (string.Compare(modeStr, "translation", true) == 0)
                {
                    isTranslation = true;
                }
                else
                {
                    throw new InvalidOperationException("The specified mode is not supported: " + modeStr);
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
                    throw new InvalidOperationException("The specified mode is not supported with authorization token: " + modeStr);
                }

                if (args.Length >= 3)
                {
                    var audioInputStr = args[2];

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

#if false
                var factory = RecognizerFactory.Instance;
                if (useToken)
                {
                    token = GetToken(subKey).Result;
                    factory.AuthorizationToken = token;
                    Console.WriteLine("Use authorization token.");
                }
                else
                {
                    factory.SubscriptionKey = subKey;
                    Console.WriteLine("Use subscription key.");
                }
#endif

                if (isSpeechReco)
                {
                    if (useEndpoint)
                    {
                        Console.WriteLine("=============== Run speech recognition samples by specifying endpoint. ===============");
                        SpeechRecognitionSamples.SpeechRecognitionByEndpointAsync(subKey, endpoint, lang: lang, model: modelId, fileName: fileName, useStream: useStream).Wait();
                    }
                    else
                    {
                        if (useBaseModel)
                        {
                            Console.WriteLine("=============== Run speech recognition samples using base model. ===============");
                            SpeechRecognitionSamples.SpeechRecognitionBaseModelAsync(subKey, lang: lang, fileName: fileName, useStream: useStream, useToken: useToken).Wait();
                        }
                        else
                        {
                            Console.WriteLine("=============== Run speech recognition samples using customized model. ===============");
                            SpeechRecognitionSamples.SpeechRecognitionCustomizedModelAsync(subKey, lang, modelId, fileName, useStream: useStream, useToken: useToken).Wait();
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
                            IntentRecognitionSamples.IntentRecognitionBaseModelAsync(subKey, fileName).Wait();
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
                            TranslationSamples.TranslationBaseModelAsync(subKey, fileName, useStream: useStream).Wait();
                        }
                        else
                        {
                            Console.WriteLine("=============== Translation using CRIS model is not supported yet. ===============");
                        }
                    }
                }
            }
        }
    }
}
