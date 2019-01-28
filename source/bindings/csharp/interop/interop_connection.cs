//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXCONNECTIONHANDLE = System.IntPtr;
    using SPXRECOHANDLE = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void ConnectionCallbackFunctionDelegate(SPXEVENTHANDLE hevent, IntPtr context);

    internal class Connection : SpxExceptionThrower, IDisposable
    {
        internal SPXCONNECTIONHANDLE connectionHandle = IntPtr.Zero;
        private bool disposed = false;

        internal Connection(IntPtr connectionPtr)
        {
            connectionHandle = connectionPtr;
        }

        ~Connection()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
            }
            // dispose unmanaged resources
            if (connectionHandle != IntPtr.Zero)
            {
                LogErrorIfFail(connection_handle_release(connectionHandle));
                connectionHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static Connection FromRecognizer(Recognizer recognizer)
        {
            ThrowIfNull(recognizer);
            SPXCONNECTIONHANDLE handle = IntPtr.Zero;
            ThrowIfFail(connection_from_recognizer(recognizer.recoHandle, out handle));
            return new Connection(handle);
        }

        public void Open(bool forContinuousRecognition)
        {
            ThrowIfFail(connectionHandle != IntPtr.Zero, SpxError.InvalidHandle.ToInt32());
            ThrowIfFail(connection_open(connectionHandle, forContinuousRecognition));
        }

        public void Close()
        {
            ThrowIfFail(connectionHandle != IntPtr.Zero, SpxError.InvalidHandle.ToInt32());
            ThrowIfFail(connection_close(connectionHandle));
        }

        public void SetConnectedCallback(ConnectionCallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(connection_handle_is_valid(connectionHandle));
            ThrowIfFail(connection_connected_set_callback(connectionHandle, callbackDelegate, gcHandlePtr));
        }

        public void SetDisconnectedCallback(ConnectionCallbackFunctionDelegate callbackDelegate, IntPtr gcHandlePtr)
        {
            ThrowIfFail(connection_handle_is_valid(connectionHandle));
            ThrowIfFail(connection_disconnected_set_callback(connectionHandle, callbackDelegate, gcHandlePtr));
        }


        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_from_recognizer(SPXRECOHANDLE recognizerHandle, out SPXCONNECTIONHANDLE connectionHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool connection_handle_is_valid(SPXCONNECTIONHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_handle_release(SPXCONNECTIONHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_open(SPXCONNECTIONHANDLE handle, bool forContinuousRecognition);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_close(SPXCONNECTIONHANDLE handle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_connected_set_callback(SPXCONNECTIONHANDLE connection, ConnectionCallbackFunctionDelegate callback, IntPtr context);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR connection_disconnected_set_callback(SPXCONNECTIONHANDLE connection, ConnectionCallbackFunctionDelegate Callback, IntPtr context);

    }
}
