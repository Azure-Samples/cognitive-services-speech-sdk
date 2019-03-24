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
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    internal static class AudioDataStream
    {

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_data_stream_is_handle_valid(SPXAUDIOSTREAMHANDLE audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_create_from_result(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle result);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_get_status(InteropSafeHandle audioStream, out StreamStatus status);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_get_reason_canceled(InteropSafeHandle audioStream, out CancellationReason reason);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_get_canceled_error_code(InteropSafeHandle audioStream, out CancellationErrorCode errorCode);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_data_stream_can_read_data(InteropSafeHandle audioStream, UInt32 requestedSize);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool audio_data_stream_can_read_data_from_position(InteropSafeHandle audioStream, UInt32 requestedSize, UInt32 position);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_read(InteropSafeHandle audioStream, IntPtr buffer, UInt32 bufferSize, out UInt32 filledSize);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_read_from_position(InteropSafeHandle audioStream, IntPtr buffer, UInt32 bufferSize, UInt32 position, out UInt32 filledSize);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_save_to_wave_file(InteropSafeHandle audioStream, [MarshalAs(UnmanagedType.LPStr)] string fileName);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_get_position(InteropSafeHandle audioStream, out UInt32 position);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_set_position(InteropSafeHandle audioStream, UInt32 position);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_get_property_bag(InteropSafeHandle audioStream, out SPXPROPERTYBAGHANDLE propbag);
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR audio_data_stream_release(SPXAUDIOSTREAMHANDLE audioStream);

    }
}
