//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Diagnostics;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines kind of recognizer parameters.
    /// </summary>
    public enum RecognizerParameterKind
    {
        DeploymentId = Internal.SpeechPropertyId.SpeechServiceConnection_DeploymentId,
        OutputFormat = Internal.SpeechPropertyId.SpeechServiceResponse_OutputFormat
    }

    /// <summary>
    /// Defines name of speech recognizer parameters.
    /// </summary>
    public class SpeechParameterNames
    {
        /// <summary>
        /// The name of parameter for getting/setting deployment id of a customized model.
        /// </summary>
        public const string DeploymentId = "SPEECH-ModelId";

        /// <summary>
        /// The name of parameter for getting/setting language of recognition.
        /// </summary>
        public const string RecognitionLanguage = "SPEECH-RecoLanguage";

        /// <summary>
        /// The name of parameter for getting/setting language of recognition.
        /// </summary>
        public const string OutputFormat = "SPEECH-OutputFormat";
    }

    internal class OutputFormatParameterValues
    {
        /// <summary>
        /// The name of simple output format.
        /// </summary>
        public const string Simple = "SIMPLE";

        /// <summary>
        /// The name of detailed output format.
        /// </summary>
        public const string Detailed = "DETAILED";
    }

    /// <summary>
    /// Defines name of translation parameters.
    /// </summary>
    public class TranslationParameterNames
    {
        /// <summary>
        /// The name of parameter for getting/setting source language of translation.
        /// </summary>
        public const string SourceLanguage = "TRANSLATION-FromLanguage";

        /// <summary>
        /// The name of parameter for getting/setting target languages of translation.
        /// </summary>
        public const string TargetLanguages = "TRANSLATION-ToLanguages";

        /// <summary>
        /// The name of parameter for getting/setting voice name to generate synthesized audio output of the translated text.
        /// </summary>
        public const string Voice = "TRANSLATION-Voice";

        /// <summary>
        /// The name of parameter for getting/setting features of translation recognizer.
        /// </summary>
        public const string Features = "TRANSLATION-Features";

    }
}
