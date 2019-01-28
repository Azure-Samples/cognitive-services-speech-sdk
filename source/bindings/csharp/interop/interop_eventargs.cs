//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXEVENTHANDLE = System.IntPtr;

    internal class EventArgs : SpxExceptionThrower, IDisposable
    {
        protected SPXEVENTHANDLE eventHandle = IntPtr.Zero;
        protected bool disposed = false;

        internal EventArgs(IntPtr eventPtr)
        {
            eventHandle = eventPtr;
        }

        ~EventArgs()
        {
            Dispose(false);
        }

        public virtual void Dispose()
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
            if (eventHandle != IntPtr.Zero)
            {
                LogErrorIfFail(recognizer_event_handle_release(eventHandle));
                eventHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool recognizer_event_handle_is_valid(SPXEVENTHANDLE hevent);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_event_handle_release(SPXEVENTHANDLE hevent);
    }
}
