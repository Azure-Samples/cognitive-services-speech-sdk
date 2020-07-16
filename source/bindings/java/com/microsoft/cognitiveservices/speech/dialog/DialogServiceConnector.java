//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.dialog;

import java.io.Closeable;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.util.EventHandlerImpl;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.StringRef;
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
    private AtomicInteger eventCounter = new AtomicInteger(0);
    protected Integer backgroundAttempts = 0;
    private AudioConfig audioInputKeepAlive = null;

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
            this.dialogServiceConnectorHandle = new SafeHandle(0, SafeHandleType.DialogServiceConnector);
            Contracts.throwIfFail(createDialogServiceConnectorFomConfig(this.dialogServiceConnectorHandle, config.getImpl(), null));
        } else {
            this.dialogServiceConnectorHandle = new SafeHandle(0, SafeHandleType.DialogServiceConnector);
            Contracts.throwIfFail(createDialogServiceConnectorFomConfig(this.dialogServiceConnectorHandle, config.getImpl(), audioConfig.getImpl()));
        }
        audioInputKeepAlive = audioConfig;
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
     * The collection of properties and their values defined for this DialogServiceConnector.
     * @return The collection of properties and their values defined for this DialogServiceConnector.
     */
    public PropertyCollection getProperties() {
        return propertyHandle;
    }

    private com.microsoft.cognitiveservices.speech.PropertyCollection propertyHandle;

    /**
     * Connects with the service.
     * @return A task representing the asynchronous operation that connects to the service.
     */
    public Future<Void> connectAsync() {
        return executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void call() {
                Contracts.throwIfFail(connect(dialogServiceConnectorHandle));
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
                Contracts.throwIfFail(disconnect(dialogServiceConnectorHandle));
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
                StringRef stringRef = new StringRef("");
                Contracts.throwIfFail(sendActivity(dialogServiceConnectorHandle, stringRef, activity));
                return stringRef.getValue();
            }
        });
    }

    /* This class exists only to sidestep the package protected restriction on the Result constructor */
    private class DialogSpeechRecognitionResult extends SpeechRecognitionResult
    {
        DialogSpeechRecognitionResult(long resultHandle) {
            super(resultHandle);
        }
    }

    /**
     * Starts a listening session that will terminate after the first utterance.
     * @return A task representing the asynchronous operation that starts a one shot listening session.
     */
    public Future<SpeechRecognitionResult> listenOnceAsync() {
        return executorService.submit(new java.util.concurrent.Callable<SpeechRecognitionResult>() {
            public SpeechRecognitionResult  call() {
                IntRef result = new IntRef(0);
                Contracts.throwIfFail(listenOnce(dialogServiceConnectorHandle, result));
                return new DialogSpeechRecognitionResult(result.getValue());
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
                Contracts.throwIfFail(startKeywordRecognition(dialogServiceConnectorHandle, model.getImpl()));
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
                Contracts.throwIfFail(stopKeywordRecognition(dialogServiceConnectorHandle));
                return null;
            }
        });
    }

    /**
     * Defines event handler for the recognizing event.
     */
    public EventHandlerImpl<SpeechRecognitionEventArgs> recognizing = new EventHandlerImpl<SpeechRecognitionEventArgs>(eventCounter);

    /**
     * Defines event handler for the recognized event.
     */
    public EventHandlerImpl<SpeechRecognitionEventArgs> recognized = new EventHandlerImpl<SpeechRecognitionEventArgs>(eventCounter);

    /**
     * Defines event handler for the session start event.
     */
    public EventHandlerImpl<SessionEventArgs> sessionStarted = new EventHandlerImpl<SessionEventArgs>(eventCounter);

    /**
     * Defines event handler for the session stop event.
     */
    public EventHandlerImpl<SessionEventArgs> sessionStopped = new EventHandlerImpl<SessionEventArgs>(eventCounter);

    /**
     * Defines event handler for the canceled event.
     */
    public EventHandlerImpl<SpeechRecognitionCanceledEventArgs> canceled = new EventHandlerImpl<SpeechRecognitionCanceledEventArgs>(eventCounter);

    /**
     * Defines event handler for the activity received event.
     */
    public EventHandlerImpl<ActivityReceivedEventArgs> activityReceived = new EventHandlerImpl<ActivityReceivedEventArgs>(eventCounter);

    /*! \cond PROTECTED */

    private void initialize() {
        final DialogServiceConnector _this = this;

        executorService = Executors.newCachedThreadPool();

        this.recognizing.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                dialogServiceConnectorObjects.add(_this);
                Contracts.throwIfFail(recognizingSetCallback(_this.dialogServiceConnectorHandle.getValue()));
            }
        });

        this.recognized.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                dialogServiceConnectorObjects.add(_this);
                Contracts.throwIfFail(recognizedSetCallback(_this.dialogServiceConnectorHandle.getValue()));
            }
        });

        this.sessionStarted.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                dialogServiceConnectorObjects.add(_this);
                Contracts.throwIfFail(sessionStartedSetCallback(_this.dialogServiceConnectorHandle.getValue()));
            }
        });

        this.sessionStopped.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                dialogServiceConnectorObjects.add(_this);
                Contracts.throwIfFail(sessionStoppedSetCallback(_this.dialogServiceConnectorHandle.getValue()));
            }
        });

        this.canceled.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                dialogServiceConnectorObjects.add(_this);
                Contracts.throwIfFail(canceledSetCallback(_this.dialogServiceConnectorHandle.getValue()));
            }
        });

        this.activityReceived.updateNotificationOnConnected(new Runnable(){
            @Override
            public void run() {
                dialogServiceConnectorObjects.add(_this);
                Contracts.throwIfFail(activityReceivedSetCallback(_this.dialogServiceConnectorHandle.getValue()));
            }
        });

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromDialogServiceConnectorHandle(_this.dialogServiceConnectorHandle, propHandle));
        propertyHandle = new PropertyCollection(propHandle);
    }

    /**
     * This is used to keep any instance of this class alive that is subscribed to downstream events.
     */
    static java.util.Set<DialogServiceConnector> dialogServiceConnectorObjects = java.util.Collections.synchronizedSet(new java.util.HashSet<DialogServiceConnector>());

    private void recognizingEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "connector");
            if (this.disposed) {
                return;
            }
            SpeechRecognitionEventArgs resultEventArg = new SpeechRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<SpeechRecognitionEventArgs> handler = this.recognizing;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private void recognizedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "connector");
            if (this.disposed) {
                return;
            }
            SpeechRecognitionEventArgs resultEventArg = new SpeechRecognitionEventArgs(eventArgs, true);
            EventHandlerImpl<SpeechRecognitionEventArgs> handler = this.recognized;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }
        } catch (Exception e) {}
    }

    private void sessionStartedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "connector");
            if (this.disposed) {
                return;
            }
            SessionEventArgs arg = new SessionEventArgs(eventArgs, true);
            EventHandlerImpl<SessionEventArgs> handler = this.sessionStarted;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }    
        } catch (Exception e) {}
    }

    private void sessionStoppedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "connector");
            if (this.disposed) {
                return;
            }
            SessionEventArgs arg = new SessionEventArgs(eventArgs, true);
            EventHandlerImpl<SessionEventArgs> handler = this.sessionStopped;
            if (handler != null) {
                handler.fireEvent(this, arg);
            }    
        } catch (Exception e) {}
    }

    private void canceledEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "connector");
            if (this.disposed) {
                return;
            }
            SpeechRecognitionCanceledEventArgs resultEventArg = new SpeechRecognitionCanceledEventArgs(eventArgs, true);
            EventHandlerImpl<SpeechRecognitionCanceledEventArgs> handler = this.canceled;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private void activityReceivedEventCallback(long eventArgs)
    {
        try {
            Contracts.throwIfNull(this, "connector");
            if (this.disposed) {
                return;
            }
            ActivityReceivedEventArgs resultEventArg = new ActivityReceivedEventArgs(eventArgs);
            EventHandlerImpl<ActivityReceivedEventArgs> handler = this.activityReceived;
            if (handler != null) {
                handler.fireEvent(this, resultEventArg);
            }    
        } catch (Exception e) {}
    }

    private SafeHandle dialogServiceConnectorHandle = null;

    /*! \endcond */

    /*! \cond INTERNAL */
    /**
     * Returns the internal conversation translator instance
     * @return The internal conversation translator instance
     */
    public SafeHandle getImpl() {
        return dialogServiceConnectorHandle;
    }

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

    protected void dispose(final boolean disposing) {
        if (disposed) {
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
                // Trigger graceful shutdown of executor service
                executorService.shutdown();

                if (propertyHandle != null)
                {
                    propertyHandle.close();
                    propertyHandle = null;
                }
                if (dialogServiceConnectorHandle != null)
                {
                    dialogServiceConnectorHandle.close();
                    dialogServiceConnectorHandle = null;
                }

                audioInputKeepAlive = null;
                dialogServiceConnectorObjects.remove(this);

                // If the executor service has not been shut down, force shut down
                if (!executorService.isShutdown()) {
                    executorService.shutdownNow();
                }
                disposed = true;
            }
        }
    }
    /*! \endcond */

    private final static native long createDialogServiceConnectorFomConfig(SafeHandle dialogServiceConnectorHandle, SafeHandle configHandle, SafeHandle audioConfigHandle);
    private final native long connect(SafeHandle dialogServiceConnectorHandle);
    private final native long disconnect(SafeHandle dialogServiceConnectorHandle);
    private final native long sendActivity(SafeHandle dialogServiceConnectorHandle, StringRef stringRef, String activity);
    private final native long listenOnce(SafeHandle dialogServiceConnectorHandle, IntRef result);
    private final native long startKeywordRecognition(SafeHandle dialogServiceConnectorHandle, SafeHandle modelHandle);
    private final native long stopKeywordRecognition(SafeHandle dialogServiceConnectorHandle);
    private final native long recognizingSetCallback(long dialogServiceConnectorHandle);
    private final native long recognizedSetCallback(long dialogServiceConnectorHandle);
    private final native long sessionStartedSetCallback(long dialogServiceConnectorHandle);
    private final native long sessionStoppedSetCallback(long dialogServiceConnectorHandle);
    private final native long canceledSetCallback(long dialogServiceConnectorHandle);
    private final native long activityReceivedSetCallback(long dialogServiceConnectorHandle);
    private final native long getPropertyBagFromDialogServiceConnectorHandle(SafeHandle dialogServiceConnectorHandle, IntRef propHandle);

}
