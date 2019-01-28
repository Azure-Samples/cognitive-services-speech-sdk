//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Text;

namespace Microsoft.CognitiveServices.Speech.Internal
{

    using SPXHR = System.IntPtr;
    using SPXLUMODELHANDLE = System.IntPtr;
    using SPXTRIGGERHANDLE = System.IntPtr;

    internal class IntentTrigger : SpxExceptionThrower, IDisposable
    {
        internal SPXTRIGGERHANDLE triggerHandle = IntPtr.Zero;
        private bool disposed = false;

        internal IntentTrigger(IntPtr triggerPtr)
        {
            triggerHandle = triggerPtr;
        }

        ~IntentTrigger()
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
            if (triggerHandle != IntPtr.Zero)
            {
                ThrowIfFail(intent_trigger_handle_release(triggerHandle));
                triggerHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public static IntentTrigger From(string simplePhrase)
        {
            SPXTRIGGERHANDLE triggerHandle = IntPtr.Zero;
            ThrowIfFail(intent_trigger_create_from_phrase(out triggerHandle, simplePhrase));
            return new IntentTrigger(triggerHandle);
        }

        public static IntentTrigger From(LanguageUnderstandingModel model)
        {
            SPXTRIGGERHANDLE triggerHandle = IntPtr.Zero;
            ThrowIfFail(intent_trigger_create_from_language_understanding_model(out triggerHandle, model.luHandle, null));
            GC.KeepAlive(model);
            return new IntentTrigger(triggerHandle);
        }

        public static IntentTrigger From(LanguageUnderstandingModel model, string intentName)
        {
            SPXTRIGGERHANDLE triggerHandle = IntPtr.Zero;
            ThrowIfFail(intent_trigger_create_from_language_understanding_model(out triggerHandle, model.luHandle, intentName));
            GC.KeepAlive(model);
            return new IntentTrigger(triggerHandle);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool intent_trigger_handle_is_valid(SPXTRIGGERHANDLE htrigger);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR intent_trigger_create_from_phrase(out SPXTRIGGERHANDLE htrigger,
            [MarshalAs(UnmanagedType.LPStr)] string phrase);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR intent_trigger_create_from_language_understanding_model(out SPXTRIGGERHANDLE htrigger, SPXLUMODELHANDLE hlumodel,
            [MarshalAs(UnmanagedType.LPStr)] string intentName);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR intent_trigger_handle_release(SPXTRIGGERHANDLE htrigger);
    }
}
