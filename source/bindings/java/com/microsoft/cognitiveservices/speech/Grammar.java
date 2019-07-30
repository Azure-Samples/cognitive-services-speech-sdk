//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents a generic grammar used to assist in improving speech recogniton accuracy.
 */
public class Grammar implements Closeable
{
    /**
     * Creates a Grammar from its storage Id.
     *
     * Creating a grammar from a storage ID is only usable in specific scenarios and is not generally possible.
     *
     * @param id The Id of the grammar
     * @return Grammar associated with the given Id.
     */
    public static Grammar fromStorageId(String id)
    {
        com.microsoft.cognitiveservices.speech.internal.Grammar grammarImpl = com.microsoft.cognitiveservices.speech.internal.Grammar.FromStorageId(id);
        return new Grammar(grammarImpl);
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        dispose(true);
    }

    /*! \cond PROTECTED */

    /**
     * Returns the internal grammar instance
     * @return The internal grammar instance
     */
    public com.microsoft.cognitiveservices.speech.internal.Grammar getGrammarImpl()
    {
        return grammarImpl;
    }

    /**
     * This method performs cleanup of resources.
     * The Boolean parameter disposing indicates whether the method is called from Dispose (if disposing is true) or from the finalizer (if disposing is false).
     * Derived classes should override this method to dispose resource if needed.
     * @param disposing Flag to request disposal.
     */
    protected void dispose(boolean disposing) {
        if (disposed) {
            return;
        }

        if (disposing) {
            grammarImpl.delete();
        }

        disposed = true;
    }

    /*! \endcond */

   /**
    * Protected constructor.
    * @param grammarImpl Underlying grammar implementation
    */
    protected Grammar(com.microsoft.cognitiveservices.speech.internal.Grammar grammarImpl)
    {
        Contracts.throwIfNull(grammarImpl, "GrammarInternalImplementation");
        this.grammarImpl = grammarImpl;
    }

    private com.microsoft.cognitiveservices.speech.internal.Grammar grammarImpl;
    private boolean disposed = false;
}
