// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ReplayableAudioNode } from "../common.browser/Exports";
import {
    ArgumentNullError,
    ConnectionClosedEvent,
    ConnectionEvent,
    ConnectionMessage,
    ConnectionOpenResponse,
    ConnectionState,
    createNoDashGuid,
    Deferred,
    IAudioSource,
    IAudioStreamNode,
    IConnection,
    IDisposable,
    IStreamChunk,
    MessageType,
    Promise,
    PromiseHelper,
    PromiseResult,
} from "../common/Exports";
import { AudioStreamFormatImpl } from "../sdk/Audio/AudioStreamFormat";
import {
    CancellationErrorCode,
    CancellationReason,
    PropertyId,
    RecognitionEventArgs,
    Recognizer,
    SessionEventArgs,
    SpeechRecognitionResult,
} from "../sdk/Exports";
import {
    RequestSession,
    SpeechDetected,
} from "./Exports";
import {
    AuthInfo,
    IAuthentication,
} from "./IAuthentication";
import { IConnectionFactory } from "./IConnectionFactory";
import { RecognizerConfig } from "./RecognizerConfig";
import { SpeechConnectionMessage } from "./SpeechConnectionMessage.Internal";

export abstract class ServiceRecognizerBase implements IDisposable {
    private privAuthentication: IAuthentication;
    private privConnectionFactory: IConnectionFactory;
    private privAudioSource: IAudioSource;
    private privSpeechConfigConnectionId: string;
    private privConnectionConfigurationPromise: Promise<IConnection>;
    private privConnectionId: string;
    private privAuthFetchEventId: string;
    private privIsDisposed: boolean;
    private privRecognizer: Recognizer;
    protected privRecognizerConfig: RecognizerConfig;

    public constructor(
        authentication: IAuthentication,
        connectionFactory: IConnectionFactory,
        audioSource: IAudioSource,
        recognizerConfig: RecognizerConfig,
        recognizer: Recognizer) {

        if (!authentication) {
            throw new ArgumentNullError("authentication");
        }

        if (!connectionFactory) {
            throw new ArgumentNullError("connectionFactory");
        }

        if (!audioSource) {
            throw new ArgumentNullError("audioSource");
        }

        if (!recognizerConfig) {
            throw new ArgumentNullError("recognizerConfig");
        }

        this.privAuthentication = authentication;
        this.privConnectionFactory = connectionFactory;
        this.privAudioSource = audioSource;
        this.privRecognizerConfig = recognizerConfig;
        this.privIsDisposed = false;
        this.privRecognizer = recognizer;
    }

    public get audioSource(): IAudioSource {
        return this.privAudioSource;
    }

    public isDisposed(): boolean {
        return this.privIsDisposed;
    }
    public dispose(reason?: string): void {
        this.privIsDisposed = true;
        if (this.privConnectionConfigurationPromise) {
            this.privConnectionConfigurationPromise.onSuccessContinueWith((connection: IConnection) => {
                connection.dispose(reason);
            });
        }
    }

    public recognize(
        speechContextJson: string,
        successCallback: (e: SpeechRecognitionResult) => void,
        errorCallBack: (e: string) => void,
    ): Promise<boolean> {
        const requestSession = new RequestSession(this.privAudioSource.id(), speechContextJson);

        requestSession.listenForServiceTelemetry(this.privAudioSource.events);

        return this.audioSource
            .attach(requestSession.audioNodeId)
            .continueWithPromise<boolean>((result: PromiseResult<IAudioStreamNode>) => {
                let audioNode: ReplayableAudioNode;

                if (result.isError) {
                    this.cancelRecognitionLocal(requestSession, CancellationReason.Error, CancellationErrorCode.ConnectionFailure, result.error);
                } else {
                    audioNode = new ReplayableAudioNode(result.result, this.audioSource.format as AudioStreamFormatImpl);
                    requestSession.onAudioSourceAttachCompleted(audioNode, false);
                }

                return this.configureConnection(requestSession)
                    .on((_: IConnection) => {

                        const sessionStartEventArgs: SessionEventArgs = new SessionEventArgs(requestSession.sessionId);

                        if (!!this.privRecognizer.sessionStarted) {
                            this.privRecognizer.sessionStarted(this.privRecognizer, sessionStartEventArgs);
                        }

                        const messageRetrievalPromise = this.receiveMessage(requestSession, successCallback, errorCallBack);
                        const audioSendPromise = this.sendAudio(audioNode, requestSession);

                        const completionPromise = PromiseHelper.whenAll([messageRetrievalPromise, audioSendPromise]);

                        return completionPromise.on((r: boolean) => {
                            requestSession.dispose();
                            this.sendTelemetryData(requestSession, requestSession.getTelemetry());
                        }, (error: string) => {
                            requestSession.dispose(error);
                            this.sendTelemetryData(requestSession, requestSession.getTelemetry());
                            this.cancelRecognitionLocal(requestSession, CancellationReason.Error, CancellationErrorCode.RuntimeError, error);
                        });

                    }, (error: string) => {
                        this.cancelRecognitionLocal(requestSession, CancellationReason.Error, CancellationErrorCode.ConnectionFailure, error);
                    }).on(() => {
                        return requestSession.completionPromise;
                    }, (error: string) => {
                        this.cancelRecognitionLocal(requestSession, CancellationReason.Error, CancellationErrorCode.RuntimeError, error);
                    }).onSuccessContinueWithPromise((_: IConnection): Promise<boolean> => {
                        return PromiseHelper.fromResult(true);
                    });
            });
    }

