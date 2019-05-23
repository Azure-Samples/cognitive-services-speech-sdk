//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.translation.TranslationSynthesisEventArgs;

/**
 * Class that defines a SpeechBotConnector.
 */
public class SpeechBotConnector implements Closeable {

    /*! \cond PROTECTED */
    static Class<?> speechBotConnector = null;
    private static ExecutorService executorService;

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
        speechBotConnector = SpeechBotConnector.class;
        executorService = Executors.newCachedThreadPool();
    }

    /*! \endcond */

    /**
     * Builds a SpeechBotConnector with audio from default microphone input
     * @param config Bot connector configuration.
     */
    public SpeechBotConnector(BotConnectorConfig config) {
        this(config, AudioConfig.fromDefaultMicrophoneInput());
    }

    /**
     * Builds a SpeechBotConnector
     * @param config Bot connector configuration.
     * @param audioConfig An optional audio input configuration associated with the recognizer
     */
    public SpeechBotConnector(BotConnectorConfig config, AudioConfig audioConfig) {
        Contracts.throwIfNull(config, "config");
        if (audioConfig == null) {
            this.speechBotConnectorImpl = com.microsoft.cognitiveservices.speech.internal.SpeechBotConnector.FromConfig(config.getBotConfigImpl());
        } else {
            this.speechBotConnectorImpl = com.microsoft.cognitiveservices.speech.internal.SpeechBotConnector.FromConfig(config.getBotConfigImpl(), audioConfig.getConfigImpl());
        }
        initialize();
    }

    /**
     * Connects with the service.
     * @return A task representing the asynchronous operation that connects to the service.
     */
    public Future<Void> connectAsync() {
        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                speechBotConnectorImpl.ConnectAsync().Get();
                return null;
            }
        });
    }

    /**
     * Disconnects from the service.
     * @return A task representing the asynchronous operation that disconnects from the service.
     */
    public Future<Void> disconnectAsync() {
        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                speechBotConnectorImpl.DisconnectAsync().Get();
                return null;
            }
        });
    }

    /**
     * Sends an activity to the backing bot.
     * @param activity Activity to be sent.
     * @return A task representing the asynchronous operation that sends an activity to the bot.
     */
    public Future<String> sendActivityAsync(final BotConnectorActivity activity) {
        Contracts.throwIfNull(activity, "activity");
        return executorService.submit(new java.util.concurrent.Callable<String>() {
            public String call() {
                return speechBotConnectorImpl.SendActivityAsync(activity.getImpl()).Get();
            }
        });
    }

    /**
     * Starts a listening session that will terminate after the first utterance.
     * @return A task representing the asynchronous operation that starts a one shot listening session.
     */
    public Future<Void> listenOnceAsync() {
        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                speechBotConnectorImpl.ListenOnceAsync().Get();
                return null;
            }
        });
    }

    /**
     * Initiates keyword recognition.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @return A task representing the asynchronous operation that starts keyword recognition.
     */
    public Future<Void> startKeywordRecognitionAsync(final KeywordRecognitionModel model) {
        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                speechBotConnectorImpl.StartKeywordRecognitionAsync(model.getModelImpl()).Get();
                return null;
            }
        });
    }

    /**
     * Stop keyword recognition.
     * @return A task representing the asynchronous operation that stops keyword recognition.
     */
    public Future<Void> stopKeywordRecognitionAsync() {
        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                speechBotConnectorImpl.StopKeywordRecognitionAsync().Get();
                return null;
            }
        });
    }

    /**
     * Defines event handler for the recognizing event.
     */
    public EventHandlerImpl<SpeechRecognitionEventArgs> recognizing = new EventHandlerImpl<SpeechRecognitionEventArgs>();

    /**
     * Defines event handler for the recognized event.
     */
    public EventHandlerImpl<SpeechRecognitionEventArgs> recognized = new EventHandlerImpl<SpeechRecognitionEventArgs>();

    /**
     * Defines event handler for the session start event.
     */
    public EventHandlerImpl<SessionEventArgs> sessionStarted = new EventHandlerImpl<SessionEventArgs>();

    /**
     * Defines event handler for the session stop event.
     */
    public EventHandlerImpl<SessionEventArgs> sessionStopped = new EventHandlerImpl<SessionEventArgs>();

    /**
     * Defines event handler for the canceled event.
     */
    public EventHandlerImpl<SpeechRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<SpeechRecognitionCanceledEventArgs>();

    /**
     * Defines event handler for the activity received event.
     */
    public EventHandlerImpl<ActivityReceivedEventArgs> activityReceived = new EventHandlerImpl<ActivityReceivedEventArgs>();

    /*! \cond PROTECTED */
    private void initialize() {
        recognizingHandler = new RecoEventHandlerImpl(this, /* isRecognizedHandler */ false);
        speechBotConnectorImpl.getRecognizing().AddEventListener(recognizingHandler);
        recognizedHandler = new RecoEventHandlerImpl(this, /* isRecognizedHandler */ true);
        speechBotConnectorImpl.getRecognized().AddEventListener(recognizedHandler);
        sessionStartedHandler = new SessionEventHandlerImpl(this, /* isSessionStart */ true);
        speechBotConnectorImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
        sessionStoppedHandler = new SessionEventHandlerImpl(this, /* isSessionStopped */ false);
        speechBotConnectorImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
        canceledHandler = new CanceledEventHandlerImpl(this);
        speechBotConnectorImpl.getCanceled().AddEventListener(canceledHandler);
        activityReceivedHandler = new ActivityReceivedEventHandlerImpl(this);
        speechBotConnectorImpl.getActivityReceived().AddEventListener(activityReceivedHandler);
    }

    private RecoEventHandlerImpl recognizingHandler;
    private RecoEventHandlerImpl recognizedHandler;
    private SessionEventHandlerImpl sessionStartedHandler;
    private SessionEventHandlerImpl sessionStoppedHandler;
    private CanceledEventHandlerImpl canceledHandler;
    private ActivityReceivedEventHandlerImpl activityReceivedHandler;

    private class RecoEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventListener {

        RecoEventHandlerImpl(SpeechBotConnector connector, boolean isRecognizedHandler) {
            Contracts.throwIfNull(connector, "connector");
            this.connector = connector;
            this.isRecognizedHandler = isRecognizedHandler;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (connector.disposed) {
                return;
            }
            SpeechRecognitionEventArgs resultEventArg = new SpeechRecognitionEventArgs(eventArgs);
            EventHandlerImpl<SpeechRecognitionEventArgs> handler = this.isRecognizedHandler ? this.connector.recognized : this.connector.recognizing;
            if (handler != null) {
                handler.fireEvent(this.connector, resultEventArg);
            }
        }

        private SpeechBotConnector connector;
        private boolean isRecognizedHandler;
    }

    private class SessionEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SessionEventListener {

        SessionEventHandlerImpl(SpeechBotConnector connector, boolean isSessionStart) {
            Contracts.throwIfNull(connector, "connector");
            this.connector = connector;
            this.isSessionStart = isSessionStart;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SessionEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (connector.disposed) {
                return;
            }
            SessionEventArgs resultEventArg = new SessionEventArgs(eventArgs);
            EventHandlerImpl<SessionEventArgs> handler = this.isSessionStart ? this.connector.sessionStarted : this.connector.sessionStopped;
            if (handler != null) {
                handler.fireEvent(this.connector, resultEventArg);
            }
        }

        private SpeechBotConnector connector;
        private boolean isSessionStart;
    }

    private class CanceledEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventListener {

        CanceledEventHandlerImpl(SpeechBotConnector connector) {
            Contracts.throwIfNull(connector, "connector");
            this.connector = connector;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (connector.disposed) {
                return;
            }
            SpeechRecognitionCanceledEventArgs resultEventArg = new SpeechRecognitionCanceledEventArgs(eventArgs);
            EventHandlerImpl<SpeechRecognitionCanceledEventArgs> handler = this.connector.canceled;
            if (handler != null) {
                handler.fireEvent(this.connector, resultEventArg);
            }
        }

        private SpeechBotConnector connector;
    }

    private class ActivityReceivedEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.ActivityReceivedEventListener {

        ActivityReceivedEventHandlerImpl(SpeechBotConnector connector) {
            Contracts.throwIfNull(connector, "connector");
            this.connector = connector;
        }

        @Override
        public void Execute(com.microsoft.cognitiveservices.speech.internal.ActivityReceivedEventArgs eventArgs) {
            Contracts.throwIfNull(eventArgs, "eventArgs");
            if (connector.disposed) {
                return;
            }
            ActivityReceivedEventArgs resultEventArg = new ActivityReceivedEventArgs(eventArgs);
            EventHandlerImpl<ActivityReceivedEventArgs> handler = this.connector.activityReceived;
            if (handler != null) {
                handler.fireEvent(this.connector, resultEventArg);
            }
        }

        private SpeechBotConnector connector;
    }

    private com.microsoft.cognitiveservices.speech.internal.SpeechBotConnector speechBotConnectorImpl;
    /*! \endcond */

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        dispose(true);
    }

    /*! \cond PROTECTED */
    private boolean disposed = false;

    protected void dispose(boolean disposing) {
        if (disposed) {
            return;
        }
        if (disposing) {
            speechBotConnectorImpl.getRecognizing().RemoveEventListener(recognizingHandler);
            speechBotConnectorImpl.getRecognized().RemoveEventListener(recognizedHandler);
            speechBotConnectorImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
            speechBotConnectorImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
            speechBotConnectorImpl.getCanceled().RemoveEventListener(canceledHandler);
            speechBotConnectorImpl.getActivityReceived().RemoveEventListener(activityReceivedHandler);

            recognizingHandler.delete();
            recognizedHandler.delete();
            sessionStartedHandler.delete();
            sessionStoppedHandler.delete();
            canceledHandler.delete();
            activityReceivedHandler.delete();

            speechBotConnectorImpl.delete();
        }
        disposed = true;
    }
    /*! \endcond */
}
