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
import com.microsoft.cognitiveservices.speech.Recognizer;

/**
 * Allows additions of new phrases to improve speech recognition.
 *
 * Phrases added to the recognizer are effective at the start of the next recognition, or the next time the SpeechSDK must reconnect
 * to the speech service.
 * Added in version 1.5.0.
 */
public final class PhraseListGrammar implements Closeable
{
    /**
     * Creates a PhraseListGrammar from a given speech recognizer. Will accept any recognizer that derives from Recognizer.
     * @param recognizer The recognizer to add phrase lists to.
     * @return PhraseListGrammar associated to the recognizer.
     */
    public static PhraseListGrammar fromRecognizer(Recognizer recognizer)
    {
        com.microsoft.cognitiveservices.speech.internal.PhraseListGrammar phraseListGrammarImpl = com.microsoft.cognitiveservices.speech.internal.PhraseListGrammar.FromRecognizer(recognizer.getRecognizerImpl());
        return new PhraseListGrammar(phraseListGrammarImpl);
    }

    /**
     * Adds a single phrase to the current recognizer.
     * @param phrase Phrase to add.
     */
    public void addPhrase(String phrase)
    {
        phraseListGrammarImpl.AddPhrase(phrase);
    }

    /**
     * Clears all phrases added to the current recognizer.
     */
     public void clear()
     {
        phraseListGrammarImpl.Clear();
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

        if (disposing) {
            phraseListGrammarImpl.delete();
        }

        disposed = true;
    }

    /*! \endcond */

    private com.microsoft.cognitiveservices.speech.internal.PhraseListGrammar phraseListGrammarImpl;
    private boolean disposed = false;

    private PhraseListGrammar(com.microsoft.cognitiveservices.speech.internal.PhraseListGrammar phraseListGrammarImpl)
    {
        Contracts.throwIfNull(phraseListGrammarImpl, "RecognizerInternalImplementation");
        this.phraseListGrammarImpl = phraseListGrammarImpl;
    }
}
