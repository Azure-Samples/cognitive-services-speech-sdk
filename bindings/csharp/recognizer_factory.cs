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
        /// The name of parameter "SubscriptionKey"
        /// </summary>
        public const string SubscriptionKeyParameterName = "SubscriptionKey";

        /// <summary>
        /// The name of parameter "Region"
        /// </summary>
        public const string RegionParameterName = "Region";

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
                return Parameters.Get<string>(SubscriptionKeyParameterName);
            }

            set
            {
                Parameters.Set(SubscriptionKeyParameterName, value);
            }
        }

        /// <summary>
        /// The property represents the region being used.
        /// </summary>
        public string Region
        {
            get
            {
                return Parameters.Get<string>(RegionParameterName);
            }

            set
            {
                Parameters.Set(RegionParameterName, value);
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
        /// The property represents the collections of defined parameters and their values.
        /// </summary>
        public ParameterCollection<RecognizerFactory> Parameters { get; }

        /// <summary>
        /// Creates a speech recognizer using default settings. The audio input is from the default microphone device.
        /// </summary>
        /// <returns>A speech recognizer instance</returns>
        public SpeechRecognizer CreateSpeechRecognizer()
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateSpeechRecognizer();
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a speech recognizer using the specified language. The audio input is from the default microphone device.
        /// </summary>
        /// <param name="language">The language of audio input. It must be in BCP 47 format.</param>
        /// <returns>A speech recognizer instance</returns>
        public SpeechRecognizer CreateSpeechRecognizer(string language)
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateSpeechRecognizer(language);
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a speech recognizer using default settings. The audio input is from the specified file.
        /// </summary>
        /// <param name="fileName">The file name of audio input.</param>
        /// <returns>A speech recognizer instance</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string fileName)
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateSpeechRecognizerWithFileInput(fileName);
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a speech recognizer using specific language. The audio input is from the specified file.
        /// </summary>
        /// <param name="fileName">The file name of audio input.</param>
        /// <param name="language">The language in BCP 47 format that the audio speaks.</param>
        /// <returns>A speech recognizer instance</returns>
        public SpeechRecognizer CreateSpeechRecognizerWithFileInput(string fileName, string language)
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateSpeechRecognizerWithFileInput(fileName, language);
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a intent recognizer using default settings. The audio input is from the default microphone device.
        /// </summary>
        /// <returns>A intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizer()
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateIntentRecognizer();
            return new IntentRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a intent recognizer using the specified language. The audio input is from the default microphone device.
        /// </summary>
        /// <param name="language">The audio input language. It must be in BCP 47 format.</param>
        /// <returns>A intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizer(string language)
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateIntentRecognizer(language);
            return new IntentRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a intent recognizer using default settings. The audio input is from the specified file.
        /// </summary>
        /// <param name="fileName">The file name of audio input.</param>
        /// <returns>A intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizerWithFileInput(string fileName)
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateIntentRecognizerWithFileInput(fileName);
            return new IntentRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a intent recognizer using specific language. The audio input is from the specified file.
        /// </summary>
        /// <param name="fileName">The file name of audio input.</param>
        /// <param name="language">The audio input language in BCP 47 format.</param>
        /// <returns>A intent recognizer instance</returns>
        public IntentRecognizer CreateIntentRecognizerWithFileInput(string fileName, string language)
        {
            var factory = new Internal.RecognizerFactory();
            var recoImpl = factory.CreateIntentRecognizerWithFileInput(fileName, language);
            return new IntentRecognizer(recoImpl);
        }
    }
}
