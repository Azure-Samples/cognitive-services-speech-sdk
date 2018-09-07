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
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech factory instance.</returns>
        public static SpeechFactory FromSubscription(string subscriptionKey, string region)
        {
            return new SpeechFactory(Internal.SpeechFactory.FromSubscription(subscriptionKey, region));
        }

        /// <summary>
        /// Creates an instance of the speech factory with specified authorization token and region.
        /// Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
        /// expipres, the caller needs to refresh it by setting the property `AuthorizationToken` with a new valid token.
        /// Otherwise, all the recognizers created by this SpeechFactory instance will encounter errors during recognition.
        /// </summary>
        /// <param name="authorizationToken">The authorization token.</param>
        /// <param name="region">The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).</param>
        /// <returns>A speech factory instance.</returns>
        public static SpeechFactory FromAuthorizationToken(string authorizationToken, string region)
        {
            return new SpeechFactory(Internal.SpeechFactory.FromAuthorizationToken(authorizationToken, region));
        }

        /// <summary>
        /// Creates an instance of the speech factory with specified endpoint and subscription key.
        /// This method is intended only for users who use a non-standard service endpoint or paramters.
        /// Note: The query parameters specified in the endpoint URL are not changed, even if they are set by any other APIs.
        /// For example, if language is defined in the uri as query parameter "language=de-DE", and also set by CreateSpeechRecognizer("en-US"),
        /// the language setting in uri takes precedence, and the effective language is "de-DE".
        /// Only the parameters that are not specified in the endpoint URL can be set by other APIs.
        /// </summary>
        /// <param name="endpoint">The service endpoint to connect to.</param>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <returns>A speech factory instance.</returns>
        public static SpeechFactory FromEndPoint(Uri endpoint, string subscriptionKey)
        {
            return new SpeechFactory(Internal.SpeechFactory.FromEndpoint(endpoint.ToString(), subscriptionKey));
        }

        /// <summary>
        /// Gets the subscription key.
        /// </summary>
        public string SubscriptionKey
        {
            get
            {
                return Parameters.Get(FactoryParameterNames.SubscriptionKey);
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
                return Parameters.Get(FactoryParameterNames.AuthorizationToken);
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
                return Parameters.Get(FactoryParameterNames.Region);
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
        /// Creates a speech recognizer using the default microphone input for a specified language.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer(string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language));
        }

        /// <summary>
        /// Creates a speech recognizer using the default microphone input for a specified language and a specified output format.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <param name="format">Output format: simple or detailed.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer(string language, OutputFormat format)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language,
                format == OutputFormat.Simple ? Internal.OutputFormat.Simple : Internal.OutputFormat.Detailed));
        }

        /// <summary>
        /// Creates a speech recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates a speech recognizer using the specified file as audio input for a specified language.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile, string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile, language));
        }

        /// <summary>
        /// Creates a speech recognizer using the specified file as audio input for a specified language and a specified output format.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <param name="format">Output format: simple or detailed.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile, string language, OutputFormat format)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile, language,
                format == OutputFormat.Simple ? Internal.OutputFormat.Simple : Internal.OutputFormat.Detailed));
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
        /// Creates a speech recognizer using the specified input stream as audio input for a specified language.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithStream(AudioInputStream audioStream, string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.Forwarder, language), audioStream);
        }

        /// <summary>
        /// Creates a speech recognizer using the specified input stream as audio input for a specified language and a specified output format.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <param name="format">Output format: simple or detailed.</param>
        /// <returns>A speech recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithStream(AudioInputStream audioStream, string language, OutputFormat format)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithStreamImpl(audioStream.Forwarder, language,
                format == OutputFormat.Simple ? Internal.OutputFormat.Simple : Internal.OutputFormat.Detailed), audioStream);
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
        /// Creates an intent recognizer using the default microphone input for a specified language.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizer(string language)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language));
        }

        /// <summary>
        /// Creates an intent recognizer using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>An intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizerWithFileInput(string audioFile)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates an intent recognizer using the specified file as audio input for a specified language.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>An intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizerWithFileInput(string audioFile, string language)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile, language));
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
        /// Creates an intent recognizer using the specified input stream as audio input for a specified language.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizerWithStream(AudioInputStream audioStream, string language)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithStreamImpl(audioStream.Forwarder, language), audioStream);
        }

        /// <summary>
        /// Creates a translation recognizer using the default microphone input for a specified source language and a specified list of target languages.
        /// </summary>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizer(string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, AsStringVector(targetLanguages)));
        }

        /// <summary>
        /// Creates a translation recognizer using the default microphone input for a specified source language, a specified list of target languages and a specified output voice.
        /// </summary>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizer(string sourceLanguage, IEnumerable<string> targetLanguages, string voice)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, AsStringVector(targetLanguages), voice));
        }

        /// <summary>
        /// Creates a translation recognizer using the specified file as audio input for a specified source language and a specified list of target languages.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithFileInput(string audioFile, string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithFileInput(audioFile, sourceLanguage, AsStringVector(targetLanguages)));
        }

        /// <summary>
        /// Creates a translation recognizer using the specified file as audio input for a specified source language, a specified list of target languages and a specified output voice.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithFileInput(string audioFile, string sourceLanguage, IEnumerable<string> targetLanguages, string voice)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithFileInput(audioFile, sourceLanguage, AsStringVector(targetLanguages), voice));
        }

        /// <summary>
        /// Creates a translation recognizer using the specified input stream as audio input for a specified source language and a specified list of target languages.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithStream(AudioInputStream audioStream, string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithStreamImpl(audioStream.Forwarder, sourceLanguage, AsStringVector(targetLanguages)), audioStream);
        }

        /// <summary>
        /// Creates a translation recognizer using the specified input stream as audio input for a specified source language, a specified list of target languages and a specified output voice.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <param name="voice">Specifies the name of voice tag if a synthesized audio output is desired.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithStream(AudioInputStream audioStream, string sourceLanguage, IEnumerable<string> targetLanguages, string voice)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithStreamImpl(audioStream.Forwarder, sourceLanguage, AsStringVector(targetLanguages), voice), audioStream);
        }

        private SpeechFactory(Internal.ICognitiveServicesSpeechFactory factoryImpl)
        {
            this.factoryImpl = factoryImpl;
            InitParameters();
        }

        private void InitParameters()
        {
            Parameters = new FactoryParametersImpl(factoryImpl.Parameters);
        }

        // Hold the reference.
        internal Internal.ICognitiveServicesSpeechFactory factoryImpl;

        private static Internal.StringVector AsStringVector(IEnumerable<string> input)
        {
            var inputVector = new Internal.StringVector();
            foreach (var element in input)
            {
                inputVector.Add(element);
            }
            return inputVector;
        }

        private class FactoryParametersImpl : IFactoryParameters
        {
            private Internal.FactoryPropertyCollection factoryParameterImpl;

            public FactoryParametersImpl(Internal.FactoryPropertyCollection internalFactoryParameters)
            {
                factoryParameterImpl = internalFactoryParameters;
            }

            public string Get(FactoryParameterKind propertyKind)
            {
                return Get(propertyKind, string.Empty);
            }

            public string Get(string propertyName)
            {
                return Get(propertyName, string.Empty);
            }

            public string Get(FactoryParameterKind propertyKind, string defaultValue)
            {
                return factoryParameterImpl.GetProperty((Internal.SpeechPropertyId)propertyKind, defaultValue); 
            }

            public string Get(string propertyName, string defaultValue)
            {
                return factoryParameterImpl.GetProperty(propertyName, defaultValue);
            }

            public void Set(FactoryParameterKind propertyKind, string value)
            {
                factoryParameterImpl.SetProperty((Internal.SpeechPropertyId)propertyKind, value);
            }

            public void Set(string propertyName, string value)
            {
                factoryParameterImpl.SetProperty(propertyName, value);
            }
        }

    }
}
