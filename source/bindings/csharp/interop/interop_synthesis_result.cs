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

    internal static class SynthesisResult
    {

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synth_result_get_result_id(InteropSafeHandle result, IntPtr resultId, UInt32 resultIdLength);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synth_result_get_reason(InteropSafeHandle result, out ResultReason reason);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synth_result_get_reason_canceled(InteropSafeHandle result, out CancellationReason reason);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synth_result_get_canceled_error_code(InteropSafeHandle result, out CancellationErrorCode errorCode);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synth_result_get_audio_data(InteropSafeHandle result, IntPtr buffer, UInt32 bufferSize, out UInt32 filledSize);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synth_result_get_audio_length(InteropSafeHandle result, out UInt32 audioLength);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synth_result_get_property_bag(InteropSafeHandle result, out SPXPROPERTYBAGHANDLE hpropbag);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool synthesizer_result_handle_is_valid(InteropSafeHandle result);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synthesizer_result_handle_release(SPXRESULTHANDLE result);

    }
}