    // Called when telemetry data is sent to the service.
    // Used for testing Telemetry capture.
    public static telemetryData: (json: string) => void;
    public static telemetryDataEnabled: boolean = true;

    protected abstract processTypeSpecificMessages(
        connectionMessage: SpeechConnectionMessage,
        requestSession: RequestSession,
        connection: IConnection,
        successCallback?: (e: SpeechRecognitionResult) => void,
        errorCallBack?: (e: string) => void): void;

    protected sendTelemetryData = (requestSession: RequestSession, telemetryData: string) => {
        if (ServiceRecognizerBase.telemetryDataEnabled !== true) {
            return PromiseHelper.fromResult(true);
        }

        if (!!ServiceRecognizerBase.telemetryData) {
            try {
                ServiceRecognizerBase.telemetryData(telemetryData);
                /* tslint:disable:no-empty */
            } catch { }
        }

        return this.fetchConnection(requestSession).onSuccessContinueWithPromise((connection: IConnection): Promise<boolean> => {
            return connection.send(new SpeechConnectionMessage(
                MessageType.Text,
                "telemetry",
                requestSession.requestId,
                "application/json",
                telemetryData));
        });
    }

    // Cancels recognition.
    protected abstract cancelRecognition(
        sessionId: string,
        requestId: string,
        cancellationReason: CancellationReason,
        errorCode: CancellationErrorCode,
        error: string): void;

    // Cancels recognition.
    protected cancelRecognitionLocal(
        requestSession: RequestSession,
        cancellationReason: CancellationReason,
        errorCode: CancellationErrorCode,
        error: string): void {

        if (!requestSession.isCanceled) {
            requestSession.onCancelled();

            this.cancelRecognition(
                requestSession.sessionId,
                requestSession.requestId,
                cancellationReason,
                errorCode,
                error);
        }
    }

    private fetchConnection = (requestSession: RequestSession): Promise<IConnection> => {
        return this.configureConnection(requestSession);
    }

    private configureConnection = (requestSession: RequestSession, isUnAuthorized: boolean = false): Promise<IConnection> => {
        if (this.privConnectionConfigurationPromise) {
            if (this.privConnectionConfigurationPromise.result().isCompleted &&
                (this.privConnectionConfigurationPromise.result().isError
                    || this.privConnectionConfigurationPromise.result().result.state() === ConnectionState.Disconnected)) {

                this.privConnectionId = null;
                this.privConnectionConfigurationPromise = null;
                return this.configureConnection(requestSession);
            } else {
                // requestSession.onConnectionEstablishCompleted(200);
                return this.privConnectionConfigurationPromise;
            }
        }

        this.privAuthFetchEventId = createNoDashGuid();
        this.privConnectionId = createNoDashGuid();

        requestSession.onPreConnectionStart(this.privAuthFetchEventId, this.privConnectionId);

        const authPromise = isUnAuthorized ? this.privAuthentication.fetchOnExpiry(this.privAuthFetchEventId) : this.privAuthentication.fetch(this.privAuthFetchEventId);

        this.privConnectionConfigurationPromise = authPromise
            .continueWithPromise((result: PromiseResult<AuthInfo>) => {
                if (result.isError) {
                    requestSession.onAuthCompleted(true, result.error);
                    throw new Error(result.error);
                } else {
                    requestSession.onAuthCompleted(false);
                }

                const connection: IConnection = this.privConnectionFactory.create(this.privRecognizerConfig, result.result, this.privConnectionId);
                requestSession.listenForServiceTelemetry(connection.events);

                return connection.open().onSuccessContinueWithPromise((response: ConnectionOpenResponse): Promise<IConnection> => {
                    if (response.statusCode === 200) {
                        requestSession.onPreConnectionStart(this.privAuthFetchEventId, this.privConnectionId);
                        requestSession.onConnectionEstablishCompleted(response.statusCode);
                        //  requestSession.listenForServiceTelemetry(this.privConnectionFetchPromise.result().result.events);
                        return this.sendSpeechConfig(connection, requestSession, this.privRecognizerConfig.platformConfig.serialize())
                            .onSuccessContinueWithPromise((_: boolean) => {
                                return this.sendSpeechContext(connection, requestSession, requestSession.contextJson).onSuccessContinueWith((_: boolean) => {
                                    return connection;
                                });
                            });

                    } else if (response.statusCode === 403 && !isUnAuthorized) {
                        return this.configureConnection(requestSession, true);
                    } else {
                        requestSession.onConnectionEstablishCompleted(response.statusCode, response.reason);
                        return PromiseHelper.fromError<IConnection>(`Unable to contact server. StatusCode: ${response.statusCode}, ${this.privRecognizerConfig.parameters.getProperty(PropertyId.SpeechServiceConnection_Endpoint)} Reason: ${response.reason}`);
                    }
                });
            });

        return this.privConnectionConfigurationPromise;
    }

