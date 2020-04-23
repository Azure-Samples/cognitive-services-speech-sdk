//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.translation;

import java.util.ArrayList;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.PropertyCollection;

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
     */
    public TranslationRecognizer(com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig stc) {
        super(null);

        Contracts.throwIfNull(stc, "stc");
        Contracts.throwIfNull(recoHandle, "recoHandle");
        Contracts.throwIfFail(createTranslationRecognizerFromConfig(recoHandle, stc.getImpl(), null));
        initialize();
    }

    /**
     * Constructs an instance of a translation recognizer.
     * @param stc speech translation config.
     * @param audioConfig audio config.
     */
    public TranslationRecognizer(com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig stc, AudioConfig audioConfig) {
        super(audioConfig);

        Contracts.throwIfNull(stc, "stc");
        if (audioConfig == null) {
            Contracts.throwIfFail(createTranslationRecognizerFromConfig(recoHandle, stc.getImpl(), null));
        } else {
            Contracts.throwIfFail(createTranslationRecognizerFromConfig(recoHandle, stc.getImpl(), audioConfig.getImpl()));
        }
        initialize();
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public String getSpeechRecognitionLanguage() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Adds a target language for translation.
     * Added in version 1.7.0.
     * @param value the language identifier in BCP-47 format.
     */
    public void addTargetLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        Contracts.throwIfFail(addTargetLanguage(recoHandle, value));
    }

    /**
     * Removes a target language for translation.
     * Added in version 1.7.0.
     * @param value the language identifier in BCP-47 format.
     */
    public void removeTargetLanguage(String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");
        Contracts.throwIfFail(removeTargetLanguage(recoHandle, value));
    }

    /**
     * Gets all target languages that have been configured for translation.
     * @return the list of target languages.
     */
    public ArrayList<String> getTargetLanguages() {

        ArrayList<String> result = new ArrayList<String>();
        String plainStr = propertyHandle.getProperty(PropertyId.SpeechServiceConnection_TranslationToLanguages);
        String[] values = plainStr.split(",");
        for (int i = 0; i < values.length; ++i)
        {
            result.add(values[i]);
        }

        return result;
    }

    /**
     * Gets the name of output voice.
     * @return the name of output voice.
     */
    public String getVoiceName() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceConnection_TranslationVoice);
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
        propertyHandle.setProperty(PropertyId.SpeechServiceAuthorization_Token, token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return propertyHandle.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * The collection of properties and their values defined for this TranslationRecognizer.
     * @return The collection of properties and their values defined for this TranslationRecognizer.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }

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

                Runnable runnable = new Runnable() { public void run() { result[0] = new TranslationRecognitionResult(thisReco.recognize()); }};
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
                Runnable runnable = new Runnable() { public void run() { thisReco.startContinuousRecognition(recoHandle); }};
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
                Runnable runnable = new Runnable() { public void run() { thisReco.stopContinuousRecognition(recoHandle); }};
                thisReco.doAsyncRecognitionAction(runnable);
                return null;
        }});
    }

    /*! \cond PROTECTED */

    @Override
    protected void dispose(final boolean disposing) {

        if (disposed) {
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
                if (propertyHandle != null)
                {
                    propertyHandle.close();
                    propertyHandle = null;
                }
                if (translationSynthesisHandle != null)
                {
                    translationSynthesisHandle.close();
                    translationSynthesisHandle = null;
                }
                if (recoHandle != null)
                {
                    recoHandle.close();
                    recoHandle = null;
                }
                translationRecognizerObjects.remove(this);
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
    static java.util.Set<TranslationRecognizer> translationRecognizerObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<TranslationRecognizer>());

    // TODO Remove this... After tests are updated to no longer depend upon this
    public SafeHandle getRecoImpl() {
        return recoHandle;
    }

    /*! \endcond */

    private void initialize() {
        final TranslationRecognizer _this = this;
        translationSynthesisHandle = new SafeHandle(_this.recoHandle.getValue(), SafeHandleType.TranslationSynthesis);

        this.recognizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(recognizingSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(recognizedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.synthesizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(synthesizingSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(canceledSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(sessionStartedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(sessionStoppedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        this.speechStartDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(speechStartDetectedSetCallback(_this.recoHandle.getValue()));
            }
        });

        this.speechEndDetected.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                translationRecognizerObjects.add(_this);
                Contracts.throwIfFail(speechEndDetectedSetCallback(_this.recoHandle.getValue()));                
            }
        });

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromRecognizerHandle(_this.recoHandle, propHandle));
        propertyHandle = new PropertyCollection(propHandle);
    }

    private void recognizingEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            TranslationRecognitionEventArgs resultEventArg = new TranslationRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<TranslationRecognitionEventArgs> handler = this.recognizing;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private void recognizedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            TranslationRecognitionEventArgs resultEventArg = new TranslationRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<TranslationRecognitionEventArgs> handler = this.recognized;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception e) {}
    }

    private void canceledEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            TranslationRecognitionCanceledEventArgs resultEventArg = new TranslationRecognitionCanceledEventArgs(eventArgs, true);
            EventHandlerImpl<TranslationRecognitionCanceledEventArgs> handler = this.canceled;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }
    
    private void synthesizingEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "recognizer");
            if (this.disposed) {
                return;
            }
            TranslationSynthesisEventArgs resultEventArg = new TranslationSynthesisEventArgs(eventArgs, true);
            EventHandlerImpl<TranslationSynthesisEventArgs> handler = this.synthesizing;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private final native long createTranslationRecognizerFromConfig(SafeHandle recoHandle, SafeHandle speechConfigHandle, SafeHandle audioConfigHandle);
    private final native long synthesizingSetCallback(long recoHandle);
    private final native long addTargetLanguage(SafeHandle recoHandle, String value);
    private final native long removeTargetLanguage(SafeHandle recoHandle, String value);
    private PropertyCollection propertyHandle = null;
    SafeHandle translationSynthesisHandle = null;
    private boolean disposed = false;
}
