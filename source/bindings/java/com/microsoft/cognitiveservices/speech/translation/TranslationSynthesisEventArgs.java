//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;

/**
 * Defines the content of translation synthesis result event.
 */
public final class TranslationSynthesisEventArgs extends SessionEventArgs
{
    /*! \cond INTERNAL */

    /**
     * Constructs an instance of a TranslationRecognitionEventArgs object.
     * @param eventArgs recognition event args object.
     */
    TranslationSynthesisEventArgs(long eventArgs) {
        super(eventArgs);
        storeEventData(false);

    }
    
    TranslationSynthesisEventArgs(long eventArgs, boolean dispose) {
        super(eventArgs);
        storeEventData(dispose);
    }

    /*! \endcond */

    /**
     * Specifies the translation synthesis result.
     * @return Specifies the translation synthesis result.
     */
    public final TranslationSynthesisResult getResult() {
        return result;
    }

    /**
     * Returns a String that represents the translation recognition result event.
     * @return A String that represents the translation recognition result event.
     */
    @Override
    public String toString()
    {
        return "SessionId:" + this.getSessionId() + " Result:" + result.toString()  + ".";
    }

    private void storeEventData(boolean disposeNativeResources) {
        Contracts.throwIfNull(eventHandle, "eventHandle");
        IntRef resultHandle = new IntRef(0);
        Contracts.throwIfFail(getSynthesisResult(eventHandle, resultHandle));
        this.result = new TranslationSynthesisResult(resultHandle.getValue());
        Contracts.throwIfNull(this.getSessionId(), "SessionId");
        if (disposeNativeResources == true) {
            super.close();
        }
    }
    
    private TranslationSynthesisResult result;

    private final native long getSynthesisResult(SafeHandle eventHandle, IntRef resultHandle);
}
