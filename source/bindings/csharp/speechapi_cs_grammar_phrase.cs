//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;

namespace Microsoft.CognitiveServices.Speech
{
    using Internal;
    using System.Runtime.InteropServices;
    using static Internal.c_interop;

    /*! \cond INTERNAL */
    using SPXPHRASEHANDLE = IntPtr;
    /*! \endcond */

    /// <summary>
    /// Represents a phrase that may be spoken by the user.
    /// Added in version 1.5.0.
    /// </summary>
    public sealed class GrammarPhrase
    {
        /// <summary>
        /// Creates a grammar phrase using the specified phrase text.
        /// </summary>
        /// <param name="text">The text representing a phrase that may be spoken by the user.</param>
        /// <returns>A shared pointer to a grammar phrase.</returns>
        public static GrammarPhrase From(string text)
        {
            SPXPHRASEHANDLE hphrase = SPXHANDLE_INVALID;
            IntPtr textPtr = Utf8StringMarshaler.MarshalManagedToNative(text);
            try
            {
                SPX_THROW_ON_FAIL(grammar_phrase_create_from_text(out hphrase, textPtr));
            }
            finally
            {
                Marshal.FreeHGlobal(textPtr);
            }
            return new GrammarPhrase(hphrase);
        }

        /// <summary>
        /// Internal constructor. Creates a new instance using the provided native handle.
        /// </summary>
        /// <param name="hphrase">Grammar phrase handle.</param>
        internal GrammarPhrase(SPXPHRASEHANDLE hphrase)
        {
            m_hphrase = new InteropSafeHandle(hphrase, grammar_phrase_handle_release);
        }
        
        /// <summary>
        /// Internal native handle property.
        /// </summary>
        internal InteropSafeHandle NativeHandle { get { return m_hphrase; } }

        private InteropSafeHandle m_hphrase;
    }
}
