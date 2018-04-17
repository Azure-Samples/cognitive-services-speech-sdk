//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using Carbon.Recognition.Speech;
using Carbon.Recognition.Intent;
using Carbon.Recognition.Translation;

namespace Carbon.Recognition
{
    /// <summary>
    /// Factory methods to create recognizers.
    /// </summary>
    public sealed class RecognizerFactory : IDisposable
    {
        /// <summary>
        /// Creates an instance of recognizer factory.
        /// </summary>
        public RecognizerFactory()
        {
            InitInternal();
        }

        /// <summary>
        /// Creates an instance of recognizer factory with specified subscription key and region (optional).
        /// </summary>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <param name="region">The region name.</param>
        public RecognizerFactory(string subscriptionKey, string region = null)
        {
            InitInternal();
            SubscriptionKey = subscriptionKey;
            if (region != null)
            {
                Region = region;
            }
        }

        /// <summary>
        /// Gets/sets the subscription key.
        /// </summary>
        public string SubscriptionKey
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.SpeechSubscriptionKey);
            }

            set
            {
                // factoryImpl.SetSubscriptionKey(value);
                Parameters.Set(ParameterNames.SpeechSubscriptionKey, value);
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
                return Parameters.Get<string>(ParameterNames.SpeechAuthToken);
            }

            set
            {
                // factoryImpl.SetSubscriptionKey(value);
                Parameters.Set(ParameterNames.SpeechAuthToken, value);
            }
        }

        /// <summary>
        /// Gets/sets the region name of the service to be connected.
        /// </summary>
        public string Region
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.Region);
            }

            set
            {
                Parameters.Set(ParameterNames.Region, value);
            }
        }

        /// <summary>
        /// Gets/sets the service endpoint.
        /// </summary>
        public string Endpoint
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.SpeechEndpoint);
            }

            set
            {
                // factoryImpl.SetSubscriptionKey(value);
                Parameters.Set(ParameterNames.SpeechEndpoint, value);
            }
        }

        /// <summary>
        /// The collection of parameters and their values defined for this <see cref="RecognizerFactory"/>.
        /// </summary>
        public ParameterCollection<RecognizerFactory> Parameters { get; private set; }

        /// <summary>
        /// Creates a translation recognizer, using the default microphone input.
        /// </summary>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer()
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer());
        }

        /// <summary>
        /// Creates a translation recognizer, using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file.</param>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer(string audioFile)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates a translation recognizer, using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer(AudioInputStream audioStream)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates an intent recognizer, using the specified file as audio input.
        /// </summary>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizer()
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizer());
        }

        /// <summary>
        /// Creates an intent recognizer, using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file.</param>
        /// <returns>An intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizer(string audioFile)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates an intent recognizer, using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizer(AudioInputStream audioStream)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a translation recognizer, using the default microphone input.
        /// </summary>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizer(string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizer(sourceLanguage, AsWStringVector(targetLanguages)));
        }

        /// <summary>
        /// Creates a translation recognizer, using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizer(string audioFile, string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            return new TranslationRecognizer(factoryImpl.CreateTranslationRecognizerWithFileInput(audioFile, sourceLanguage, AsWStringVector(targetLanguages)));
        }

        /// <summary>
        /// Creates a translation recognizer, using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizer(AudioInputStream audioStream, string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Dispose of associated resources.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            Parameters.Dispose();
            factoryImpl.Dispose();
            disposed = true;
        }

        private Carbon.Internal.IRecognizerFactory factoryImpl;
        private bool disposed = false;

        private void InitInternal()
        {
            Parameters = new ParameterCollection<RecognizerFactory>(this);
            factoryImpl = Carbon.Internal.RecognizerFactory.GetDefault();
        }

        private static Internal.WstringVector AsWStringVector(IEnumerable<string> input)
        {
            var inputVector = new Internal.WstringVector();
            foreach (var element in input)
            {
                inputVector.Add(element);
            }
            return inputVector;
        }
    }
}
