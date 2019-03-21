//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;

namespace Microsoft.CognitiveServices.Speech
{
    using Internal;
    using static Internal.c_interop;

    /*! \cond INTERNAL */
    using SPXGRAMMARHANDLE = IntPtr;
    /*! \endcond */

    /// <summary>
    /// Represents base class grammar for customizing speech recognition.
    /// </summary>
    public class Grammar
    {
        /// <summary>
        /// Internal constructor. Creates a new instance using the provided native handle.
        /// </summary>
        /// <param name="hgrammar">Grammar handle.</param>
        internal Grammar(SPXGRAMMARHANDLE hgrammar)
        {
            m_hgrammar = new InteropSafeHandle(hgrammar, grammar_handle_release);
        }
        
        /// <summary>
        /// Internal native handle property.
        /// </summary>
        internal InteropSafeHandle NativeHandle { get { return m_hgrammar; } }

        private InteropSafeHandle m_hgrammar;
    }
}
