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
    /// Auto Detect Source Language configuration.
    /// Updated in 1.13.0
    /// </summary>
    public class AutoDetectSourceLanguageConfig
    {
        internal InteropSafeHandle configHandle;

        internal AutoDetectSourceLanguageConfig(IntPtr configPtr)
        {
            ThrowIfNull(configPtr);
            configHandle = new InteropSafeHandle(configPtr, Internal.AutoDetectSourceLanguageConfig.auto_detect_source_lang_config_release);
        }

        /// <summary>
        /// Creates an instance of the AutoDetectSourceLanguageConfig with open range
        /// Note: only <see cref="SpeechSynthesizer"/> supports source language auto detection from open range,
        /// for <see cref="Recognizer"/>, please use AutoDetectSourceLanguageConfig with specific source languages.
        /// Added in 1.13.0
        /// </summary>
        /// <returns>A new AutoDetectSourceLanguageConfig instance.</returns>
        public static AutoDetectSourceLanguageConfig FromOpenRange()
        {
            IntPtr autoDetectSourceLanguageConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AutoDetectSourceLanguageConfig.create_auto_detect_source_lang_config_from_open_range(out autoDetectSourceLanguageConfigHandle));
            return new AutoDetectSourceLanguageConfig(autoDetectSourceLanguageConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the AutoDetectSourceLanguageConfig with source languages
        /// </summary>
        /// <param name="languages">The list of source languages.</param>
        /// <returns>A new AutoDetectSourceLanguageConfig instance.</returns>
        public static AutoDetectSourceLanguageConfig FromLanguages(string[] languages)
        {
            if (languages == null || languages.Length == 0)
            {
                throw new ArgumentNullException(nameof(languages));
            }

            foreach (string language in languages)
            {
                if (string.IsNullOrEmpty(language))
                {
                    throw new ArgumentNullException("language in languages cannot be null or empty");
                }
            }
            IntPtr autoDetectSourceLanguageConfigHandle = IntPtr.Zero;
            ThrowIfFail(Internal.AutoDetectSourceLanguageConfig.create_auto_detect_source_lang_config_from_languages(out autoDetectSourceLanguageConfigHandle, string.Join(",", languages)));
            return new AutoDetectSourceLanguageConfig(autoDetectSourceLanguageConfigHandle);
        }

        /// <summary>
        /// Creates an instance of the AutoDetectSourceLanguageConfig with a list of source language config
        /// </summary>
        /// <param name="sourceLanguageConfigs">The list of source languages config</param>
        /// <returns>A new AutoDetectSourceLanguageConfig instance.</returns>
        public static AutoDetectSourceLanguageConfig FromSourceLanguageConfigs(SourceLanguageConfig[] sourceLanguageConfigs)
        {
            if (sourceLanguageConfigs == null || sourceLanguageConfigs.Length == 0)
            {
                throw new ArgumentNullException(nameof(sourceLanguageConfigs));
            }

            IntPtr autoDetectSourceLanguageConfigHandle = IntPtr.Zero;
            bool isFirst = true;
            foreach (SourceLanguageConfig sourceLanguageConfig in sourceLanguageConfigs)
            {
                if (sourceLanguageConfig == null)
                {
                    throw new ArgumentNullException("sourceLanguageConfig in sourceLanguageConfigs cannot be null");
                }
                if (isFirst)
                {
                    ThrowIfFail(Internal.AutoDetectSourceLanguageConfig.create_auto_detect_source_lang_config_from_source_lang_config(out autoDetectSourceLanguageConfigHandle, sourceLanguageConfig.configHandle));
                    isFirst = false;
                }
                else
                {
                    ThrowIfFail(Internal.AutoDetectSourceLanguageConfig.add_source_lang_config_to_auto_detect_source_lang_config(autoDetectSourceLanguageConfigHandle, sourceLanguageConfig.configHandle));
                }
            }
            return new AutoDetectSourceLanguageConfig(autoDetectSourceLanguageConfigHandle);
        }
    }
}