    private receiveMessage = (
        requestSession: RequestSession,
        successCallback: (e: SpeechRecognitionResult) => void,
        errorCallBack: (e: string) => void,
    ): Promise<boolean> => {
        return this.fetchConnection(requestSession).onSuccessContinueWithPromise((connection: IConnection): Promise<boolean> => {
            return connection.read()
                .onSuccessContinueWithPromise((message: ConnectionMessage) => {

                    // indicates we are draining the queue and it came with no message;
                    if (!message) {
                        if (requestSession.isCompleted) {
                            return PromiseHelper.fromResult(true);
                        } else {
                            return this.receiveMessage(requestSession, successCallback, errorCallBack);
                        }
                    }

                    const connectionMessage = SpeechConnectionMessage.fromConnectionMessage(message);

                    if (connectionMessage.requestId.toLowerCase() === requestSession.requestId.toLowerCase()) {
                        switch (connectionMessage.path.toLowerCase()) {
                            case "turn.start":
                                break;
                            case "speech.startdetected":
                                const speechStartDetected: SpeechDetected = SpeechDetected.fromJSON(connectionMessage.textBody);

                                const speechStartEventArgs = new RecognitionEventArgs(speechStartDetected.Offset, requestSession.sessionId);

                                if (!!this.privRecognizer.speechStartDetected) {
                                    this.privRecognizer.speechStartDetected(this.privRecognizer, speechStartEventArgs);
                                }

                                break;
                            case "speech.enddetected":

                                let json: string;

                                if (connectionMessage.textBody.length > 0) {
                                    json = connectionMessage.textBody;
                                } else {
                                    // If the request was empty, the JSON returned is empty.
                                    json = "{ Offset: 0 }";
                                }

                                const speechStopDetected: SpeechDetected = SpeechDetected.fromJSON(json);

                                requestSession.onServiceRecognized(speechStopDetected.Offset + requestSession.currentTurnAudioOffset);

                                const speechStopEventArgs = new RecognitionEventArgs(speechStopDetected.Offset + requestSession.currentTurnAudioOffset, requestSession.sessionId);

                                if (!!this.privRecognizer.speechEndDetected) {
                                    this.privRecognizer.speechEndDetected(this.privRecognizer, speechStopEventArgs);
                                }

                                if (requestSession.isSpeechEnded && this.privRecognizerConfig.isContinuousRecognition) {
                                    this.cancelRecognitionLocal(requestSession, CancellationReason.EndOfStream, CancellationErrorCode.NoError, undefined);
                                }
                                break;
                            case "turn.end":
                                const sessionStopEventArgs: SessionEventArgs = new SessionEventArgs(requestSession.sessionId);

                                requestSession.onServiceTurnEndResponse(this.privRecognizerConfig.isContinuousRecognition);

                                if (!this.privRecognizerConfig.isContinuousRecognition || requestSession.isSpeechEnded) {
                                    if (!!this.privRecognizer.sessionStopped) {
                                        this.privRecognizer.sessionStopped(this.privRecognizer, sessionStopEventArgs);
                                    }

                                    return PromiseHelper.fromResult(true);
                                } else {
                                    this.fetchConnection(requestSession).onSuccessContinueWith((connection: IConnection) => {
                                        this.sendSpeechContext(connection, requestSession, requestSession.contextJson);
                                    });
                                }
                            default:
                                this.processTypeSpecificMessages(
                                    connectionMessage,
                                    requestSession,
                                    connection,
                                    successCallback,
                                    errorCallBack);
                        }
                    }

                    return this.receiveMessage(requestSession, successCallback, errorCallBack);
                });
        });
    }

