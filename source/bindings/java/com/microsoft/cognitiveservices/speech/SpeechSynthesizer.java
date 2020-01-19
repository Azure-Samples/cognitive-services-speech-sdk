//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Performs speech synthesis to speaker, file, or other audio output streams, and gets synthesized audio as result.
 * Added in version 1.7.0
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

    private com.microsoft.cognitiveservices.speech.internal.SpeechSynthesizer synthImpl;

    private ResultHandlerImpl synthesisStartedHandler;
    private ResultHandlerImpl synthesizingHandler;
    private ResultHandlerImpl synthesisCompletedHandler;
    private ResultHandlerImpl synthesisCanceledHandler;
    private WordBoundaryHandlerImpl wordBoundaryHandler;

    private com.microsoft.cognitiveservices.speech.PropertyCollection parameters;

    private boolean disposed = false;
    private final Object synthesizerLock = new Object();
    private int activeAsyncSynthesisCounter = 0;

    /**
     * Initializes a new instance of Speech Synthesizer.
     * @param speechConfig speech configuration.
     */
    public SpeechSynthesizer(SpeechConfig speechConfig) {
        Contracts.throwIfNull(speechConfig, "speechConfig");
        this.synthImpl = com.microsoft.cognitiveservices.speech.internal.SpeechSynthesizer.FromConfig(speechConfig.getImpl());
        initialize();
    }

    /**
     * Initializes a new instance of Speech Synthesizer.
     * @param speechConfig speech configuration.
     * @param audioConfig audio configuration.
     */
    public SpeechSynthesizer(SpeechConfig speechConfig, AudioConfig audioConfig) {
        Contracts.throwIfNull(speechConfig, "speechConfig");

        com.microsoft.cognitiveservices.speech.internal.AudioConfig audioConfigImpl = audioConfig == null ? null : audioConfig.getConfigImpl();
        this.synthImpl = com.microsoft.cognitiveservices.speech.internal.SpeechSynthesizer.FromConfig(speechConfig.getImpl(), audioConfigImpl);

        initialize();
    }

    /**
     * Execute the speech synthesis on plain text, synchronously.
     * @param text The plain text for synthesis.
     * @return The synthesis result.
     */
    public SpeechSynthesisResult SpeakText(String text) {
        return new SpeechSynthesisResult(this.synthImpl.SpeakText(text));
    }

    /**
     * Execute the speech synthesis on SSML, synchronously.
     * @param ssml The SSML for synthesis.
     * @return The synthesis result.
     */
    public SpeechSynthesisResult SpeakSsml(String ssml) {
        return new SpeechSynthesisResult(this.synthImpl.SpeakSsml(ssml));
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

                Runnable runnable = new Runnable() { public void run() { result[0] = new SpeechSynthesisResult(synthImpl.SpeakText(finalText)); }};
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

                Runnable runnable = new Runnable() { public void run() { result[0] = new SpeechSynthesisResult(synthImpl.SpeakSsml(finalSsml)); }};
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
        return new SpeechSynthesisResult(this.synthImpl.StartSpeakingText(text));
    }

    /**
     * Start the speech synthesis on SSML, synchronously.
     * @param ssml The SSML for synthesis.
     * @return The synthesis result.
     */
    public SpeechSynthesisResult StartSpeakingSsml(String ssml) {
        return new SpeechSynthesisResult(this.synthImpl.StartSpeakingSsml(ssml));
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

                Runnable runnable = new Runnable() { public void run() { result[0] = new SpeechSynthesisResult(synthImpl.StartSpeakingText(finalText)); }};
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

                Runnable runnable = new Runnable() { public void run() { result[0] = new SpeechSynthesisResult(synthImpl.StartSpeakingSsml(finalSsml)); }};
                thisSynth.doAsyncSynthesisAction(runnable);

                return result[0];
            }});
    }

    /**
     * The collection of properties and their values defined for this SpeechSynthesizer.
     * @return The collection of properties and their values defined for this SpeechSynthesizer.
     */
    public PropertyCollection getProperties() {
        return this.parameters;
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
        synthImpl.SetAuthorizationToken(token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return synthImpl.GetAuthorizationToken();
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

        this.synthesisStartedHandler = new ResultHandlerImpl(this, this.SynthesisStarted);
        this.SynthesisStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                synthImpl.getSynthesisStarted().AddEventListener(synthesisStartedHandler);
            }
        });

        this.synthesizingHandler = new ResultHandlerImpl(this, this.Synthesizing);
        this.Synthesizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                synthImpl.getSynthesizing().AddEventListener(synthesizingHandler);
            }
        });

        this.synthesisCompletedHandler = new ResultHandlerImpl(this, this.SynthesisCompleted);
        this.SynthesisCompleted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                synthImpl.getSynthesisCompleted().AddEventListener(synthesisCompletedHandler);
            }
        });

        this.synthesisCanceledHandler = new ResultHandlerImpl(this, this.SynthesisCanceled);
        this.SynthesisCanceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                synthImpl.getSynthesisCanceled().AddEventListener(synthesisCanceledHandler);
            }
        });

        this.wordBoundaryHandler = new WordBoundaryHandlerImpl(this, this.WordBoundary);
        this.WordBoundary.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                s_speechSynthesizerObjects.add(_this);
                synthImpl.getWordBoundary().AddEventListener(wordBoundaryHandler);
            }
        });

        this.parameters = new PrivatePropertyCollection(this.synthImpl.getProperties());
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
                if (this.SynthesisStarted.isUpdateNotificationOnConnectedFired())
                    this.synthImpl.getSynthesisStarted().RemoveEventListener(this.synthesisStartedHandler);
                if (this.Synthesizing.isUpdateNotificationOnConnectedFired())
                    this.synthImpl.getSynthesizing().RemoveEventListener(this.synthesizingHandler);
                if (this.SynthesisCompleted.isUpdateNotificationOnConnectedFired())
                    this.synthImpl.getSynthesisCompleted().RemoveEventListener(this.synthesisCompletedHandler);
                if (this.SynthesisCanceled.isUpdateNotificationOnConnectedFired())
                    this.synthImpl.getSynthesisCanceled().RemoveEventListener(this.synthesisCanceledHandler);
                if (this.WordBoundary.isUpdateNotificationOnConnectedFired())
                    this.synthImpl.getWordBoundary().RemoveEventListener(this.wordBoundaryHandler);

                this.synthesisStartedHandler.delete();
                this.synthesizingHandler.delete();
                this.synthesisCompletedHandler.delete();
                this.synthesisCanceledHandler.delete();
                this.wordBoundaryHandler.delete();
                this.synthImpl.delete();
                this.parameters.close();
                s_speechSynthesizerObjects.remove(this);
                this.disposed = true;
            }
        }
    }

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    // Defines a private class to raise an event for synthesis result when a corresponding callback is invoked by the native layer.
    private class ResultHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisEventListener {

        private SpeechSynthesizer synthesizer;
        private EventHandlerImpl<SpeechSynthesisEventArgs> synthesisHandler;

        ResultHandlerImpl(SpeechSynthesizer synthesizer, EventHandlerImpl<SpeechSynthesisEventArgs> synthesisHandler) {
            Contracts.throwIfNull(synthesizer, "synthesizer");

            this.synthesizer = synthesizer;
            this.synthesisHandler = synthesisHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (this.synthesizer.disposed) {
                return;
            }

            SpeechSynthesisEventArgs resultEventArg = new SpeechSynthesisEventArgs(eventArgs);
            EventHandlerImpl<SpeechSynthesisEventArgs> handler = this.synthesisHandler;
            if (handler != null) {
                handler.fireEvent(this.synthesizer, resultEventArg);
            }

            resultEventArg.close();
        }
    }

    // Defines a private class to raise an event for word boundary when a corresponding callback is invoked by the native layer.
    private class WordBoundaryHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisWordBoundaryEventListener {

        private SpeechSynthesizer synthesizer;
        private EventHandlerImpl<SpeechSynthesisWordBoundaryEventArgs> wordBoundaryHandler;

        WordBoundaryHandlerImpl(SpeechSynthesizer synthesizer, EventHandlerImpl<SpeechSynthesisWordBoundaryEventArgs> wordBoundaryHandler) {
            Contracts.throwIfNull(synthesizer, "synthesizer");

            this.synthesizer = synthesizer;
            this.wordBoundaryHandler = wordBoundaryHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechSynthesisWordBoundaryEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");

            if (this.synthesizer.disposed) {
                return;
            }

            SpeechSynthesisWordBoundaryEventArgs wordBoundaryEventArgs = new SpeechSynthesisWordBoundaryEventArgs(eventArgs);
            EventHandlerImpl<SpeechSynthesisWordBoundaryEventArgs> handler = this.wordBoundaryHandler;
            if (handler != null) {
                handler.fireEvent(this.synthesizer, wordBoundaryEventArgs);
            }
        }
    }
}
