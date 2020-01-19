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
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 */
public final class TranslationRecognizer extends com.microsoft.cognitiveservices.speech.Recognizer
{
    /**
     * The event recognizing signals that an intermediate recognition result is received.
     */
    public final EventHandlerImpl<TranslationRecognitionEventArgs> recognizing = new EventHandlerImpl<TranslationRecognitionEventArgs>(eventCounter);

    /**
     * The event recognized signals that a final recognition result is received.
     */
    public final EventHandlerImpl<TranslationRecognitionEventArgs> recognized = new EventHandlerImpl<TranslationRecognitionEventArgs>(eventCounter);

    /**
     * The event canceled signals that the recognition/translation was canceled.
     */
    public final EventHandlerImpl<TranslationRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<TranslationRecognitionCanceledEventArgs>(eventCounter);

    /**
     * The event synthesizing signals that a translation synthesis result is received.
     */
    public final EventHandlerImpl<TranslationSynthesisEventArgs> synthesizing = new EventHandlerImpl<TranslationSynthesisEventArgs>(eventCounter);

    /**
     * Constructs an instance of a translation recognizer.
     * @param stc speech translation config.
     * @param audioConfig audio config.
     */
    public TranslationRecognizer(com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig stc, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(stc, "stc");
        if (audioConfig == null) {
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer.FromConfig(stc.getImpl());
        }
        else {
            this.recoImpl = com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer.FromConfig(stc.getImpl(), audioConfig.getConfigImpl());
        }

        initialize();
    }

    /**
     * Constructs an instance of a translation recognizer.
     * @param stc speech translation config.
     */
    public TranslationRecognizer(com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig stc) {
        super(null);

        Contracts.throwIfNull(stc, "stc");
        this.recoImpl = com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer.FromConfig(stc.getImpl());
        initialize();
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return _Parameters.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Adds a target language for translation.
     * Added in version 1.7.0.
     * @param value the language identifier in BCP-47 format.
     */
    public void addTargetLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        recoImpl.AddTargetLanguage(value);
    }

