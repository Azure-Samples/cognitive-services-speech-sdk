//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;
    using SPXCONVERSATIONHANDLE = System.IntPtr;

    internal static class Conversation
    {
        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR conversation_update_participant(InteropSafeHandle convHandle, bool add, InteropSafeHandle participant);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR conversation_update_participant_by_user(InteropSafeHandle convHandle, bool add, InteropSafeHandle participant);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR conversation_update_participant_by_user_id(InteropSafeHandle convHandle, bool add, [MarshalAs(UnmanagedType.LPStr)] string userId);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Auto)]
        public static extern SPXHR conversation_get_conversation_id(InteropSafeHandle convHandle, IntPtr conversationIdPtr, UInt32 size);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_end_conversation(InteropSafeHandle convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_get_property_bag(InteropSafeHandle convHandle, out SPXPROPERTYBAGHANDLE propbag);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_release_handle(SPXCONVERSATIONHANDLE convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_start_conversation(InteropSafeHandle convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_delete_conversation(InteropSafeHandle convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_lock_conversation(InteropSafeHandle convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_unlock_conversation(InteropSafeHandle convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_mute_all_participants(InteropSafeHandle convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_unmute_all_participants(InteropSafeHandle convHandle);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR conversation_mute_participant(InteropSafeHandle convHandle, Utf8StringHandle participantId);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR conversation_unmute_participant(InteropSafeHandle convHandle, Utf8StringHandle participantId);
    }
}
