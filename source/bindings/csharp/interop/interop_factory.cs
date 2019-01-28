//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRECOHANDLE = System.IntPtr;
    using SPXSPEECHCONFIGHANDLE = System.IntPtr;
    using SPXAUDIOCONFIGHANDLE = System.IntPtr;

    internal static class Import
    {
#if !UNIX
        public const string NativeDllName = "Microsoft.CognitiveServices.Speech.core.dll";
#else
        public const string NativeDllName = "libMicrosoft.CognitiveServices.Speech.core.so";
#endif
    }

    internal class MonoPInvokeCallbackAttribute : Attribute
    {
        public MonoPInvokeCallbackAttribute() { }
    }

    internal static class SpxFactory 
    {
        public delegate IntPtr GetResultDelegate(IntPtr handle, IntPtr result, UInt32 maxCharCount);

        public static string GetDataFromHandleUsingDelegate(GetResultDelegate functionPtr, IntPtr handle, Int32 maxCharCount)
        {
            string resultTextStr = string.Empty;
            IntPtr resultTextPtr = Marshal.AllocHGlobal(maxCharCount + 1);
            try
            {
                SpxExceptionThrower.ThrowIfFail(functionPtr(handle, resultTextPtr, (uint)maxCharCount));
                resultTextStr = Utf8StringMarshaler.MarshalNativeToManaged(resultTextPtr);
            }
            finally
            {
                Marshal.FreeHGlobal(resultTextPtr);
            }
            return resultTextStr;
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_create_speech_recognizer_from_config(out SPXRECOHANDLE phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_create_translation_recognizer_from_config(out SPXRECOHANDLE phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR recognizer_create_intent_recognizer_from_config(out SPXRECOHANDLE phreco, SPXSPEECHCONFIGHANDLE hspeechconfig, SPXAUDIOCONFIGHANDLE haudioInput);
    }    
}
