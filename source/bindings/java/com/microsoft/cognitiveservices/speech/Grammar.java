//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents a generic grammar used to assist in improving speech recogniton accuracy.
 */
public class Grammar implements Closeable {
    
    /**
     * Creates a Grammar from its storage Id.
     *
     * Creating a grammar from a storage ID is only usable in specific scenarios and is not generally possible.
     *
     * @param id The Id of the grammar
     * @return Grammar associated with the given Id.
     */
    public static Grammar fromStorageId(String id) {
        IntRef grammarRef = new IntRef(0);
        Contracts.throwIfFail(fromStorageId(grammarRef, id));
        return new Grammar(grammarRef.getValue());
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        dispose(true);
    }

    /*! \cond INTERNAL */
    
    /**
     * Returns the internal grammar instance
     * @return The internal grammar instance
     */
    public SafeHandle getImpl() {
        return grammarHandle;
    }
    
    /*! \endcond */

    /*! \cond PROTECTED */

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
            if (grammarHandle != null) {
                grammarHandle.close();
                grammarHandle = null;
            }
        }

        disposed = true;
    }

   /**
    * Protected constructor.
    * @param grammarImpl Underlying grammar implementation
    */
    protected Grammar(long grammarHandleValue) {
        Contracts.throwIfNull(grammarHandleValue, "grammarHandleValue");
        this.grammarHandle = new SafeHandle(grammarHandleValue, SafeHandleType.Grammar);
    }
    
    /*! \endcond */

    private final static native long fromStorageId(IntRef grammarHandleRef, String id);
    private SafeHandle grammarHandle = null;
    private boolean disposed = false;
}
