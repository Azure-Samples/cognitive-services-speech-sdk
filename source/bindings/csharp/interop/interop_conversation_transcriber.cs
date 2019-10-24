//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;

    internal static class ConversationTranscriber
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR conversation_transcriber_update_participant(InteropSafeHandle recoHandle, bool add, InteropSafeHandle participant);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR conversation_transcriber_update_participant_by_user(InteropSafeHandle recoHandle, bool add, InteropSafeHandle participant);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR conversation_transcriber_update_participant_by_user_id(InteropSafeHandle recoHandle, bool add, [MarshalAs(UnmanagedType.LPStr)] string userId);
    }
}