    /**
     * Removes a target language for translation.
     * Added in version 1.7.0.
     * @param value the language identifier in BCP-47 format.
     */
    public void removeTargetLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        recoImpl.RemoveTargetLanguage(value);
    }

    /**
     * Gets all target languages that have been configured for translation.
     * @return the list of target languages.
     */
    public ArrayList<String> getTargetLanguages() {

        ArrayList<String> result = new ArrayList<String>();
        com.microsoft.cognitiveservices.speech.internal.StringVector v = recoImpl.GetTargetLanguages();
        for (int i = 0; i < v.size(); ++i)
        {
            result.add(v.get(i));
        }

        return result;
    }

    /**
     * Gets the name of output voice.
     * @return the name of output voice.
     */
    public String getVoiceName() {
        return _Parameters.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
     * the caller needs to refresh it by calling this setter with a new valid token.
     * Otherwise, the recognizer will encounter errors during recognition.
     * @param token Authorization token.
     */
    public void setAuthorizationToken(String token) {
        Contracts.throwIfNullOrWhitespace(token, "token");
        recoImpl.SetAuthorizationToken(token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return recoImpl.GetAuthorizationToken();
    }

    /**
     * The collection of properties and their values defined for this TranslationRecognizer.
     * @return The collection of properties and their values defined for this TranslationRecognizer.
     */
    public PropertyCollection getProperties() {
        return _Parameters;
    }

    private PropertyCollection _Parameters;

    /**
     * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitableonly for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of TranslationRecognitionResult.
     */
    public Future<TranslationRecognitionResult> recognizeOnceAsync() {
        final TranslationRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<TranslationRecognitionResult>() {
            public TranslationRecognitionResult  call() {
                // A variable defined in an enclosing scope must be final or effectively final.
                // The compiler treats an array initialized once as an effectively final.
                final TranslationRecognitionResult[] result = new TranslationRecognitionResult[1];

                Runnable runnable = new Runnable() { public void run() { result[0] = new TranslationRecognitionResult(recoImpl.Recognize()); }};
                thisReco.doAsyncRecognitionAction(runnable);

                return result[0];
        }});
    }

    /**
     * Starts recognition and translation on a continuous audio stream, until StopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive translation results.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public Future<Void> startContinuousRecognitionAsync() {
        final TranslationRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { recoImpl.StartContinuousRecognition(); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /**
     * Stops continuous recognition and translation.
     * @return A task representing the asynchronous operation that stops the translation.
     */
    public Future<Void> stopContinuousRecognitionAsync() {
        final TranslationRecognizer thisReco = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { recoImpl.StopContinuousRecognition(); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /*! \cond PROTECTED */

    @Override
    protected void dispose(final boolean disposing)
    {
        if (disposed)
        {
            return;
        }

        if (disposing) {
            if(this.eventCounter.get() != 0) {
                // There is an event callback in progress, closing while in an event call results in SWIG problems, so 
                // spin a thread to try again in 500ms and return.
                getProperties().getProperty("Backgrounding release", ""); 
                Thread t = new Thread(
                    new Runnable(){
                        @Override
                        public void run() {
                            try{
                                Thread.sleep(500);
                                dispose(disposing);
                            } catch (Exception e){}
                        }
                    });
                t.start();
            }
            else {
                if (this.recognizing.isUpdateNotificationOnConnectedFired())
                    recoImpl.getRecognizing().RemoveEventListener(recognizingHandler);
                if (this.recognized.isUpdateNotificationOnConnectedFired())
                    recoImpl.getRecognized().RemoveEventListener(recognizedHandler);
                if (this.synthesizing.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSynthesizing().RemoveEventListener(synthesisResultHandler);
                if (this.canceled.isUpdateNotificationOnConnectedFired())
                    recoImpl.getCanceled().RemoveEventListener(errorHandler);
                if (this.sessionStarted.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
                if (this.sessionStopped.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
                if (this.speechStartDetected.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSpeechStartDetected().RemoveEventListener(speechStartDetectedHandler);
                if (this.speechEndDetected.isUpdateNotificationOnConnectedFired())
                    recoImpl.getSpeechEndDetected().RemoveEventListener(speechEndDetectedHandler);

                recognizingHandler.delete();
                recognizedHandler.delete();
                errorHandler.delete();
                recoImpl.delete();
                _Parameters.close();

                _translationRecognizerObjects.remove(this);
                disposed = true;
                super.dispose(disposing);
            }
        }
    }

    /*! \endcond */

    /*! \cond INTERNAL */

   /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    static java.util.Set<TranslationRecognizer> _translationRecognizerObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<TranslationRecognizer>());

    // TODO Remove this... After tests are updated to no longer depend upon this
    public com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer getRecoImpl() {
        return recoImpl;
    }

    /*! \endcond */

    private void initialize() {
        super.internalRecognizerImpl = this.recoImpl;

        final TranslationRecognizer _this = this;

        recognizingHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ false);
        this.recognizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getRecognizing().AddEventListener(recognizingHandler);
            }
        });

        recognizedHandler = new ResultHandlerImpl(this, /*isRecognizedHandler:*/ true);
        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getRecognized().AddEventListener(recognizedHandler);
            }
        });

        synthesisResultHandler = new SynthesisHandlerImpl(this);
        this.synthesizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getSynthesizing().AddEventListener(synthesisResultHandler);
            }
        });

        errorHandler = new CanceledHandlerImpl(this);
        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getCanceled().AddEventListener(errorHandler);
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
            }
        });

        this.speechStartDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getSpeechStartDetected().AddEventListener(speechStartDetectedHandler);
            }
        });

        this.speechEndDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _translationRecognizerObjects.add(_this);
                recoImpl.getSpeechEndDetected().AddEventListener(speechEndDetectedHandler);
            }
        });

        _Parameters = new PrivatePropertyCollection(recoImpl.getProperties());
    }

    private final com.microsoft.cognitiveservices.speech.internal.TranslationRecognizer recoImpl;
    private ResultHandlerImpl recognizingHandler;
    private ResultHandlerImpl recognizedHandler;
    private SynthesisHandlerImpl synthesisResultHandler;
    private CanceledHandlerImpl errorHandler;
    private boolean disposed = false;

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    // Defines a private class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class ResultHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationTexEventListener
    {
        public ResultHandlerImpl(TranslationRecognizer recognizer, boolean isRecognizedHandler)
        {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
            this.isRecognizedHandler = isRecognizedHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.TranslationRecognitionEventArgs eventArgs)
        {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (recognizer.disposed)
            {
                return;
            }

            TranslationRecognitionEventArgs resultEventArg = new TranslationRecognitionEventArgs(eventArgs);
            EventHandlerImpl<TranslationRecognitionEventArgs>  handler = isRecognizedHandler ? recognizer.recognized : recognizer.recognizing;
            if (handler != null)
            {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
        private boolean isRecognizedHandler;
    }

    // Defines a private class to raise an event for error during recognition when a corresponding callback is invoked by the native layer.
    private class CanceledHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationTexCanceledEventListener {
        public CanceledHandlerImpl(TranslationRecognizer recognizer) {
            Contracts.throwIfNull(recognizer, "recognizer");
            this.recognizer = recognizer;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.TranslationRecognitionCanceledEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (recognizer.disposed) {
                return;
            }

            TranslationRecognitionCanceledEventArgs resultEventArg = new TranslationRecognitionCanceledEventArgs(eventArgs);
            EventHandlerImpl<TranslationRecognitionCanceledEventArgs> handler = this.recognizer.canceled;

            if (handler != null) {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
    }

    // Defines a private class to raise an event for intermediate/final result when a corresponding callback is invoked by the native layer.
    private class SynthesisHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisEventListener
    {
        public SynthesisHandlerImpl(TranslationRecognizer recognizer)
        {
            Contracts.throwIfNull(recognizer, "recognizer");

            this.recognizer = recognizer;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisEventArgs eventArgs)
        {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (recognizer.disposed)
            {
                return;
            }

            TranslationSynthesisEventArgs resultEventArg = new TranslationSynthesisEventArgs(eventArgs);
            EventHandlerImpl<TranslationSynthesisEventArgs> handler = recognizer.synthesizing;
            if (handler != null)
            {
                handler.fireEvent(this.recognizer, resultEventArg);
            }
        }

        private TranslationRecognizer recognizer;
    }
}
