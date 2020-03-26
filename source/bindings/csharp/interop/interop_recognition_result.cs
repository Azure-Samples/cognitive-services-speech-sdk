//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    internal static class RecognitionResult
    {

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR result_get_reason(InteropSafeHandle result, ref ResultReason reason);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR result_get_result_id(InteropSafeHandle result, IntPtr resultId, UInt32 resultIdLength);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR result_get_text(InteropSafeHandle result, IntPtr text, UInt32 textLength);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR result_get_offset(InteropSafeHandle result, ref UInt64 offset);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR result_get_duration(InteropSafeHandle result, ref UInt64 duration);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR result_get_property_bag(InteropSafeHandle result, out SPXPROPERTYBAGHANDLE hpropbag);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool recognizer_result_handle_is_valid(InteropSafeHandle result);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_result_handle_release(SPXRESULTHANDLE result);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR translation_text_result_get_translation_text_buffer_header(InteropSafeHandle handle, IntPtr translationResultBuffer, ref int lengthPointer);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR translation_synthesis_result_get_audio_data(InteropSafeHandle handle, IntPtr audioBuffer, ref int lengthPointer);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR intent_result_get_intent_id(InteropSafeHandle result, IntPtr intentId, UInt32 intentIdLength);

    }
}
