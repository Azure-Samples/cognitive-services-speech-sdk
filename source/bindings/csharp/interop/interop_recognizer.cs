//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXASYNCHANDLE = System.IntPtr;
    using SPXRECOHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using SPXKEYWORDHANDLE = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;
    using SPXCONVERSATIONHANDLE = System.IntPtr;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void CallbackFunctionDelegate(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, IntPtr context);

    internal static class Recognizer
    {

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_get_property_bag(InteropSafeHandle recoHandle, out SPXPROPERTYBAGHANDLE propbag);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool recognizer_handle_is_valid(SPXRECOHANDLE recoHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_handle_release(SPXRECOHANDLE recoHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_enable(SPXRECOHANDLE recoHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_disable(SPXRECOHANDLE recoHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_is_enabled(SPXRECOHANDLE recoHandle, out bool enabled);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognize_once(InteropSafeHandle recoHandle, out SPXRESULTHANDLE result);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognize_once_async(SPXRECOHANDLE recoHandle, out SPXASYNCHANDLE asyncHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognize_once_async_wait_for(SPXASYNCHANDLE asyncHandle, UInt32 milliseconds, out SPXRESULTHANDLE phresult);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_start_continuous_recognition_async(InteropSafeHandle recoHandle, out SPXASYNCHANDLE asyncHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_start_continuous_recognition_async_wait_for(SPXASYNCHANDLE asyncHandle, UInt32 milliseconds);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_stop_continuous_recognition_async(InteropSafeHandle recoHandle, out SPXASYNCHANDLE asyncHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_stop_continuous_recognition_async_wait_for(SPXASYNCHANDLE asyncHandle, UInt32 milliseconds);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool recognizer_async_handle_is_valid(SPXASYNCHANDLE asyncHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_async_handle_release(SPXASYNCHANDLE asyncHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_start_keyword_recognition(SPXRECOHANDLE recoHandle, SPXKEYWORDHANDLE keyword);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_start_keyword_recognition_async(InteropSafeHandle recoHandle, InteropSafeHandle hkeyword, out SPXASYNCHANDLE asyncHandle);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_start_keyword_recognition_async_wait_for(SPXASYNCHANDLE asyncHandle, UInt32 milliseconds);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognize_keyword_once(InteropSafeHandle recoHandle, InteropSafeHandle keyword, out SPXRESULTHANDLE result);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognize_keyword_once_async(SPXRECOHANDLE recoHandle, InteropSafeHandle keyword, out SPXASYNCHANDLE asyncHandle);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognize_keyword_once_async_wait_for(SPXASYNCHANDLE asyncHandle, UInt32 milliseconds, out SPXRESULTHANDLE phresult);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_stop_keyword_recognition(SPXRECOHANDLE recoHandle);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_stop_keyword_recognition_async(InteropSafeHandle recoHandle, out SPXASYNCHANDLE asyncHandle);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_stop_keyword_recognition_async_wait_for(SPXASYNCHANDLE asyncHandle, UInt32 milliseconds);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognition_event_get_offset(InteropSafeHandle eventHandle, ref UInt64 offset);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR recognizer_session_event_get_session_id(InteropSafeHandle eventHandle, IntPtr sessionIdStr, UInt32 sessionId);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognition_event_get_result(InteropSafeHandle eventHandle, out SPXRESULTHANDLE result);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool recognizer_event_handle_is_valid(SPXEVENTHANDLE eventHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_session_started_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_session_stopped_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_speech_start_detected_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_speech_end_detected_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognizing_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_recognized_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_canceled_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR intent_recognizer_add_intent(InteropSafeHandle recoHandle, IntPtr intentId,
            InteropSafeHandle trigger);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR translator_synthesizing_audio_set_callback(InteropSafeHandle recoHandle, CallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR translator_add_target_language(InteropSafeHandle recoHandle, [MarshalAs(UnmanagedType.LPStr)] string language);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR translator_remove_target_language(InteropSafeHandle recoHandle, [MarshalAs(UnmanagedType.LPStr)] string language);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_join_conversation(InteropSafeHandle convhandle, InteropSafeHandle recoHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_leave_conversation(InteropSafeHandle recoHandle);
    }
}
