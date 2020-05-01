//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXAUDIOSTREAMHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using SPXAUDIOSTREAMFORMATHANDLE = System.IntPtr;

    internal static class AudioDataStream
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool audio_data_stream_is_handle_valid(SPXAUDIOSTREAMHANDLE audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_create_from_result(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle result);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_create_from_keyword_result(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle result);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_get_status(InteropSafeHandle audioStream, out StreamStatus status);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_get_reason_canceled(InteropSafeHandle audioStream, out CancellationReason reason);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_get_canceled_error_code(InteropSafeHandle audioStream, out CancellationErrorCode errorCode);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool audio_data_stream_can_read_data(InteropSafeHandle audioStream, UInt32 requestedSize);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool audio_data_stream_can_read_data_from_position(InteropSafeHandle audioStream, UInt32 requestedSize, UInt32 position);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_read(InteropSafeHandle audioStream, IntPtr buffer, UInt32 bufferSize, out UInt32 filledSize);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_read_from_position(InteropSafeHandle audioStream, IntPtr buffer, UInt32 bufferSize, UInt32 position, out UInt32 filledSize);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_save_to_wave_file(InteropSafeHandle audioStream, [MarshalAs(UnmanagedType.LPStr)] string fileName);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_get_position(InteropSafeHandle audioStream, out UInt32 position);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_set_position(InteropSafeHandle audioStream, UInt32 position);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_detach_input(InteropSafeHandle audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_get_property_bag(InteropSafeHandle audioStream, out SPXPROPERTYBAGHANDLE propbag);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_data_stream_release(SPXAUDIOSTREAMHANDLE audioStream);

    }

    internal static class AudioInputStream
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool audio_stream_is_handle_valid(InteropSafeHandle audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_release(SPXAUDIOSTREAMHANDLE audioStream);
    }

    internal static class AudioOutputStream
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool audio_stream_is_handle_valid(InteropSafeHandle audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_release(SPXAUDIOSTREAMHANDLE audioStream);
    }

    internal static class AudioStreamFormat
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool audio_stream_format_is_handle_valid(SPXAUDIOSTREAMFORMATHANDLE format);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_format_create_from_default_input(out SPXAUDIOSTREAMFORMATHANDLE format);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_format_create_from_default_output(out SPXAUDIOSTREAMFORMATHANDLE format);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_format_create_from_waveformat_pcm(out SPXAUDIOSTREAMFORMATHANDLE format, UInt32 samplesPerSecond, Byte bitsPerSample, Byte channels);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_format_release(SPXAUDIOSTREAMFORMATHANDLE format);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_format_create_from_compressed_format(out SPXAUDIOSTREAMFORMATHANDLE format, Microsoft.CognitiveServices.Speech.Audio.AudioStreamContainerFormat compressedFormat);
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate int PullAudioStreamReadDelegate(IntPtr context, IntPtr dataBuffer, uint size);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void PullAudioStreamGetPropertyDelegate(IntPtr context, PropertyId id, IntPtr dataBuffer, uint size);
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void PullAudioStreamCloseDelegate(IntPtr context);
    internal static class PullAudioInputStream
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_create_pull_audio_input_stream(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle format);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR pull_audio_input_stream_set_callbacks(InteropSafeHandle audioStream, IntPtr context, PullAudioStreamReadDelegate readCallback, PullAudioStreamCloseDelegate closeCallback);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR pull_audio_input_stream_set_getproperty_callback(InteropSafeHandle audioStream, IntPtr context, PullAudioStreamGetPropertyDelegate getPropertyCallback);
    }

    internal static class PullAudioOutputStream
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_create_pull_audio_output_stream(out SPXAUDIOSTREAMHANDLE audioStream);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR pull_audio_output_stream_read(InteropSafeHandle audioStream, IntPtr buffer, UInt32 bufferSize, out UInt32 filledSize);
    }

    internal static class PushAudioInputStream
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_create_push_audio_input_stream(out SPXAUDIOSTREAMHANDLE audioStream, InteropSafeHandle format);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR push_audio_input_stream_write(InteropSafeHandle audioStream, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2)] byte[] buffer, UInt32 size);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR push_audio_input_stream_close(InteropSafeHandle audioStream);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Auto)]
        public static extern SPXHR push_audio_input_stream_set_property_by_id(InteropSafeHandle haudioStream, Int32 id,
                    [MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Auto)]
        public static extern SPXHR push_audio_input_stream_set_property_by_name(InteropSafeHandle haudioStream,
            [MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.LPStr)] string value);
    }

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate uint PushAudioStreamWriteDelegate(IntPtr context, IntPtr dataBuffer, uint size);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void PushAudioStreamCloseDelegate(IntPtr context);

    internal static class PushAudioOutputStream
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR audio_stream_create_push_audio_output_stream(out SPXAUDIOSTREAMHANDLE audioStream);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR push_audio_output_stream_set_callbacks(InteropSafeHandle audioStream, IntPtr context, PushAudioStreamWriteDelegate writeCallback, PushAudioStreamCloseDelegate closeCallback);
    }

}
