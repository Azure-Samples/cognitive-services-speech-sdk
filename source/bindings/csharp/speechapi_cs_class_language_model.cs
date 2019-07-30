//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    using Internal;
    using System.Runtime.InteropServices;
    using static Internal.c_interop;

    /*! \cond INTERNAL */
    using SPXRECOHANDLE = IntPtr;
    using SPXGRAMMARHANDLE = IntPtr;
    /*! \endcond */

    /// <summary>
    /// Represents a list of grammars for dynamic grammar scenarios.
    /// Added in version 1.7.0.
    /// </summary>
    /// <remarks>
    /// ClassLanguageModels are only usable in specific scenarios and are not generally available.
    /// </remarks>
    public sealed class ClassLanguageModel : Grammar
    {
        /// <summary>
        /// Creates a class language model from a storage ID.
        /// </summary>
        /// <param name="storageId)">The persisted storage ID of the language model.</param>
        /// <returns>The grammar.</returns>
        /// <remarks>
        /// Creating a ClassLanguageModel from a storage ID is only usable in specific scenarios and is not generally available.
        /// </remarks>
        public new static ClassLanguageModel FromStorageId(string storageId)
        {
            SPXGRAMMARHANDLE hgrammar = SPXHANDLE_INVALID;

            IntPtr textPtr = Utf8StringMarshaler.MarshalManagedToNative(storageId);
            try
            {
                SPX_THROW_ON_FAIL(class_language_model_from_storage_id(out hgrammar, textPtr));
            }
            finally
            {
                Marshal.FreeHGlobal(textPtr);
            }

            return new ClassLanguageModel(hgrammar);
        }

        /// <summary>
        /// Internal constructor. Creates a new instance using the provided native handle.
        /// </summary>
        /// <param name="hgrammar">Grammar handle.</param>
        internal ClassLanguageModel(SPXGRAMMARHANDLE hgrammar) : base(hgrammar) { }

        /// <summary>
        /// Adds a single grammar to the current grammar list
        /// </summary>
        /// <param name="grammar">The grammar to add</param>
        /// <param name="className">The name of the class the grammar represents.</param>
        /// <remarks>
        /// Currently Class Language Models are the only support grammars to add.
        /// </remarks>
        public void AssignClass(string className, Grammar grammar)
        {

            IntPtr textPtr = Utf8StringMarshaler.MarshalManagedToNative(className);
            try
            {
                SPX_THROW_ON_FAIL(class_language_model_assign_class(this.NativeHandle, textPtr, grammar.NativeHandle));
            }
            finally
            {
                Marshal.FreeHGlobal(textPtr);
            }

        }
    }
}
