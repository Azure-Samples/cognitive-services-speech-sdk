//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXCONNECTIONHANDLE = System.IntPtr;
    using SPXRECOHANDLE = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;
    using SPXCONNECTIONMESSAGEHANDLE = System.IntPtr;
    using SPXCONNECTIONMESSAGEEVENTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void ConnectionCallbackFunctionDelegate(SPXEVENTHANDLE hevent, IntPtr context);

    internal class Connection
    {

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_from_recognizer(InteropSafeHandle recognizerHandle, out SPXCONNECTIONHANDLE connectionHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_from_conversation_translator(InteropSafeHandle convTransHandle, out SPXCONNECTIONHANDLE connectionHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_from_dialog_service_connector(InteropSafeHandle dialogServiceConnectorHandle, out SPXCONNECTIONHANDLE connectionHandle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool connection_handle_is_valid(InteropSafeHandle handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_handle_release(SPXCONNECTIONHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_open(InteropSafeHandle handle, bool forContinuousRecognition);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_close(InteropSafeHandle handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_connected_set_callback(InteropSafeHandle connection, ConnectionCallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_disconnected_set_callback(InteropSafeHandle connection, ConnectionCallbackFunctionDelegate Callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_message_received_set_callback(InteropSafeHandle connection, ConnectionCallbackFunctionDelegate Callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_set_message_property(InteropSafeHandle connection,
                                                                       [MarshalAs(UnmanagedType.LPStr)] string path,
                                                                       IntPtr name,
                                                                       IntPtr value);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_send_message(InteropSafeHandle connection,
                                                                       [MarshalAs(UnmanagedType.LPStr)] string path,
                                                                       IntPtr payload);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_send_message_data(InteropSafeHandle connection, [MarshalAs(UnmanagedType.LPStr)] string path,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)] byte[] payload, UInt32 size);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool connection_message_received_event_handle_is_valid(InteropSafeHandle handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_message_received_event_handle_release(SPXEVENTHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_message_received_event_get_message(InteropSafeHandle eventHandle, out SPXCONNECTIONMESSAGEHANDLE hcm);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool connection_message_handle_is_valid(InteropSafeHandle handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_message_handle_release(SPXEVENTHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_message_get_property_bag(InteropSafeHandle connectionMessageHandle, out SPXPROPERTYBAGHANDLE propbag);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR connection_message_get_data(InteropSafeHandle connectionMessageHandle, IntPtr buffer, UInt32 bufferSize);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern UInt32 connection_message_get_data_size(InteropSafeHandle connectionMessageHandle);
    }
}
