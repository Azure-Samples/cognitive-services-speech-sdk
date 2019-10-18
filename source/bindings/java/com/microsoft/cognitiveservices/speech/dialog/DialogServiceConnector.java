//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionCanceledEventArgs;
import com.microsoft.cognitiveservices.speech.PropertyCollection;

/**
 * Class that defines a DialogServiceConnector.
 * Note: close() must be called in order to relinquish underlying resources held by the object.
 */
public class DialogServiceConnector implements Closeable {

    /*! \cond PROTECTED */
    static Class<?> dialogServiceConnector = null;
    private ExecutorService executorService;

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(DialogServiceConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
        dialogServiceConnector = DialogServiceConnector.class;
    }

    /*! \endcond */

    /**
     * Builds a DialogServiceConnector with audio from default microphone input
     * @param config Dialog service connector configuration.
     */
    public DialogServiceConnector(DialogServiceConfig config) {
        this(config, AudioConfig.fromDefaultMicrophoneInput());
    }

    /**
     * Builds a DialogServiceConnector
     * @param config Dialog service connector configuration.
     * @param audioConfig An optional audio input configuration associated with the recognizer
     */
    public DialogServiceConnector(DialogServiceConfig config, AudioConfig audioConfig) {
        Contracts.throwIfNull(config, "config");
        if (audioConfig == null) {
            this.dialogServiceConnectorImpl = com.microsoft.cognitiveservices.speech.internal.DialogServiceConnector.FromConfig(config.getConfigImpl());
        } else {
            this.dialogServiceConnectorImpl = com.microsoft.cognitiveservices.speech.internal.DialogServiceConnector.FromConfig(config.getConfigImpl(), audioConfig.getConfigImpl());
        }

        initialize();
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
        dialogServiceConnectorImpl.SetAuthorizationToken(token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public String getAuthorizationToken() {
        return dialogServiceConnectorImpl.GetAuthorizationToken();
    }

     /**
     * The collection or properties and their values defined for this DialogServiceConnector.
     * @return The collection or properties and their values defined for this DialogServiceConnector.
     */
    public PropertyCollection getProperties() {
        return _Parameters;
    }

    private com.microsoft.cognitiveservices.speech.PropertyCollection _Parameters;

    /**
     * Connects with the service.
     * @return A task representing the asynchronous operation that connects to the service.
     */
    public Future<Void> connectAsync() {
        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                dialogServiceConnectorImpl.ConnectAsync().Get();
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
                dialogServiceConnectorImpl.DisconnectAsync().Get();
                return null;
            }
        });
    }

    /**
     * Sends an activity to the backing dialog.
     * @param activity Activity to be sent.
     * @return A task representing the asynchronous operation that sends an activity to the backing dialog.
     */
    public Future<String> sendActivityAsync(final String activity) {
        Contracts.throwIfNull(activity, "activity");
        return executorService.submit(new java.util.concurrent.Callable<String>() {
            public String call() {
                return dialogServiceConnectorImpl.SendActivityAsync(activity).Get();
            }
        });
    }

    /* This class exists only to sidestep the package protected restriction on the Result constructor */
    private class DialogSpeechRecognitionResult extends SpeechRecognitionResult
    {
        DialogSpeechRecognitionResult(com.microsoft.cognitiveservices.speech.internal.RecognitionResult result)
        {
            super(result);
        }
    }

    /**
     * Starts a listening session that will terminate after the first utterance.
     * @return A task representing the asynchronous operation that starts a one shot listening session.
     */
    public Future<SpeechRecognitionResult> listenOnceAsync() {
        return executorService.submit(new java.util.concurrent.Callable<SpeechRecognitionResult>() {
            public SpeechRecognitionResult  call() {
                return new DialogSpeechRecognitionResult(dialogServiceConnectorImpl.ListenOnceAsync().Get());
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
                dialogServiceConnectorImpl.StartKeywordRecognitionAsync(model.getModelImpl()).Get();
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
                dialogServiceConnectorImpl.StopKeywordRecognitionAsync().Get();
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
        final DialogServiceConnector _this = this;

        executorService = Executors.newCachedThreadPool();

        recognizingHandler = new RecoEventHandlerImpl(this, /* isRecognizedHandler */ false);
        this.recognizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _dialogServiceConnectorObjects.add(_this);
                dialogServiceConnectorImpl.getRecognizing().AddEventListener(recognizingHandler);
            }
        });

        recognizedHandler = new RecoEventHandlerImpl(this, /* isRecognizedHandler */ true);
        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _dialogServiceConnectorObjects.add(_this);
                dialogServiceConnectorImpl.getRecognized().AddEventListener(recognizedHandler);
            }
        });

        sessionStartedHandler = new SessionEventHandlerImpl(this, /* isSessionStart */ true);
        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _dialogServiceConnectorObjects.add(_this);
                dialogServiceConnectorImpl.getSessionStarted().AddEventListener(sessionStartedHandler);
            }
        });

        sessionStoppedHandler = new SessionEventHandlerImpl(this, /* isSessionStopped */ false);
        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _dialogServiceConnectorObjects.add(_this);
                dialogServiceConnectorImpl.getSessionStopped().AddEventListener(sessionStoppedHandler);
            }
        });

        canceledHandler = new CanceledEventHandlerImpl(this);
        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _dialogServiceConnectorObjects.add(_this);
                dialogServiceConnectorImpl.getCanceled().AddEventListener(canceledHandler);
            }
        });

        activityReceivedHandler = new ActivityReceivedEventHandlerImpl(this);
        this.activityReceived.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                _dialogServiceConnectorObjects.add(_this);
                dialogServiceConnectorImpl.getActivityReceived().AddEventListener(activityReceivedHandler);
            }
        });

        _Parameters = new PrivatePropertyCollection(dialogServiceConnectorImpl.getProperties());
    }

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    static java.util.Set<DialogServiceConnector> _dialogServiceConnectorObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<DialogServiceConnector>());

    private RecoEventHandlerImpl recognizingHandler;
    private RecoEventHandlerImpl recognizedHandler;
    private SessionEventHandlerImpl sessionStartedHandler;
    private SessionEventHandlerImpl sessionStoppedHandler;
    private CanceledEventHandlerImpl canceledHandler;
    private ActivityReceivedEventHandlerImpl activityReceivedHandler;

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }

    private class RecoEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionEventListener {

        RecoEventHandlerImpl(DialogServiceConnector connector, boolean isRecognizedHandler) {
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

        private DialogServiceConnector connector;
        private boolean isRecognizedHandler;
    }

    private class SessionEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SessionEventListener {

        SessionEventHandlerImpl(DialogServiceConnector connector, boolean isSessionStart) {
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

        private DialogServiceConnector connector;
        private boolean isSessionStart;
    }

    private class CanceledEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.SpeechRecognitionCanceledEventListener {

        CanceledEventHandlerImpl(DialogServiceConnector connector) {
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

        private DialogServiceConnector connector;
    }

    private class ActivityReceivedEventHandlerImpl extends com.microsoft.cognitiveservices.speech.internal.ActivityReceivedEventListener {

        ActivityReceivedEventHandlerImpl(DialogServiceConnector connector) {
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

        private DialogServiceConnector connector;
    }

    private com.microsoft.cognitiveservices.speech.internal.DialogServiceConnector dialogServiceConnectorImpl;
    /*! \endcond */

    /**
     * Dispose of associated resources.
     * Note: close() must be called in order to relinquish underlying resources held by the object.
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

            // Trigger graceful shutdown of executor service
            executorService.shutdown();

            if (this.recognizing.isUpdateNotificationOnConnectedFired()) {
                dialogServiceConnectorImpl.getRecognizing().RemoveEventListener(recognizingHandler);
            }
            if (this.recognized.isUpdateNotificationOnConnectedFired()) {
                dialogServiceConnectorImpl.getRecognized().RemoveEventListener(recognizedHandler);
            }
            if (this.sessionStarted.isUpdateNotificationOnConnectedFired()) {
                dialogServiceConnectorImpl.getSessionStarted().RemoveEventListener(sessionStartedHandler);
            }
            if (this.sessionStopped.isUpdateNotificationOnConnectedFired()) {
                dialogServiceConnectorImpl.getSessionStopped().RemoveEventListener(sessionStoppedHandler);
            }
            if (this.canceled.isUpdateNotificationOnConnectedFired()) {
                dialogServiceConnectorImpl.getCanceled().RemoveEventListener(canceledHandler);
            }
            if (this.activityReceived.isUpdateNotificationOnConnectedFired()) {
                dialogServiceConnectorImpl.getActivityReceived().RemoveEventListener(activityReceivedHandler);
            }

            recognizingHandler.delete();
            recognizedHandler.delete();
            sessionStartedHandler.delete();
            sessionStoppedHandler.delete();
            canceledHandler.delete();
            activityReceivedHandler.delete();
            dialogServiceConnectorImpl.delete();
            _dialogServiceConnectorObjects.remove(this);

            // If the executor service has not been shut down, force shut down
            if (!executorService.isShutdown()) {
                executorService.shutdownNow();
            }
            disposed = true;
        }
    }
    /*! \endcond */
}
