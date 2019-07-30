//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;

    using SPXRECOHANDLE = System.IntPtr;
    using SPXGRAMMARHANDLE = System.IntPtr;
    using SPXPHRASEHANDLE = System.IntPtr;

    internal partial class c_interop
    {
        internal static IntPtr SPXHANDLE_INVALID = (IntPtr)(-1);

        internal static void SPX_THROW_ON_FAIL(SPXHR hr)
        {
            SpxExceptionThrower.ThrowIfFail(hr);
        }

        // --- grammar C99 APIs ---

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool grammar_handle_is_valid(InteropSafeHandle grammar);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR phrase_list_grammar_from_recognizer_by_name(out SPXGRAMMARHANDLE grammar, InteropSafeHandle reco, [MarshalAs(UnmanagedType.LPStr)] string name);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR grammar_handle_release(SPXGRAMMARHANDLE grammar);

        // --- phrase list grammar C99 APIs ---

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR phrase_list_grammar_add_phrase(InteropSafeHandle grammar, InteropSafeHandle phrase);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR phrase_list_grammar_clear(InteropSafeHandle grammar);

        // --- grammar phrase C99 APIs ---

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool grammar_phrase_handle_is_valid(InteropSafeHandle phrase);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR grammar_phrase_create_from_text(out SPXPHRASEHANDLE phrase, IntPtr text);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR grammar_phrase_handle_release(SPXPHRASEHANDLE phrase);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR grammar_create_from_storage_id(out SPXGRAMMARHANDLE hgrammarlist, IntPtr id);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR grammar_list_from_recognizer(out SPXGRAMMARHANDLE hgrammarlist, InteropSafeHandle hreco);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR grammar_list_add_grammar(InteropSafeHandle hgrammarlist, InteropSafeHandle hgrammar);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR class_language_model_from_storage_id(out SPXGRAMMARHANDLE hclm, IntPtr storageid);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR class_language_model_assign_class(InteropSafeHandle hclm, IntPtr classname, InteropSafeHandle hgrammar);
    }
}
