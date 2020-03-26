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
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    internal static class PropertyCollection
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern bool property_bag_is_valid(InteropSafeHandle propbag);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR property_bag_set_string(InteropSafeHandle propbag, Int32 id,
            IntPtr name,
            IntPtr value);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern IntPtr property_bag_get_string(InteropSafeHandle propbag, Int32 id,
            IntPtr name,
            IntPtr defaultValue);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR property_bag_free_string(IntPtr ptr);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention)]
        public static extern SPXHR property_bag_release(SPXPROPERTYBAGHANDLE propbag);
    }
}
