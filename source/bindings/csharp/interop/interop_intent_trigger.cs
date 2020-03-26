//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Internal
{

    using SPXHR = System.IntPtr;
    using SPXLUMODELHANDLE = System.IntPtr;
    using SPXTRIGGERHANDLE = System.IntPtr;

    internal class IntentTrigger
    {
        internal InteropSafeHandle triggerHandle;

        internal IntentTrigger(IntPtr triggerPtr)
        {
            triggerHandle = new InteropSafeHandle(triggerPtr, Internal.IntentTrigger.intent_trigger_handle_release);
        }

        public static IntentTrigger From(string simplePhrase)
        {
            SPXTRIGGERHANDLE triggerHandle = IntPtr.Zero;
            IntPtr simplePhrasePtr = Utf8StringMarshaler.MarshalManagedToNative(simplePhrase);
            try
            {
                ThrowIfFail(intent_trigger_create_from_phrase(out triggerHandle, simplePhrasePtr));
            }
            finally
            {
                Marshal.FreeHGlobal(simplePhrasePtr);
            }
            return new IntentTrigger(triggerHandle);
        }

        public static IntentTrigger From(Intent.LanguageUnderstandingModel model)
        {
            SPXTRIGGERHANDLE triggerHandle = IntPtr.Zero;
            ThrowIfFail(intent_trigger_create_from_language_understanding_model(out triggerHandle, model.luHandle, IntPtr.Zero));
            GC.KeepAlive(model);
            return new IntentTrigger(triggerHandle);
        }

        public static IntentTrigger From(Intent.LanguageUnderstandingModel model, string intentName)
        {
            SPXTRIGGERHANDLE triggerHandle = IntPtr.Zero;
            IntPtr intentNamePtr = Utf8StringMarshaler.MarshalManagedToNative(intentName);
            try
            {
                ThrowIfFail(intent_trigger_create_from_language_understanding_model(out triggerHandle, model.luHandle, intentNamePtr));
            }
            finally
            {
                Marshal.FreeHGlobal(intentNamePtr);
            }
            GC.KeepAlive(model);
            return new IntentTrigger(triggerHandle);
        }

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool intent_trigger_handle_is_valid(SPXTRIGGERHANDLE trigger);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR intent_trigger_create_from_phrase(out SPXTRIGGERHANDLE trigger, IntPtr phrase);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR intent_trigger_create_from_language_understanding_model(out SPXTRIGGERHANDLE trigger, InteropSafeHandle lumodel,
            IntPtr intentName);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR intent_trigger_handle_release(SPXTRIGGERHANDLE trigger);
    }
}
