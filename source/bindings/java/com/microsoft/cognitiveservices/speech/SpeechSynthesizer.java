//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Performs speech synthesis to speaker, file, or other audio output streams, and gets synthesized audio as result.
 * Updated in version 1.14.0
 */
public class SpeechSynthesizer implements Closeable
{
    private AtomicInteger eventCounter = new AtomicInteger(0);

    /**
     * Defines event handler for synthesis started event.
     */
    final public EventHandlerImpl<SpeechSynthesisEventArgs> SynthesisStarted = new EventHandlerImpl<SpeechSynthesisEventArgs>(eventCounter);

    /**
     * Defines event handler for synthesizing event.
     */
    final public EventHandlerImpl<SpeechSynthesisEventArgs> Synthesizing = new EventHandlerImpl<SpeechSynthesisEventArgs>(eventCounter);

    /**
     * Defines event handler for synthesis completed event.
     */
    final public EventHandlerImpl<SpeechSynthesisEventArgs> SynthesisCompleted = new EventHandlerImpl<SpeechSynthesisEventArgs>(eventCounter);

    /**
     * Defines event handler for synthesis canceled event.
     */
    final public EventHandlerImpl<SpeechSynthesisEventArgs> SynthesisCanceled = new EventHandlerImpl<SpeechSynthesisEventArgs>(eventCounter);

    /**
     * Defines event handler for word boundary event.
     */
    final public EventHandlerImpl<SpeechSynthesisWordBoundaryEventArgs> WordBoundary = new EventHandlerImpl<SpeechSynthesisWordBoundaryEventArgs>(eventCounter);

    private static ExecutorService s_executorService;