    private sendSpeechConfig = (connection: IConnection, requestSession: RequestSession, speechConfigJson: string): Promise<boolean> => {
        // filter out anything that is not required for the service to work.
        if (ServiceRecognizerBase.telemetryDataEnabled !== true) {
            const withTelemetry = JSON.parse(speechConfigJson);

            const replacement: any = {
                 context: {
                    system: withTelemetry.context.system,
                 }};

            speechConfigJson = JSON.stringify(replacement);
        }

        if (speechConfigJson && this.privConnectionId !== this.privSpeechConfigConnectionId) {
            this.privSpeechConfigConnectionId = this.privConnectionId;
            return connection.send(new SpeechConnectionMessage(
                MessageType.Text,
                "speech.config",
                requestSession.requestId,
                "application/json",
                speechConfigJson));
        }

        return PromiseHelper.fromResult(true);
    }

    private sendSpeechContext = (connection: IConnection, requestSession: RequestSession, speechContextJson: string): Promise<boolean> => {
        if (speechContextJson) {
            return connection.send(new SpeechConnectionMessage(
                MessageType.Text,
                "speech.context",
                requestSession.requestId,
                "application/json",
                speechContextJson));
        }
        return PromiseHelper.fromResult(true);
    }

    private sendAudio = (
        audioStreamNode: IAudioStreamNode,
        requestSession: RequestSession): Promise<boolean> => {
        // NOTE: Home-baked promises crash ios safari during the invocation
        // of the error callback chain (looks like the recursion is way too deep, and
        // it blows up the stack). The following construct is a stop-gap that does not
        // bubble the error up the callback chain and hence circumvents this problem.
        // TODO: rewrite with ES6 promises.
        const deferred = new Deferred<boolean>();

        // The time we last sent data to the service.
        let lastSendTime: number = 0;

        const audioFormat: AudioStreamFormatImpl = this.privAudioSource.format as AudioStreamFormatImpl;

        const readAndUploadCycle = (_: boolean) => {

            // If speech is done, stop sending audio.
            if (!this.privIsDisposed && !requestSession.isSpeechEnded && !requestSession.isCompleted) {
                this.fetchConnection(requestSession).onSuccessContinueWith((connection: IConnection) => {
                    audioStreamNode.read().on(
                        (audioStreamChunk: IStreamChunk<ArrayBuffer>) => {

                            // we have a new audio chunk to upload.
                            if (requestSession.isSpeechEnded) {
                                // If service already recognized audio end then dont send any more audio
                                deferred.resolve(true);
                                return;
                            }

                            const payload = (audioStreamChunk.isEnd) ? null : audioStreamChunk.buffer;
                            const uploaded: Promise<boolean> = connection.send(
                                new SpeechConnectionMessage(
                                    MessageType.Binary, "audio", requestSession.requestId, null, payload));

                            if (!audioStreamChunk.isEnd) {

                                // Caculate any delay to the audio stream needed. /2 to allow 2x real time transmit rate max.
                                const minSendTime = ((payload.byteLength / audioFormat.avgBytesPerSec) / 2) * 1000;

                                const delay: number = Math.max(0, (lastSendTime - Date.now() + minSendTime));

                                uploaded.onSuccessContinueWith((result: boolean) => {
                                    setTimeout(() => {
                                        lastSendTime = Date.now();
                                        readAndUploadCycle(result);
                                    }, delay);
                                });
                            } else {
                                // the audio stream has been closed, no need to schedule next
                                // read-upload cycle.
                                requestSession.onSpeechEnded();
                                deferred.resolve(true);
                            }
                        },
                        (error: string) => {
                            if (requestSession.isSpeechEnded) {
                                // For whatever reason, Reject is used to remove queue subscribers inside
                                // the Queue.DrainAndDispose invoked from DetachAudioNode down below, which
                                // means that sometimes things can be rejected in normal circumstances, without
                                // any errors.
                                deferred.resolve(true); // TODO: remove the argument, it's is completely meaningless.
                            } else {
                                // Only reject, if there was a proper error.
                                deferred.reject(error);
                            }
                        });
                });
            }
        };

        readAndUploadCycle(true);

        return deferred.promise();
    }
}
