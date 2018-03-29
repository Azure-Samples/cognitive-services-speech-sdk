//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Carbon.Recognition.Speech;
using Carbon.Recognition.Intent;

namespace Carbon.Recognition
{
     /// <summary>
     /// Defines factory methods to create recognizers.
     /// </summary>
     public class RecognizerFactory
     {
        /// <summary>
        /// Creates a recognizer factory.
        /// </summary>
        public RecognizerFactory()
        {
            Parameters = new ParameterCollection<RecognizerFactory>(this);
        }

        /// <summary>
        /// Creates a recognizer factory with specified subscription key and region (optional).
        /// </summary>
        /// <param name="subscriptionKey">The subscription key.</param>
        /// <param name="region">The region name.</param>
        public RecognizerFactory(string subscriptionKey, string region = null)
        {
            Parameters = new ParameterCollection<RecognizerFactory>(this);
            SubscriptionKey = subscriptionKey;
            if (region != null)
            {
                Region = region;
            }
        }

        /// <summary>
        /// The property represents the subscription key being used.
        /// </summary>
        public string SubscriptionKey
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.SubscriptionKey);
            }

            set
            {
                Parameters.Set(ParameterNames.SubscriptionKey, value);
            }
        }

        /// <summary>
        /// The property represents the region being used.
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
        /// The property represents the target language for the recognition.
        /// </summary>
        public string Language
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.Language);
            }

            set
            {
                Parameters.Set(ParameterNames.Language, value);
            }
        }

        /// <summary>
        /// The property represents the recognition mode.
        /// </summary>
        public string RecognitionMode
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.RecognitionMode);
            }

            set
            {
                Parameters.Set(ParameterNames.RecognitionMode, value);
            }
        }

        /// <summary>
        /// The property represents the intput file name.
        /// </summary>
        public string InputFile
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.InputFile);
            }

            set
            {
                Parameters.Set(ParameterNames.InputFile, value);
            }
        }

        /// <summary>
        /// Sets the subscription key. It follows the builder pattern.
        /// </summary>
        /// <param name="key">The subscription key.</param>
        /// <returns>The current instance.</returns>
        public RecognizerFactory SetSubscriptionKey(string key)
        {
            SubscriptionKey = key;
            return this;
        }

        /// <summary>
        /// Sets the region. It follows the builder pattern.
        /// </summary>
        /// <param name="region">The region name.</param>
        /// <returns>The current instance.</returns>
        public RecognizerFactory SetRegion(string region)
        {
            Region = region;
            return this;
        }

        /// <summary>
        /// Sets the language parameter. It follows the builder pattern.
        /// </summary>
        /// <param name="lang">The input language name in BCP47 format.</param>
        /// <returns>The current instance.</returns>
        public RecognizerFactory SetLanguage(string lang)
        {
            Language = lang;
            return this;
        }

        /// <summary>
        /// Sets the recognition mode. It follows the builder pattern.
        /// </summary>
        /// <param name="mode">The recognition mode.</param>
        /// <returns>The current instance.</returns>
        public RecognizerFactory SetRecognitionMode(string mode)
        {
            RecognitionMode = mode;
            return this;
        }

        /// <summary>
        /// Sets the input file name. It follows the builder pattern.
        /// </summary>
        /// <param name="mode">The input file name.</param>
        /// <returns>The current instance.</returns>
        public RecognizerFactory SetInputFile(string fileName)
        {
            InputFile = fileName;
            return this;
        }

        /// <summary>
        /// The property represents the collections of defined parameters and their values.
        /// </summary>
        public ParameterCollection<RecognizerFactory> Parameters { get; }

        /// <summary>
        /// Creates a speech recognizer.
        /// </summary>
        /// <returns>A speech recognizer instance</returns>
        public SpeechRecognizer CreateSpeechRecognizer()
        {
            var recoImpl = Internal.DefaultRecognizerFactory.CreateSpeechRecognizer();
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a intent recognizer.
        /// </summary>
        /// <returns>A intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizer()
        {
            var recoImpl = Internal.DefaultRecognizerFactory.CreateIntentRecognizer();
            return new IntentRecognizer(recoImpl);
        }
    }
}