    static {
        s_executorService = Executors.newCachedThreadPool();
    }

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    private static java.util.Set<SpeechSynthesizer> s_speechSynthesizerObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<SpeechSynthesizer>());

    private SafeHandle synthHandle = null;

    private com.microsoft.cognitiveservices.speech.PropertyCollection propertyHandle;

    private boolean disposed = false;
    private final Object synthesizerLock = new Object();
    private int activeAsyncSynthesisCounter = 0;
    private AudioConfig audioOutputKeepAlive = null;

    /**
     * Creates a new instance of Speech Synthesizer. Uses the default speaker on the system for audio output.
     * @param speechConfig speech configuration.
     */
    public SpeechSynthesizer(SpeechConfig speechConfig) {
        Contracts.throwIfNull(speechConfig, "speechConfig");
        synthHandle = new SafeHandle(0, SafeHandleType.Synthesizer);
        AudioConfig audioConfig = AudioConfig.fromDefaultSpeakerOutput();
        Contracts.throwIfFail(createSpeechSynthesizerFromConfig(synthHandle, speechConfig.getImpl(), audioConfig.getImpl()));
        Contracts.throwIfNull(synthHandle.getValue(), "synthHandle");
        audioConfig.close();
        initialize();
    }

    /**
     * Creates a new instance of Speech Synthesizer. If audioConfig is null, there is no audio output.
     * @param speechConfig speech configuration.
     * @param audioConfig audio configuration.
     */
    public SpeechSynthesizer(SpeechConfig speechConfig, AudioConfig audioConfig) {
        Contracts.throwIfNull(speechConfig, "speechConfig");

        synthHandle = new SafeHandle(0, SafeHandleType.Synthesizer);
        Contracts.throwIfFail(createSpeechSynthesizerFromConfig(synthHandle, speechConfig.getImpl(), audioConfig == null ? null : audioConfig.getImpl()));
        Contracts.throwIfNull(synthHandle.getValue(), "synthHandle");
        audioOutputKeepAlive = audioConfig;
        initialize();
    }

    /**
     * Creates a new instance of Speech Synthesizer.
     * @param speechConfig speech configuration.
     * @param autoDetectSourceLangConfig the configuration for auto detecting source language
     * @param audioConfig audio configuration.
     */
    public SpeechSynthesizer(SpeechConfig speechConfig, AutoDetectSourceLanguageConfig autoDetectSourceLangConfig, AudioConfig audioConfig) {
        Contracts.throwIfNull(speechConfig, "speechConfig");
        Contracts.throwIfNull(autoDetectSourceLangConfig, "autoDetectSourceLangConfig");

        synthHandle = new SafeHandle(0, SafeHandleType.Synthesizer);
        Contracts.throwIfFail(createSpeechSynthesizerFromAutoDetectSourceLangConfig(synthHandle, speechConfig.getImpl(), autoDetectSourceLangConfig.getImpl(), audioConfig == null ? null : audioConfig.getImpl()));
        Contracts.throwIfNull(synthHandle.getValue(), "synthHandle");
        audioOutputKeepAlive = audioConfig;
        initialize();
    }

    /**
     * Execute the speech synthesis on plain text, synchronously.
     * @param text The plain text for synthesis.
     * @return The synthesis result.
     */
    public SpeechSynthesisResult SpeakText(String text) {
        IntRef resultRef = new IntRef(0);
        Contracts.throwIfFail(speakText(synthHandle, text, resultRef));
        return new SpeechSynthesisResult(resultRef);
    }

    /**
     * Execute the speech synthesis on SSML, synchronously.
     * @param ssml The SSML for synthesis.
     * @return The synthesis result.
     */
    public SpeechSynthesisResult SpeakSsml(String ssml) {
        IntRef resultRef = new IntRef(0);
        Contracts.throwIfFail(speakSsml(synthHandle, ssml, resultRef));
        return new SpeechSynthesisResult(resultRef);
    }

    /**
     * Execute the speech synthesis on plain text, asynchronously.
     * @param text The plain text for synthesis.
     * @return A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.
     */
    public Future<SpeechSynthesisResult> SpeakTextAsync(String text) {
        final SpeechSynthesizer thisSynth = this;
        final String finalText = text;

        return s_executorService.submit(new java.util.concurrent.Callable<SpeechSynthesisResult>() {
            public SpeechSynthesisResult  call() {
                // A variable defined in an enclosing scope must be final or effectively final.
                // The compiler treats an array initialized once as an effectively final.
                final SpeechSynthesisResult[] result = new SpeechSynthesisResult[1];

                Runnable runnable = new Runnable() { public void run() { 
                    IntRef resultRef = new IntRef(0);
                    Contracts.throwIfFail(speakText(synthHandle, finalText, resultRef));
                    result[0] = new SpeechSynthesisResult(resultRef);
                }};
                thisSynth.doAsyncSynthesisAction(runnable);

                return result[0];
            }});
    }

    /**
     * Execute the speech synthesis on SSML, asynchronously.
     * @param ssml The SSML for synthesis.
     * @return A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.
     */
    public Future<SpeechSynthesisResult> SpeakSsmlAsync(String ssml) {
        final SpeechSynthesizer thisSynth = this;
        final String finalSsml = ssml;

        return s_executorService.submit(new java.util.concurrent.Callable<SpeechSynthesisResult>() {
            public SpeechSynthesisResult  call() {
                // A variable defined in an enclosing scope must be final or effectively final.
                // The compiler treats an array initialized once as an effectively final.
                final SpeechSynthesisResult[] result = new SpeechSynthesisResult[1];

                Runnable runnable = new Runnable() { public void run() {
                    IntRef resultRef = new IntRef(0);
                    Contracts.throwIfFail(speakSsml(synthHandle, finalSsml, resultRef));
                    result[0] = new SpeechSynthesisResult(resultRef);
                }};
                thisSynth.doAsyncSynthesisAction(runnable);

                return result[0];
            }});
    }

    /**
     * Start the speech synthesis on plain text, synchronously.
     * @param text The plain text for synthesis.
     * @return The synthesis result.
     */
    public SpeechSynthesisResult StartSpeakingText(String text) {
        IntRef resultRef = new IntRef(0);
        Contracts.throwIfFail(startSpeakingText(synthHandle, text, resultRef));
        return new SpeechSynthesisResult(resultRef);
    }

    /**
     * Start the speech synthesis on SSML, synchronously.
     * @param ssml The SSML for synthesis.
     * @return The synthesis result.
     */
    public SpeechSynthesisResult StartSpeakingSsml(String ssml) {
        IntRef resultRef = new IntRef(0);
        Contracts.throwIfFail(startSpeakingSsml(synthHandle, ssml, resultRef));
        return new SpeechSynthesisResult(resultRef);
    }

    /**
     * Start the speech synthesis on plain text, asynchronously.
     * @param text The plain text for synthesis.
     * @return A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.
     */
    public Future<SpeechSynthesisResult> StartSpeakingTextAsync(String text) {
        final SpeechSynthesizer thisSynth = this;
        final String finalText = text;

        return s_executorService.submit(new java.util.concurrent.Callable<SpeechSynthesisResult>() {
            public SpeechSynthesisResult  call() {
                // A variable defined in an enclosing scope must be final or effectively final.
                // The compiler treats an array initialized once as an effectively final.
                final SpeechSynthesisResult[] result = new SpeechSynthesisResult[1];

                Runnable runnable = new Runnable() { public void run() {
                    IntRef resultRef = new IntRef(0);
                    Contracts.throwIfFail(startSpeakingText(synthHandle, finalText, resultRef));
                    result[0] = new SpeechSynthesisResult(resultRef);
                }};
                thisSynth.doAsyncSynthesisAction(runnable);

                return result[0];
            }});
    }

    /**
     * Start the speech synthesis on SSML, asynchronously.
     * @param ssml The SSML for synthesis.
     * @return A task representing the synthesis operation. The task returns a value of SpeechSynthesisResult.
     */
    public Future<SpeechSynthesisResult> StartSpeakingSsmlAsync(String ssml) {
        final SpeechSynthesizer thisSynth = this;
        final String finalSsml = ssml;

        return s_executorService.submit(new java.util.concurrent.Callable<SpeechSynthesisResult>() {
            public SpeechSynthesisResult  call() {
                // A variable defined in an enclosing scope must be final or effectively final.
                // The compiler treats an array initialized once as an effectively final.
                final SpeechSynthesisResult[] result = new SpeechSynthesisResult[1];

                Runnable runnable = new Runnable() { public void run() {
                    IntRef resultRef = new IntRef(0);
                    Contracts.throwIfFail(startSpeakingSsml(synthHandle, finalSsml, resultRef));
                    result[0] = new SpeechSynthesisResult(resultRef);
                }};
                thisSynth.doAsyncSynthesisAction(runnable);

                return result[0];
            }});
    }

    /**
     * Stop the synthesis, asynchronously.
     * This method will stop the playback and clear the unread data in <see cref="PullAudioOutputStream"/>.
     * Added in version 1.14.0
     * @return A task representing the asynchronous operation that stops the synthesis.
     */
    public Future<Void> StopSpeakingAsync() {
        final SpeechSynthesizer thisSync = this;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Runnable runnable = new Runnable() { public void run() { stopSpeaking(synthHandle); }};
                thisSync.doAsyncSynthesisAction(runnable);
                return null;
        }});
    }

    /**
     * The collection of properties and their values defined for this SpeechSynthesizer.
     * @return The collection of properties and their values defined for this SpeechSynthesizer.
     */
    public PropertyCollection getProperties() {
        return this.propertyHandle;
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token expires,
     * the caller needs to refresh it by calling this setter with a new valid token.
     * Otherwise, the synthesizer will encounter errors while speech synthesis.
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
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        synchronized (synthesizerLock) {
            if (activeAsyncSynthesisCounter != 0) {
                throw new IllegalStateException("Cannot dispose a synthesizer while async synthesis is running. Await async synthesis to avoid unexpected disposals.");
            }
            dispose(true);
        }
    }

    private void initialize() {
        final SpeechSynthesizer _this = this;

        this.SynthesisStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                Contracts.throwIfFail(synthesisStartedSetCallback(_this.synthHandle));
            }
        });

        this.Synthesizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                Contracts.throwIfFail(synthesizingSetCallback(_this.synthHandle));
            }
        });

        this.SynthesisCompleted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                Contracts.throwIfFail(synthesisCompletedSetCallback(_this.synthHandle));
            }
        });

        this.SynthesisCanceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                Contracts.throwIfFail(synthesisCanceledSetCallback(_this.synthHandle));
            }
        });

        this.WordBoundary.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                Contracts.throwIfFail(wordBoundarySetCallback(_this.synthHandle));
            }
        });

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromSynthesizerHandle(_this.synthHandle, propHandle));
        propertyHandle = new PropertyCollection(propHandle);
    }

    private void doAsyncSynthesisAction(Runnable synthImplAction) {
        synchronized (synthesizerLock) {
            activeAsyncSynthesisCounter++;
        }
        if (disposed) {
            throw new IllegalStateException(this.getClass().getName());
        }
        try {
            synthImplAction.run();
        } finally {
            synchronized (synthesizerLock) {
                activeAsyncSynthesisCounter--;
            }
        }
    }

    private Integer backgroundAttempts = 0;

    private void dispose(final boolean disposing)
    {
        if (this.disposed) {
            return;
        }

        if (disposing) {
            if(this.eventCounter.get() != 0 && backgroundAttempts <= 50 ) {
                // There is an event callback in progress, closing while in an event call results in SWIG problems, so 
                // spin a thread to try again in 500ms and return.
                Thread t = new Thread(
                    new Runnable(){
                        @Override
                        public void run() {
                            try{
                                Thread.sleep(500 * ++backgroundAttempts);
                                dispose(disposing);
                            } catch (Exception e){}
                        }
                    });
                t.start();
            }
            else {
                if (this.propertyHandle != null) {
                    this.propertyHandle.close();
                    this.propertyHandle = null;
                }
                if (this.synthHandle != null) {
                    this.synthHandle.close();
                    this.synthHandle = null;
                }
                audioOutputKeepAlive = null;
                s_speechSynthesizerObjects.remove(this);
                this.disposed = true;
            }
        }
    }

    private void synthesisStartedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "synthesizer");
            if (this.disposed) {
                return;
            }
            SpeechSynthesisEventArgs resultEventArg = new SpeechSynthesisEventArgs(eventArgs);
            EventHandlerImpl<SpeechSynthesisEventArgs> handler = this.SynthesisStarted;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception e) {}
    }

    private void synthesizingEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "synthesizer");
            if (this.disposed) {
                return;
            }
            SpeechSynthesisEventArgs resultEventArg = new SpeechSynthesisEventArgs(eventArgs);
            EventHandlerImpl<SpeechSynthesisEventArgs> handler = this.Synthesizing;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception e) {}
    }

    private void synthesisCompletedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "synthesizer");
            if (this.disposed) {
                return;
            }
            SpeechSynthesisEventArgs resultEventArg = new SpeechSynthesisEventArgs(eventArgs);
            EventHandlerImpl<SpeechSynthesisEventArgs> handler = this.SynthesisCompleted;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception e) {}
    }

    private void synthesisCanceledEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "synthesizer");
            if (this.disposed) {
                return;
            }
            SpeechSynthesisEventArgs resultEventArg = new SpeechSynthesisEventArgs(eventArgs);
            EventHandlerImpl<SpeechSynthesisEventArgs> handler = this.SynthesisCanceled;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private void wordBoundaryEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "synthesizer");
            if (this.disposed) {
                return;
            }
            SpeechSynthesisWordBoundaryEventArgs resultEventArg = new SpeechSynthesisWordBoundaryEventArgs(eventArgs);
            EventHandlerImpl<SpeechSynthesisWordBoundaryEventArgs> handler = this.WordBoundary;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private final native long createSpeechSynthesizerFromConfig(SafeHandle synthHandle, SafeHandle speechConfigHandle, SafeHandle audioConfigHandle);
    private final native long createSpeechSynthesizerFromAutoDetectSourceLangConfig(SafeHandle synthHandle, SafeHandle speechConfigHandle, SafeHandle autoDetectSourceLangConfigHandle, SafeHandle audioConfigHandle);
    private final native long speakText(SafeHandle synthHandle, String text, IntRef resultRef);
    private final native long speakSsml(SafeHandle synthHandle, String ssml, IntRef resultRef);
    private final native long startSpeakingText(SafeHandle synthHandle, String text, IntRef resultRef);
    private final native long startSpeakingSsml(SafeHandle synthHandle, String ssml, IntRef resultRef);
    private final native long stopSpeaking(SafeHandle synthHandle);
    private final native long synthesisStartedSetCallback(SafeHandle synthHandle);
    private final native long synthesizingSetCallback(SafeHandle synthHandle);
    private final native long synthesisCompletedSetCallback(SafeHandle synthHandle);
    private final native long synthesisCanceledSetCallback(SafeHandle synthHandle);
    private final native long wordBoundarySetCallback(SafeHandle synthHandle);
    private final native long getPropertyBagFromSynthesizerHandle(SafeHandle synthHandle, IntRef propHandle);
}
