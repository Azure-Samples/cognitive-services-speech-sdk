//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;
using System.Text;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXPARTICIPANTHANDLE = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    internal static class Participant
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR participant_set_preferred_langugage(InteropSafeHandle participant,
            [MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR participant_set_voice_signature(InteropSafeHandle participant,
            [MarshalAs(UnmanagedType.LPStr)] string value, int size);        

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR participant_create_handle(out SPXPARTICIPANTHANDLE participant,
            [MarshalAs(UnmanagedType.LPStr)] string userId,
            [MarshalAs(UnmanagedType.LPStr)] string preferredLanguage,
            [MarshalAs(UnmanagedType.LPStr)] string voice);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR participant_release_handle(SPXPARTICIPANTHANDLE participant);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR participant_get_property_bag(InteropSafeHandle participant, out SPXPROPERTYBAGHANDLE hpropbag);
    }
}
