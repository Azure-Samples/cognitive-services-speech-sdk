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
    using SPXKEYWORDHANDLE = System.IntPtr;

    internal static class KeywordRecognitionModel
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool keyword_recognition_model_handle_is_valid(SPXKEYWORDHANDLE keyword);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR keyword_recognition_model_handle_release(SPXKEYWORDHANDLE keyword);
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR keyword_recognition_model_create_from_file(
            [MarshalAs(UnmanagedType.LPStr)] string fileName,
            out SPXKEYWORDHANDLE kwmodel);
    }
}
