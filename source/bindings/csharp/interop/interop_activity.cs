//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;

namespace Microsoft.CognitiveServices.Speech.Dialog.Internal
{
    using SPXHR = System.IntPtr;
    using SPXACTIVITYHANDLE = System.IntPtr;

    internal static class Activity
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        internal static extern SPXHR activity_from_string(IntPtr serializedActivity, out SPXACTIVITYHANDLE activity);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        internal static extern SPXHR activity_handle_release(SPXACTIVITYHANDLE activityHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        internal static extern SPXHR activity_serialized_size(SPXACTIVITYHANDLE activityHandle, out UInt32 size);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        internal static extern SPXHR activity_serialize(SPXACTIVITYHANDLE activityHandle, StringBuilder serializedActivity, UInt32 max_size);
    }
}

