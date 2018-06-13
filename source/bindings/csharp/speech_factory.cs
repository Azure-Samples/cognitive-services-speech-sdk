//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Globalization;

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Intent;
using Microsoft.CognitiveServices.Speech.Translation;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Factory methods to create recognizers.
    /// </summary>
    public sealed class SpeechFactory
    {
        /// <summary>
        /// Creates an instance of the speech factory with specified subscription key and region.
        /// </summary>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <param name="region">The region name.</param>
        /// <returns>A speech factory instance.</returns>
        public static SpeechFactory FromSubscription(string subscriptionKey, string region)
        {
            SpeechFactory factory = new SpeechFactory(subscriptionKey, region);
            return factory;
        }

        /// <summary>
        /// Creates an instance of the speech factory with specified endpoint and subscription key.
        /// This method is intended only for users who use a non-standard service endpoint.
        /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
        /// For example, if language is defined in uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
        /// the language setting in uri takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A speech factory instance.</returns>
        public static SpeechFactory FromEndPoint(Uri endpoint, string subscriptionKey)
        {
            SpeechFactory factory = new SpeechFactory(endpoint, subscriptionKey);
            return factory;
        }

        /// <summary>
        /// Gets the subscription key.
        /// </summary>
        public string SubscriptionKey
        {
            get
            {
                return Parameters.Get<string>(FactoryParameterNames.SubscriptionKey);
            }
        }

        /// <summary>
        /// Gets/sets the authorization token.
        /// If this is set, subscription key is ignored.
        /// User needs to make sure the provided authorization token is valid and not expired.
        /// </summary>
        public string AuthorizationToken
        {
            get
            {
                return Parameters.Get<string>(FactoryParameterNames.AuthorizationToken);
            }

            set
            {
                Parameters.Set(FactoryParameterNames.AuthorizationToken, value);
            }
        }

        /// <summary>
        /// Gets the region name of the service to be connected.
        /// </summary>
        public string Region
        {
            get
            {
                return Parameters.Get<string>(FactoryParameterNames.Region);
            }
        }

        /// <summary>
        /// Gets the service endpoint when connecting to the service.
        /// </summary>
        public Uri EndpointURL
        {
            get
            {
                var endpointStr = Parameters.Get<string>(FactoryParameterNames.Endpoint);
                return new Uri(endpointStr);
            }
        }

        /// <summary>
        /// The collection of parameters and their values defined for this <see cref="SpeechFactory"/>.
        /// </summary>
        public IFactoryParameters Parameters { get; private set; }

        /// <summary>
        /// Creates a speech recognizer, using the default microphone input.
        /// </summary>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer()
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer());
        }

        /// <summary>
        /// Creates a speech recognizer using the default microphone input.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer(string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language));
        }

        /// <summary>
        /// Creates a speech recognizer using the default microphone input.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <param name="format">Output format: simple or detailed.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer(string language, SpeechOutputFormat format)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language,
                format == SpeechOutputFormat.Simple ? Internal.OutputFormat.Simple : Internal.OutputFormat.Detailed));
        }

        /// <summary>
        /// Creates a speech recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates a speech recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile, string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile, language));
        }

        /// <summary>
        /// Creates a speech recognizer using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithStream(AudioInputStream audioStream)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.Forwarder), audioStream);
        }

        /// <summary>
        /// Creates a speech recognizer using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithStream(AudioInputStream audioStream, string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.Forwarder, language), audioStream);
        }

        /// <summary>
        /// Creates a speech recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <param name="format">Output format: simple or detailed.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile, string language, SpeechOutputFormat format)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile, language,
                format == SpeechOutputFormat.Simple ? Internal.OutputFormat.Simple : Internal.OutputFormat.Detailed));
        }

        /// <summary>
        /// Creates a speech recognizer using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <param name="format">Output format: simple or detailed.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithStream(AudioInputStream audioStream, string language, SpeechOutputFormat format)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.Forwarder, language,
                format == SpeechOutputFormat.Simple ? Internal.OutputFormat.Simple : Internal.OutputFormat.Detailed), audioStream);
        }

        /// <summary>
        /// Creates an intent recognizer using the default microphone input.
        /// </summary>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizer()
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizer());
        }

        /// <summary>
        /// Creates an intent recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>An intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizerWithFileInput(string audioFile)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates an intent recognizer using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizerWithStream(AudioInputStream audioStream)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithStreamImpl(audioStream.Forwarder), audioStream);
        }


        /// <summary>
        /// Creates an intent recognizer using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizerWithStream(AudioInputStream audioStream, string language)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithStreamImpl(audioStream.Forwarder, language), audioStream);
        }

        /// <summary>
        /// Creates a translation recognizer using the default microphone input.
        /// </summary>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizer(string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, AsWStringVector(targetLanguages)));
        }

        /// <summary>
        /// Creates a translation recognizer using the default microphone input.
        /// </summary>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizer(string sourceLanguage, IEnumerable<string> targetLanguages, string voice)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, AsWStringVector(targetLanguages), voice));
        }

        /// <summary>
        /// Creates a translation recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithFileInput(string audioFile, string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithFileInput(audioFile, sourceLanguage, AsWStringVector(targetLanguages)));
        }

        /// <summary>
        /// Creates a translation recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithFileInput(string audioFile, string sourceLanguage, IEnumerable<string> targetLanguages, string voice)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithFileInput(audioFile, sourceLanguage, AsWStringVector(targetLanguages), voice));
        }

        /// <summary>
        /// Creates a translation recognizer using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithStream(AudioInputStream audioStream, string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithStreamImpl(audioStream.Forwarder, sourceLanguage, AsWStringVector(targetLanguages)), audioStream);
        }

        /// <summary>
        /// Creates a translation recognizer using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithStream(AudioInputStream audioStream, string sourceLanguage, IEnumerable<string> targetLanguages, string voice)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithStreamImpl(audioStream.Forwarder, sourceLanguage, AsWStringVector(targetLanguages), voice), audioStream);
        }

        /// <summary>
        /// Creates an instance of the speech factory with specified subscription key and region. Currently as private method.
        /// </summary>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <param name="region">The region name.</param>
        private SpeechFactory(string subscriptionKey, string region)
        {
            factoryImpl = Internal.SpeechFactory.FromSubscription(subscriptionKey, region);
            InitParameters();
        }

        /// <summary>
        /// Creates an instance of the speech factory with specified endpoint and subscription key. Currently as private method.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        private SpeechFactory(Uri endpoint, string subscriptionKey)
        {
            factoryImpl = Internal.SpeechFactory.FromEndpoint(endpoint.ToString(), subscriptionKey);
            InitParameters();
        }

        private void InitParameters()
        {
            Parameters = new FactoryParametersImpl(factoryImpl.Parameters);
        }

        // Hold the reference.
        internal Internal.ICognitiveServicesSpeechFactory factoryImpl;

        private static Internal.WstringVector AsWStringVector(IEnumerable<string> input)
        {
            var inputVector = new Internal.WstringVector();
            foreach (var element in input)
            {
                inputVector.Add(element);
            }
            return inputVector;
        }

        private class FactoryParametersImpl : IFactoryParameters
        {
            private Internal.FactoryParameterCollection factoryParameterImpl;

            public FactoryParametersImpl(Internal.FactoryParameterCollection internalFactoryParameters)
            {
                factoryParameterImpl = internalFactoryParameters;
            }

            public bool Is<T>(FactoryParameterKind propertyKind)
            {
                if (typeof(T) == typeof(string))
                {
                    return factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).IsString();
                }
                else if (typeof(T) == typeof(int))
                {
                    return factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).IsNumber();
                }
                else if (typeof(T) == typeof(bool))
                {
                    return factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).IsBool();
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public bool Is<T>(string propertyName)
            {
                if (typeof(T) == typeof(string))
                {
                    return factoryParameterImpl.Get(propertyName).IsString();
                }
                else if (typeof(T) == typeof(int))
                {
                    return factoryParameterImpl.Get(propertyName).IsNumber();
                }
                else if (typeof(T) == typeof(bool))
                {
                    return factoryParameterImpl.Get(propertyName).IsBool();
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public T Get<T>(FactoryParameterKind propertyKind)
            {
                T defaultT;
                if (typeof(T) == typeof(string))
                {
                    defaultT = (T)Convert.ChangeType(string.Empty, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    defaultT = (T)Convert.ChangeType(0, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    defaultT = (T)Convert.ChangeType(false, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }

                return Get<T>(propertyKind, defaultT);
            }

            public T Get<T>(string propertyName)
            {
                T defaultT;
                if (typeof(T) == typeof(string))
                {
                    defaultT = (T)Convert.ChangeType(string.Empty, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    defaultT = (T)Convert.ChangeType(0, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    defaultT = (T)Convert.ChangeType(false, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }

                return Get<T>(propertyName, defaultT);
            }

            public T Get<T>(FactoryParameterKind propertyKind, T defaultValue)
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).GetString(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).GetNumber(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).GetBool(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public T Get<T>(string propertyName, T defaultValue)
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = factoryParameterImpl.Get(propertyName).GetString(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = factoryParameterImpl.Get(propertyName).GetNumber(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = factoryParameterImpl.Get(propertyName).GetBool(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public void Set(FactoryParameterKind propertyKind, string value)
            {
                factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).SetString(value);
            }

            public void Set(string propertyName, string value)
            {
                factoryParameterImpl.Get(propertyName).SetString(value);
            }

            public void Set(FactoryParameterKind propertyKind, int value)
            {
                factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).SetNumber(value);
            }

            public void Set(string propertyName, int value)
            {
                factoryParameterImpl.Get(propertyName).SetNumber(value);
            }

            public void Set(FactoryParameterKind propertyKind, bool value)
            {
                factoryParameterImpl.Get((Internal.FactoryParameter)propertyKind).SetBool(value);
            }

            public void Set(string propertyName, bool value)
            {
                factoryParameterImpl.Get(propertyName).SetBool(value);
            }
        }

    }
}
