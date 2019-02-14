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

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void ConnectionCallbackFunctionDelegate(SPXEVENTHANDLE hevent, IntPtr context);

    internal class Connection
    {

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_from_recognizer(InteropSafeHandle recognizerHandle, out SPXCONNECTIONHANDLE connectionHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool connection_handle_is_valid(InteropSafeHandle handle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_handle_release(SPXCONNECTIONHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_open(InteropSafeHandle handle, bool forContinuousRecognition);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_close(InteropSafeHandle handle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_connected_set_callback(InteropSafeHandle connection, ConnectionCallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_disconnected_set_callback(InteropSafeHandle connection, ConnectionCallbackFunctionDelegate Callback, IntPtr context);

    }
}
