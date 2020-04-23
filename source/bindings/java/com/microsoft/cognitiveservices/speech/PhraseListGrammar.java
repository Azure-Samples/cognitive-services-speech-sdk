//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.Recognizer;

/**
 * Allows additions of new phrases to improve speech recognition.
 *
 * Phrases added to the recognizer are effective at the start of the next recognition, or the next time the SpeechSDK must reconnect
 * to the speech service.
 * Added in version 1.5.0.
 */
public final class PhraseListGrammar extends Grammar implements Closeable {
    /**
     * Creates a PhraseListGrammar from a given speech recognizer. Will accept any recognizer that derives from Recognizer.
     * @param recognizer The recognizer to add phrase lists to.
     * @return PhraseListGrammar associated to the recognizer.
     */
    public static PhraseListGrammar fromRecognizer(Recognizer recognizer) {
        IntRef grammarRef = new IntRef(0);
        Contracts.throwIfFail(fromRecognizer(grammarRef, recognizer.getImpl()));
        return new PhraseListGrammar(grammarRef.getValue());
    }

    /**
     * Adds a single phrase to the current recognizer.
     * @param phrase Phrase to add.
     */
    public void addPhrase(String phrase) {
        Contracts.throwIfFail(addPhrase(getImpl(), phrase));
    }

    /**
     * Clears all phrases added to the current recognizer.
     */
     public void clear() {
        Contracts.throwIfFail(clear(getImpl()));
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
     * This method performs cleanup of resources.
     * The Boolean parameter disposing indicates whether the method is called from Dispose (if disposing is true) or from the finalizer (if disposing is false).
     * Derived classes should override this method to dispose resource if needed.
     * @param disposing Flag to request disposal.
     */
    protected void dispose(boolean disposing) {
        if (disposed) {
            return;
        }

        super.dispose(disposing);

        disposed = true;
    }

    /*! \endcond */

    private boolean disposed = false;

    private PhraseListGrammar(long handleValue) {
        super(handleValue);
    }

    private final static native long fromRecognizer(IntRef grammarHandleRef, SafeHandle recoHandle);
    private final native long clear(SafeHandle grammarHandle);
    private final native long addPhrase(SafeHandle grammarHandle, String phrase);
}
