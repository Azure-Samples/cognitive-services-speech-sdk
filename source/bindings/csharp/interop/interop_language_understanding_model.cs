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
    using SPXLUMODELHANDLE = System.IntPtr;

    internal static class LanguageUnderstandingModel
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool language_understanding_model_handle_is_valid(SPXLUMODELHANDLE lumodel);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR language_understanding_model_create_from_uri(out SPXLUMODELHANDLE lumodel,
            [MarshalAs(UnmanagedType.LPStr)] string uri);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR language_understanding_model_create_from_app_id(out SPXLUMODELHANDLE lumodel,
            [MarshalAs(UnmanagedType.LPStr)] string appId);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR language_understanding_model_create_from_subscription(out SPXLUMODELHANDLE lumodel,
            [MarshalAs(UnmanagedType.LPStr)] string subscriptionKey,
            [MarshalAs(UnmanagedType.LPStr)] string appId,
            [MarshalAs(UnmanagedType.LPStr)] string region);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR language_understanding_model__handle_release(SPXLUMODELHANDLE lumodel);
    }
}
