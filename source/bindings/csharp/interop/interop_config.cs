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

    internal static class AudioConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_config_is_handle_valid(SPXAUDIOCONFIGHANDLE audioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_create_audio_input_from_default_microphone(out SPXAUDIOCONFIGHANDLE audioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_input_from_wav_file_name(out SPXAUDIOCONFIGHANDLE audioConfig,
            IntPtr fileName);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_input_from_a_microphone(out SPXAUDIOCONFIGHANDLE audioConfig,
            [MarshalAs(UnmanagedType.LPStr)] string deviceName);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_create_audio_input_from_stream(out SPXAUDIOCONFIGHANDLE audioConfig, InteropSafeHandle audioStream);        
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_create_audio_output_from_default_speaker(out SPXAUDIOCONFIGHANDLE audioConfig);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR audio_config_create_audio_output_from_wav_file_name(out SPXAUDIOCONFIGHANDLE audioConfig,
            IntPtr fileName);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_create_audio_output_from_stream(out SPXAUDIOCONFIGHANDLE audioConfig, InteropSafeHandle audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_config_release(SPXAUDIOCONFIGHANDLE audioConfig);
    }

    internal static class SpeechConfig
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool speech_config_is_handle_valid(SPXSPEECHCONFIGHANDLE config);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_subscription(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string subscriptionKey,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_authorization_token(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string authToken,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR speech_config_from_endpoint(out SPXSPEECHCONFIGHANDLE config,
            [MarshalAs(UnmanagedType.LPStr)] string endpoint,
            [MarshalAs(UnmanagedType.LPStr)] string subscription);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR speech_config_release(SPXSPEECHCONFIGHANDLE config);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR speech_config_get_property_bag(InteropSafeHandle config, out SPXPROPERTYBAGHANDLE propbag);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR speech_config_set_audio_output_format(InteropSafeHandle config, SpeechSynthesisOutputFormat formatId);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR speech_config_set_service_property(InteropSafeHandle config,
            [MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.LPStr)] string value,
            ServicePropertyChannel channel);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR speech_config_set_profanity(InteropSafeHandle config,
            ProfanityOption profanity);
    }

}
