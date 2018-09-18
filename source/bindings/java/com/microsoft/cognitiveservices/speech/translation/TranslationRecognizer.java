//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import java.util.ArrayList;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;

/**
 * Performs translation on the speech input.
 */
public final class TranslationRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer
{
    /**
     * The event recognizing signals that an intermediate recognition result is received.
     */
    public final EventHandlerImpl<TranslationTextResultEventArgs> recognizing = new EventHandlerImpl<TranslationTextResultEventArgs>();

    /**
     * The event recognized signals that a final recognition result is received.
     */
    public final EventHandlerImpl<TranslationTextResultEventArgs> recognized = new EventHandlerImpl<TranslationTextResultEventArgs>();

    /**
     * The event canceled signals that the recognition/translation was canceled.
     */
    public final EventHandlerImpl<TranslationTextResultCanceledEventArgs> canceled = new EventHandlerImpl<TranslationTextResultCanceledEventArgs>();

    /**
     * The event synthesizing signals that a translation synthesis result is received.
     */
    public final EventHandlerImpl<TranslationSynthesisResultEventArgs> synthesizing = new EventHandlerImpl<TranslationSynthesisResultEventArgs>();

    /**
     * Constructs an instance of a translation recognizer.
     * @param recoImpl The internal recognizer implementation.
     * @param audioInput An optional audio input configuration associated with the recognizer
     */
    private TranslationRecognizer(com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer recoImpl, AudioConfig audioInput) {
        super(audioInput);

        Contracts.throwIfNull(recoImpl, "recoImpl");
        this.recoImpl = recoImpl;

        recognizingHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ false);
        recoImpl.getRecognizing().AddEventListener(recognizingHandler);

        recognizedHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ true);
        recoImpl.getRecognized().AddEventListener(recognizedHandler);

        synthesisResultHandler = new SynthesisHandlerImpl(this);
        recoImpl.getSynthesizing().AddEventListener(synthesisResultHandler);

        errorHandler = new CanceledHandlerImpl(this);
        recoImpl.getCanceled().AddEventListener(errorHandler);

        recoImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
        recoImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
        recoImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
        recoImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);

        _Parameters = new PrivatePropertyCollection(recoImpl.getProperties());
    }

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    /**
     * Constructs an instance of a translation recognizer.
     * @param stc speech translation config.
     * @param audioConfig audio config.
     */
    public TranslationRecognizer(com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig stc, AudioConfig audioConfig) {
        this(com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer.FromConfig(stc.getImpl(), audioConfig.getConfigImpl()), audioConfig);
    }

    /**
     * Constructs an instance of a translation recognizer.
     * @param stc speech translation config.
     */
    public TranslationRecognizer(com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig stc) {
        this(com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer.FromConfig(stc.getImpl(), null), null);
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return _Parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets target languages for translation that were set when the recognizer was created.
     * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
     * @return Gets target languages for translation that were set when the recognizer was created.
     */
    public ArrayList<String> getTargetLanguages() {
        return new ArrayList<String>(_TargetLanguages);
    }

    ArrayList<String> _TargetLanguages = new ArrayList<String>();

    /**
     * Gets the name of output voice.
     * @return the name of output voice.
     */
    public String getVoiceName() {
        return _Parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * @param value Authorization token.
     */
    public void setAuthorizationToken(String token) {
        Contracts.throwIfNullOrWhitespace(token, "token");
        recoImpl.SetAuthorizationToken(token);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return recoImpl.GetAuthorizationToken();
    }

    /**
     * The collection or properties and their values defined for this TranslationRecognizer.
     * @return The collection or properties and their values defined for this TranslationRecognizer.
     */
    public PropertyCollection getProperties() {
        return _Parameters;
    }

    private PropertyCollection _Parameters;

    /**
     * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitableonly for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of TranslationTextResult.
     */
    public Future<TranslationTextResult> recognizeOnceAsync() {
        return s_executorService.submit(() -> {
            return new TranslationTextResult(recoImpl.Recognize());
        });
    }

    /**
     * Starts recognition and translation on a continuous audio stream, until StopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive translation results.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public Future<Void> startContinuousRecognitionAsync() {
        return s_executorService.submit(() -> {
            recoImpl.StartContinuousRecognition();
            return null;
        });
    }

    /**
     * Stops continuous recognition and translation.
     * @return A task representing the asynchronous operation that stops the translation.
     */
    public Future<Void> stopContinuousRecognitionAsync() {
        return s_executorService.submit(() -> {
            recoImpl.StopContinuousRecognition();
            return null;
        });
    }

    @Override
    protected void dispose(boolean disposing)
    {
        if (disposed)
        {
            return;
        }

        if (disposing)
        {
            recoImpl.getRecognizing().RemoveEventListener(recognizingHandler);
            recoImpl.getRecognized().RemoveEventListener(recognizedHandler);
            recoImpl.getCanceled().RemoveEventListener(errorHandler);
            recoImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
            recoImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
            recoImpl.getSpeechStartDetected().RemoveEventListener(speechStartDetectedHandler);
            recoImpl.getSpeechEndDetected().RemoveEventListener(speechEndDetectedHandler);
            recoImpl.getSynthesizing().RemoveEventListener(synthesisResultHandler);

            recognizingHandler.delete();
            recognizedHandler.delete();
            errorHandler.delete();
            recoImpl.delete();
            _Parameters.close();
            disposed = true;
            super.dispose(disposing);
        }
    }

    // TODO Remove this... After tests are updated to no longer depend upon this
    public com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer getRecoImpl() {
        return recoImpl;
    }

    private final com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer recoImpl;
    private ResultHandlerImpl recognizingHandler;
    private ResultHandlerImpl recognizedHandler;
    private SynthesisHandlerImpl synthesisResultHandler;
    private CanceledHandlerImpl errorHandler;
    private boolean disposed = false;

    // Defines an internal class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class ResultHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationTexEventListener
    {
        public ResultHandlerImpl(TranslationRecognizer recognizer, boolean isRecognizedHandler)
        {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
            this.isRecognizedHandler = isRecognizedHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.TranslationTextResultEventArgs eventArgs)
        {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (recognizer.disposed)
            {
                return;
            }

            TranslationTextResultEventArgs resultEventArg = new TranslationTextResultEventArgs(eventArgs);
            EventHandlerImpl<TranslationTextResultEventArgs>  handler = isRecognizedHandler ? recognizer.recognized : recognizer.recognizing;
            if (handler != null)
            {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
        private boolean isRecognizedHandler;
    }

    // Defines an internal class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    class CanceledHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationTexCanceledEventListener {
        public CanceledHandlerImpl(TranslationRecognizer recognizer) {
            Contracts.throwIfNull(recognizer, "recognizer");
            this.recognizer = recognizer;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.TranslationTextResultCanceledEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (recognizer.disposed) {
                return;
            }

            TranslationTextResultCanceledEventArgs resultEventArg = new TranslationTextResultCanceledEventArgs(eventArgs);
            EventHandlerImpl<TranslationTextResultCanceledEventArgs> handler = this.recognizer.canceled;

            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
    }

    // Defines an internal class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class SynthesisHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisEventListener
    {
        public SynthesisHandlerImpl(TranslationRecognizer recognizer)
        {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResultEventArgs eventArgs)
        {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (recognizer.disposed)
            {
                return;
            }

            TranslationSynthesisResultEventArgs resultEventArg = new TranslationSynthesisResultEventArgs(eventArgs);
            EventHandlerImpl<TranslationSynthesisResultEventArgs> handler = recognizer.synthesizing;
            if (handler != null)
            {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
    }
}
