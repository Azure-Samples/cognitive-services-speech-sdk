//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using Microsoft.CognitiveServices.Speech.Recognition.Speech;
using Microsoft.CognitiveServices.Speech.Recognition.Intent;
using Microsoft.CognitiveServices.Speech.Recognition.Translation;

namespace Microsoft.CognitiveServices.Speech.Recognition
{
    /// <summary>
    /// Factory methods to create recognizers.
    /// </summary>
    public sealed class RecognizerFactory
    {
        /// <summary>
        /// Creates an instance of recognizer factory. Currently as private method.
        /// </summary>
        private RecognizerFactory()
        {
            InitInternal();
        }

        /// <summary>
        /// Creates an instance of recognizer factory with specified subscription key and region (optional). Currently as private method.
        /// </summary>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <param name="region">The region name.</param>
        private RecognizerFactory(string subscriptionKey, string region)
        {
            InitInternal();
            SubscriptionKey = subscriptionKey;
            Region = region;
        }

        /// <summary>
        /// Gets the default recognizer factory.
        /// </summary>
        public static RecognizerFactory Instance
        {
            get { return instanceLazy.Value; }
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
                factoryImpl.SetSubscriptionKey(value);
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
                factoryImpl.SetAuthorizationToken(value);
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
                factoryImpl.SetRegion(value);
            }
        }

        /// <summary>
        /// Gets/Sets the service endpoint when connecting to the service.
        /// </summary>
        public Uri EndpointURL
        {
            get
            {
                var endpointStr = Parameters.Get<string>(ParameterNames.SpeechEndpoint);
                return new Uri(endpointStr);
            }

            set
            {
                factoryImpl.SetEndpointUrl(value.AbsoluteUri);
            }
        }

        /// <summary>
        /// The collection of parameters and their values defined for this <see cref="RecognizerFactory"/>.
        /// </summary>
        public ParameterCollection<RecognizerFactory> Parameters { get; private set; }

        /// <summary>
        /// Creates a speech recognizer, using the default microphone input.
        /// </summary>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer()
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer());
        }

        /// <summary>
        /// Creates a speech recognizer, using the default microphone input.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizer(string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizer(language));
        }

        /// <summary>
        /// Creates a speech recognizer, using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates a speech recognizer, using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string audioFile, string language)
        {
            return new SpeechRecognizer(factoryImpl.CreateSpeechRecognizerWithFileInput(audioFile, language));
        }

        /// <summary>
        /// Creates a speech recognizer, using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithAudioStream(AudioInputStream audioStream)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates a speech recognizer, using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>A translation recognizer instance.</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithAudioStream(AudioInputStream audioStream, string language)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Creates an intent recognizer, using the default microphone input.
        /// </summary>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizer()
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizer());
        }

        /// <summary>
        /// Creates an intent recognizer, using the default microphone input.
        /// </summary>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizer(string language)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizer(language));
        }

        /// <summary>
        /// Creates an intent recognizer, using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <returns>An intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizerWithFileInput(string audioFile)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile));
        }

        /// <summary>
        /// Creates an intent recognizer, using the specified file as audio input.
        /// </summary>
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>An intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizerWithFileInput(string audioFile, string language)
        {
            return new IntentRecognizer(factoryImpl.CreateIntentRecognizerWithFileInput(audioFile, language));
        }

        /// <summary>
        /// Creates an intent recognizer, using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizerWithStream(AudioInputStream audioStream)
        {
            throw new NotImplementedException();
        }


        /// <summary>
        /// Creates an intent recognizer, using the specified input stream as audio input.
        /// </summary>
        /// <param name="audioStream">Specifies the audio input stream.</param>
        /// <param name="language">Specifies the name of spoken language to be recognized in BCP-47 format.</param>
        /// <returns>An intent recognizer instance.</returns>
        public IntentRecognizer CreateIntentRecognizerWithStream(AudioInputStream audioStream, string language)
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
        /// <param name="audioFile">Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 KHz sample rate, and a single channel (Mono) is supported.</param>
        /// <param name="sourceLanguage">The spoken language that needs to be translated.</param>
        /// <param name="targetLanguages">The target languages of translation.</param>
        /// <returns>A translation recognizer instance.</returns>
        public TranslationRecognizer CreateTranslationRecognizerWithFileInput(string audioFile, string sourceLanguage, IEnumerable<string> targetLanguages)
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
        public TranslationRecognizer CreateTranslationRecognizerWithStream(AudioInputStream audioStream, string sourceLanguage, IEnumerable<string> targetLanguages)
        {
            throw new NotImplementedException();
        }

        private static readonly Lazy<RecognizerFactory> instanceLazy = 
            new Lazy<RecognizerFactory>(() => new RecognizerFactory(), true);

        private Microsoft.CognitiveServices.Speech.Internal.IRecognizerFactory factoryImpl;

        private void InitInternal()
        {
            Parameters = new ParameterCollection<RecognizerFactory>(this);
            factoryImpl = Microsoft.CognitiveServices.Speech.Internal.RecognizerFactory.GetDefault();
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
