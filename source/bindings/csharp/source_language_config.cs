//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Globalization;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Source Language configuration.
    /// Added in 1.8.1
    /// </summary>
    public class SourceLanguageConfig
    {
        internal InteropSafeHandle configHandle;

        internal SourceLanguageConfig(IntPtr configPtr)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.SourceLanguageConfig.source_lang_config_release);
        }

        /// <summary>
        /// Creates an instance of the SourceLanguageConfig with source language
        /// </summary>
        /// <param name="language">The source language</param>
        /// <returns>A new SourceLanguageConfig instance.</returns>
        public static SourceLanguageConfig FromLanguage(string language)
        {
            if (string.IsNullOrEmpty(language))
            {
                throw new ArgumentNullException(nameof(language));
            }
            IntPtr sourceLanguageConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SourceLanguageConfig.source_lang_config_from_language(out sourceLanguageConfigHandle, language));
            return new SourceLanguageConfig(sourceLanguageConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the SourceLanguageConfig with source language and custom endpoint id. A custom endpoint id corresponds to custom models. 
        /// </summary>
        /// <param name="language">The source language</param>
        /// <param name="endpointId">The custom endpoint id</param>
        /// <returns>A new SourceLanguageConfig instance.</returns>
        public static SourceLanguageConfig FromLanguage(string language, string endpointId)
        {
            if (string.IsNullOrEmpty(language))
            {
                throw new ArgumentNullException(nameof(language));
            }
            if (string.IsNullOrEmpty(endpointId))
            {
                throw new ArgumentNullException(nameof(endpointId));
            }
            IntPtr sourceLanguageConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.SourceLanguageConfig.source_lang_config_from_language_and_endpointId(out sourceLanguageConfigHandle, language, endpointId));
            return new SourceLanguageConfig(sourceLanguageConfigHandle);
        }
    }
}
