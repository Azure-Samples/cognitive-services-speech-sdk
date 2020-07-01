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
    /// Defines the scope that a Recognition Factor is applied to.
    /// </summary>
    public enum RecognitionFactorScope
    {
        /// <summary>
        /// A Recognition Factor will apply to grammars that can be referenced as individual partial phrases.
        /// </summary>
        /// <remarks>
        /// Currently only applies to PhraseListGrammars
        /// </remarks>
        PartialPhrase = 1,
    };

    /// <summary>
    /// Represents a list of grammars for dynamic grammar scenarios.
    /// Added in version 1.7.0.
    /// </summary>
    /// <remarks>
    /// GrammarLists are only usable in specific scenarios and are not generally available.
    /// </remarks>
    public sealed class GrammarList : Grammar
    {
        /// <summary>
        /// Creates a grammar lsit for the specified recognizer.
        /// </summary>
        /// <param name="recognizer">The recognizer from which to obtain the grammar list.</param>
        /// <returns>The grammar list associated with the recognizer.</returns>
        /// <remarks>
        /// Creating a grammar list from a recognizer is only usable in specific scenarios and is not generally available.
        /// </remarks>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1303", Justification = "exceptions not localized")]
        public static GrammarList FromRecognizer(Recognizer recognizer)
        {
            ThrowIfNull(recognizer, "null recognizer");

            InteropSafeHandle hreco = recognizer.recoHandle;

            SPXGRAMMARHANDLE hgrammarList = SPXHANDLE_INVALID;

            SPX_THROW_ON_FAIL(grammar_list_from_recognizer(out hgrammarList, hreco));

            return new GrammarList(hgrammarList);
        }

        /// <summary>
        /// Internal constructor. Creates a new instance using the provided native handle.
        /// </summary>
        /// <param name="hgrammar">Grammar handle.</param>
        internal GrammarList(SPXGRAMMARHANDLE hgrammar) : base(hgrammar) { }

        /// <summary>
        /// Adds a single grammar to the current grammar list
        /// </summary>
        /// <param name="grammar">The grammar to add</param>
        /// <remarks>
        /// Currently Class Language Models are the only support grammars to add.
        /// </remarks>
        public void Add(Grammar grammar)
        {
            ThrowIfNull(grammar);
            SPX_THROW_ON_FAIL(grammar_list_add_grammar(this.NativeHandle, grammar.NativeHandle));
        }

        /// <summary>
        /// Sets the Recognition Factor applied to all grammars in a recognizer's GrammarList
        /// </summary>
        /// <param name="factor">The RecognitionFactor to apply</param>
        /// <param name="scope">The scope for the Recognition Factor being set</param>
        /// <remarks>
        /// The Recognition Factor is a numerical value greater than 0 modifies the default weight applied to supplied grammars.
        /// Setting the Recognition Factor to 0 will disable the supplied grammars.
        /// The default Recognition Factor is 1.
        /// </remarks>
        public void SetRecognitionFactor(double factor, RecognitionFactorScope scope)
        {
            SPX_THROW_ON_FAIL(grammar_list_set_recognition_factor(this.NativeHandle, factor, scope));
        }

    }
}
