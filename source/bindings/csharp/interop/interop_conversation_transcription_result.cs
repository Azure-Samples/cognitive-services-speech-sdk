//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;

    internal static class ConversationTranscriptionResult
    {
        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR conversation_transcription_result_get_user_id(InteropSafeHandle result, IntPtr userIdPtr, UInt32 userIdSize);

        [DllImport(Import.NativeDllName, CallingConvention = Import.NativeCallConvention, CharSet = CharSet.Ansi)]
        public static extern SPXHR conversation_transcription_result_get_utterance_id(InteropSafeHandle result, IntPtr utteranceIdPtr, UInt32 utteranceIdSize);

    }
}
