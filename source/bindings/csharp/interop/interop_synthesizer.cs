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
    using SPXSYNTHHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void TtsCallbackFunctionDelegate(SPXSYNTHHANDLE synthHandle, SPXEVENTHANDLE eventHandle, IntPtr context);

    internal static class Synthesizer
    {

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool synthesizer_handle_is_valid(SPXSYNTHHANDLE synthHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_handle_release(SPXSYNTHHANDLE synthHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool synthesizer_async_handle_is_valid(SPXSYNTHHANDLE synthHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_async_handle_release(SPXSYNTHHANDLE synthHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool synthesizer_result_handle_is_valid(SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_result_handle_release(SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool synthesizer_event_handle_is_valid(SPXEVENTHANDLE eventHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_event_handle_release(SPXEVENTHANDLE eventHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_enable(SPXSYNTHHANDLE synthHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_disable(SPXSYNTHHANDLE synthHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_is_enabled(SPXSYNTHHANDLE synthHandle, out bool enabled);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_get_property_bag(InteropSafeHandle synthHandle, out SPXPROPERTYBAGHANDLE hpropbag);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_speak_text(InteropSafeHandle synthHandle, IntPtr text, UInt32 textLength, out SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_speak_ssml(InteropSafeHandle synthHandle, IntPtr ssml, UInt32 ssmlLength, out SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_speak_text_async(InteropSafeHandle synthHandle, IntPtr text, UInt32 textLength, out SPXASYNCHANDLE asyncHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_speak_ssml_async(InteropSafeHandle synthHandle, IntPtr ssml, UInt32 ssmlLength, out SPXASYNCHANDLE asyncHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_start_speaking_text(InteropSafeHandle synthHandle, IntPtr text, UInt32 textLength, out SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_start_speaking_ssml(InteropSafeHandle synthHandle, IntPtr ssml, UInt32 ssmlLength, out SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_start_speaking_text_async(InteropSafeHandle synthHandle, IntPtr text, UInt32 textLength, out SPXASYNCHANDLE asyncHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_start_speaking_ssml_async(InteropSafeHandle synthHandle, IntPtr ssml, UInt32 ssmlLength, out SPXASYNCHANDLE asyncHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_speak_async_wait_for(SPXASYNCHANDLE hasync, UInt32 milliseconds, out SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_started_set_callback(InteropSafeHandle synthHandle, TtsCallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_synthesizing_set_callback(InteropSafeHandle synthHandle, TtsCallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_completed_set_callback(InteropSafeHandle synthHandle, TtsCallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_canceled_set_callback(InteropSafeHandle synthHandle, TtsCallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_word_boundary_set_callback(InteropSafeHandle synthHandle, TtsCallbackFunctionDelegate callback, IntPtr context);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_synthesis_event_get_result(SPXEVENTHANDLE eventHandle, out SPXRESULTHANDLE resultHandle);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR synthesizer_word_boundary_event_get_values(SPXEVENTHANDLE eventHandle, out UInt64 audioOffset, out UInt32 textOffset, out UInt32 wordLength);

    }
}
