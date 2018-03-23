//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using Carbon.Recognition.Speech;

namespace Carbon.Recognition
{
    /// <summary>
    /// Defines factory methods to create recognizers.
    /// </summary>
    public class RecognizerFactory
    {
        /// <summary>
        /// Creates a speech recognizer using default settings. The audio input is from the default microphone device.
        /// </summary>
        /// <returns>A speech recognizer instance</returns>
        public static SpeechRecognizer CreateSpeechRecognizer()
        {
            var recoImpl = Internal.RecognizerFactory.CreateSpeechRecognizer();
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a speech recognizer using the specified language. The audio input is from the default microphone device.
        /// </summary>
        /// <param name="language">The language of audio input. It must be in BCP 47 format.</param>
        /// <returns>A speech recognizer instance</returns>
        public static SpeechRecognizer CreateSpeechRecognizer(string language)
        {
            var recoImpl = Internal.RecognizerFactory.CreateSpeechRecognizer(language);
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a speech recognizer using default settings. The audio input is from the specified file.
        /// </summary>
        /// <param name="fileName">The file name of audio input.</param>
        /// <returns>A speech recognizer instance</returns>
        public static SpeechRecognizer CreateSpeechRecognizerWithFileInput(string fileName)
        {
            var recoImpl = Internal.RecognizerFactory.CreateSpeechRecognizerWithFileInput(fileName);
            return new SpeechRecognizer(recoImpl);
        }

        /// <summary>
        /// Creates a speech recognizer using specific language. The audio input is from the specified file.
        /// </summary>
        /// <param name="fileName">The file name of audio input.</param>
        /// <param name="language">The language in BCP 47 format that the audio speaks.</param>
        /// <returns>A speech recognizer instance</returns>
        public static SpeechRecognizer CreateSpeechRecognizerWithFileInput(string fileName, string language)
        {
            var recoImpl = Internal.RecognizerFactory.CreateSpeechRecognizerWithFileInput(fileName, language);
            return new SpeechRecognizer(recoImpl);
        } 
    }
}
