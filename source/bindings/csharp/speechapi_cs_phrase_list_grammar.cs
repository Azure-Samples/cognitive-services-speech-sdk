//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    using Internal;
    using static Internal.c_interop;

    /*! \cond INTERNAL */
    using SPXRECOHANDLE = IntPtr;
    using SPXGRAMMARHANDLE = IntPtr;
    /*! \endcond */

    /// <summary>
    /// Represents a phrase list grammar for dynamic grammar scenarios.
    /// Added in version 1.5.0.
    /// </summary>
    public sealed class PhraseListGrammar : Grammar
    {
        /// <summary>
        /// Creates a phrase list grammar for the specified recognizer.
        /// </summary>
        /// <param name="recognizer">The recognizer from which to obtain the phrase list grammar.</param>
        /// <returns>The phrase list grammar.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1062", Justification = "validated in internal constructor")]
        public static PhraseListGrammar FromRecognizer(Recognizer recognizer)
        {
            return FromRecognizer(recognizer, "");
        }

        /// <summary>
        /// Internal constructor. Creates a new instance using the provided native handle.
        /// </summary>
        /// <param name="hgrammar">Grammar handle.</param>
        internal PhraseListGrammar(SPXGRAMMARHANDLE hgrammar) : base(hgrammar) { }

        /// <summary>
        /// Adds a simple phrase that may be spoken by the user.
        /// </summary>
        /// <param name="text">The phrase to be added.</param>
        public void AddPhrase(string text)
        {
            var phrase = GrammarPhrase.From(text);
            SPX_THROW_ON_FAIL(phrase_list_grammar_add_phrase(this.NativeHandle, phrase.NativeHandle));
        }

        /// <summary>
        /// Clears all phrases from the phrase list grammar.
        /// </summary>
        public void Clear()
        {
            SPX_THROW_ON_FAIL(phrase_list_grammar_clear(this.NativeHandle));
        }

        /// <summary>
        /// Internal. Creates a phrase list grammar for the specified recognizer, with the specified name.
        /// </summary>
        /// <param name="recognizer">The recognizer from which to obtain the phrase list grammar.</param>
        /// <param name="name">The name of the phrase list grammar to create.</param>
        /// <returns>The phrase list grammar.</returns>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        internal static PhraseListGrammar FromRecognizer(Recognizer recognizer, string name)
        {
            ThrowIfNull(recognizer, "null recognizer");

            InteropSafeHandle hreco = recognizer.recoHandle;

            SPXGRAMMARHANDLE hgrammar = SPXHANDLE_INVALID;
            SPX_THROW_ON_FAIL(phrase_list_grammar_from_recognizer_by_name(out hgrammar, hreco, name));

            return new PhraseListGrammar(hgrammar);
        }
    }
}
