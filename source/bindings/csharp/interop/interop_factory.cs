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
    using SPXRECOHANDLE = System.IntPtr;
    using SPXSYNTHHANDLE = System.IntPtr;
    using SPXCONNECTORHANDLE = System.IntPtr;
    using SPXCONVERSATIONHANDLE = System.IntPtr;
    using SPXVOICEPROFILECLIENTHANDLE = System.IntPtr;

    internal static class Import
    {
#if IOS
        public const string NativeDllName = "__Internal";
        public const CallingConvention NativeCallConvention = CallingConvention.Cdecl;
#elif OSX
        public const string NativeDllName = "libMicrosoft.CognitiveServices.Speech.core.dylib";
        public const CallingConvention NativeCallConvention = CallingConvention.Cdecl;
#elif UNIX
        public const string NativeDllName = "libMicrosoft.CognitiveServices.Speech.core.so";
        public const CallingConvention NativeCallConvention = CallingConvention.Cdecl;
#else
#if OS_BUILD
        public const string NativeDllName = "Microsoft.CognitiveServices.Speech.core.os.dll";
        public const CallingConvention NativeCallConvention = CallingConvention.StdCall;
#else
        public const string NativeDllName = "Microsoft.CognitiveServices.Speech.core.dll";
        public const CallingConvention NativeCallConvention = CallingConvention.StdCall;
#endif
#endif
    }

    [AttributeUsage(AttributeTargets.Method)]
    internal class MonoPInvokeCallbackAttribute : Attribute
    {
        private Type type;
        public MonoPInvokeCallbackAttribute(Type t) { type = t; }
    }

    internal delegate IntPtr HandleRelease(IntPtr hresult);
    internal sealed class InteropSafeHandle : SafeHandle
    {
        private HandleRelease releaseHandleFunc = null;
        public InteropSafeHandle(IntPtr handle, HandleRelease releaseHandle) : base(IntPtr.Zero, true)
        {
            releaseHandleFunc = releaseHandle;
            SetHandle(handle);
        }

        public override bool IsInvalid
        {
            get { return handle == IntPtr.Zero || handle == new IntPtr(-1); }
        }

        protected override bool ReleaseHandle()
        {
            if (releaseHandleFunc != null)
            {
                LogErrorIfFail(releaseHandleFunc(handle));
                releaseHandleFunc = null;
                handle = IntPtr.Zero;
            }
            return true;
        }

        protected override void Dispose(bool disposing)
        {
            ReleaseHandle();
        }

        public static T GetObjectFromWeakHandle<T>(IntPtr nativeHandle)
        {
            var weakGCHandle = GCHandle.FromIntPtr(nativeHandle);
            if (!weakGCHandle.IsAllocated)
            {
                return default(T);
            }
            return (T)weakGCHandle.Target;
        }
    }

    internal static class SpxFactory
    {
        public delegate IntPtr GetResultDelegate(InteropSafeHandle handle, IntPtr result, UInt32 maxCharCount);

        public static string GetDataFromHandleUsingDelegate(GetResultDelegate functionPtr, InteropSafeHandle handle, Int32 maxCharCount)
        {
            string resultTextStr = string.Empty;
            IntPtr resultTextPtr = Marshal.AllocHGlobal(maxCharCount + 1);
            try
            {
                ThrowIfFail(functionPtr(handle, resultTextPtr, (uint)maxCharCount));
                resultTextStr = Utf8StringMarshaler.MarshalNativeToManaged(resultTextPtr);
            }
            finally
            {
                Marshal.FreeHGlobal(resultTextPtr);
            }
            return resultTextStr;
        }

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_speech_recognizer_from_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle speechconfig, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_speech_recognizer_from_source_lang_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle speechconfig, InteropSafeHandle sourceLangConfig, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_speech_recognizer_from_auto_detect_source_lang_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle speechconfig, InteropSafeHandle autoDetectSourceLangConfig, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_translation_recognizer_from_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle speechconfig, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_intent_recognizer_from_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle speechconfig, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_keyword_recognizer_from_audio_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synthesizer_create_speech_synthesizer_from_config(out SPXSYNTHHANDLE synthHandle, InteropSafeHandle speechconfig, InteropSafeHandle audioOutput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR synthesizer_create_speech_synthesizer_from_auto_detect_source_lang_config(out SPXSYNTHHANDLE synthHandle, InteropSafeHandle speechconfig, InteropSafeHandle autoDetectSourceLangConfig, InteropSafeHandle audioOutput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR dialog_service_connector_create_dialog_service_connector_from_config(out SPXCONNECTORHANDLE recoHandle, InteropSafeHandle speechconfig, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR conversation_create_from_config(out SPXCONVERSATIONHANDLE recoHandle, InteropSafeHandle speechconfig, Utf8StringHandle id);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_conversation_transcriber_from_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR recognizer_create_speaker_recognizer_from_config(out SPXRECOHANDLE recoHandle, InteropSafeHandle speechConfig, InteropSafeHandle audioInput);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR create_voice_profile_client_from_config(out SPXVOICEPROFILECLIENTHANDLE clientHandle, InteropSafeHandle speechConfig);
    }
}
