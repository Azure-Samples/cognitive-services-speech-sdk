//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXAUDIOCONFIGHANDLE = System.IntPtr;
    using SPXSPEECHCONFIGHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using SPXSOURCELANGCONFIGHANDLE = System.IntPtr;
    using SPXAUTODETECTSOURCELANGCONFIGHANDLE = System.IntPtr;

    internal static class AudioConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool audio_config_is_handle_valid(SPXAUDIOCONFIGHANDLE audioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_config_create_audio_input_from_default_microphone(out SPXAUDIOCONFIGHANDLE audioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_input_from_wav_file_name(out SPXAUDIOCONFIGHANDLE audioConfig,
            IntPtr fileName);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_input_from_a_microphone(out SPXAUDIOCONFIGHANDLE audioConfig,
            [MarshalAs(UnmanagedType.LPStr)] string deviceName);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_config_create_audio_input_from_stream(out SPXAUDIOCONFIGHANDLE audioConfig, InteropSafeHandle audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_config_create_audio_output_from_default_speaker(out SPXAUDIOCONFIGHANDLE audioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_output_from_wav_file_name(out SPXAUDIOCONFIGHANDLE audioConfig,
            IntPtr fileName);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_config_create_audio_output_from_stream(out SPXAUDIOCONFIGHANDLE audioConfig, InteropSafeHandle audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_config_release(SPXAUDIOCONFIGHANDLE audioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_config_get_property_bag(SPXAUDIOCONFIGHANDLE audioConfigHandle, out SPXPROPERTYBAGHANDLE propertyBagHandle);
    }

    internal static class SpeechConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool speech_config_is_handle_valid(SPXSPEECHCONFIGHANDLE config);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_subscription(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string subscriptionKey,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_authorization_token(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string authToken,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_endpoint(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string endpoint,
            [MarshalAs(UnmanagedType.LPStr)] string subscription);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_host(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string host,
            [MarshalAs(UnmanagedType.LPStr)] string subscription);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speech_config_release(SPXSPEECHCONFIGHANDLE config);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speech_config_get_property_bag(InteropSafeHandle config, out SPXPROPERTYBAGHANDLE propbag);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speech_config_set_audio_output_format(InteropSafeHandle config, SpeechSynthesisOutputFormat formatId);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speech_config_set_service_property(InteropSafeHandle config,
            [MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.LPStr)] string value,
            ServicePropertyChannel channel);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speech_config_set_profanity(InteropSafeHandle config,
            ProfanityOption profanity);
    }

    internal static class SpeechTranslationConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_translation_config_from_subscription(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string subscriptionKey,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_translation_config_from_authorization_token(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string authToken,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_translation_config_from_endpoint(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string endpoint,
            [MarshalAs(UnmanagedType.LPStr)] string subscription);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_translation_config_from_host(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string host,
            [MarshalAs(UnmanagedType.LPStr)] string subscription);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speech_translation_config_add_target_language(InteropSafeHandle config,
            [MarshalAs(UnmanagedType.LPStr)] string language);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR speech_translation_config_remove_target_language(InteropSafeHandle config, [MarshalAs(UnmanagedType.LPStr)] string language);
    }

    internal static class SourceLanguageConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR source_lang_config_from_language(out SPXSOURCELANGCONFIGHANDLE hconfig, [MarshalAs(UnmanagedType.LPStr)] string language);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR source_lang_config_from_language_and_endpointId(out SPXSOURCELANGCONFIGHANDLE hconfig, [MarshalAs(UnmanagedType.LPStr)] string language, [MarshalAs(UnmanagedType.LPStr)] string endpointId);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR source_lang_config_release(SPXSOURCELANGCONFIGHANDLE hconfig);
    }

    internal static class AutoDetectSourceLanguageConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR create_auto_detect_source_lang_config_from_open_range(out SPXAUTODETECTSOURCELANGCONFIGHANDLE hAutoDetectSourceLanguageconfig);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR create_auto_detect_source_lang_config_from_languages(out SPXAUTODETECTSOURCELANGCONFIGHANDLE hAutoDetectSourceLanguageconfig, [MarshalAs(UnmanagedType.LPStr)] string languages);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR create_auto_detect_source_lang_config_from_source_lang_config(out SPXAUTODETECTSOURCELANGCONFIGHANDLE hAutoDetectSourceLanguageconfig, InteropSafeHandle hSourceLanguageConfig);


        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR add_source_lang_config_to_auto_detect_source_lang_config(SPXAUTODETECTSOURCELANGCONFIGHANDLE hAutoDetectSourceLanguageconfig, InteropSafeHandle hSourceLanguageConfig);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR auto_detect_source_lang_config_release(SPXAUTODETECTSOURCELANGCONFIGHANDLE hAutoDetectSourceLanguageconfig);
    }
}
