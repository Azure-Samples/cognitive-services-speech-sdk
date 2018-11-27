// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import {
    ArgumentNullError,
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
import { RecognitionEventArgs, Recognizer, SessionEventArgs, SpeechRecognitionResult } from "../sdk/Exports";
import { RequestSession, SpeechDetected } from "./Exports";
import { AuthInfo, IAuthentication } from "./IAuthentication";
import { IConnectionFactory } from "./IConnectionFactory";
import { RecognizerConfig } from "./RecognizerConfig";
import { SpeechConnectionMessage } from "./SpeechConnectionMessage.Internal";

export abstract class ServiceRecognizerBase implements IDisposable {
    private privAuthentication: IAuthentication;
    private privConnectionFactory: IConnectionFactory;
    private privConnection: IConnection;
    private privAudioSource: IAudioSource;
    private privSpeechConfigConnectionId: string;
    private privConnectionFetchPromise: Promise<IConnection>;
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
        if (this.privConnection) {
            this.privConnection.dispose(reason);
            this.privConnection = undefined;
        }
    }

    public recognize(
        speechContextJson: string,
        successCallback: (e: SpeechRecognitionResult) => void,
        errorCallBack: (e: string) => void,
    ): Promise<boolean> {
        const requestSession = new RequestSession(this.privAudioSource.id());

        requestSession.listenForServiceTelemetry(this.privAudioSource.events);

        return this.privAudioSource
            .attach(requestSession.audioNodeId)
            .continueWithPromise<boolean>((result: PromiseResult<IAudioStreamNode>) => {
                if (result.isError) {
                    this.errorCallback(result.error);
                    throw new Error(result.error);
                } else {
                    requestSession.onAudioSourceAttachCompleted(result.result, false);
                }

                const audioNode = result.result;

                return this.fetchConnection(requestSession)
                    .on((connection: IConnection) => {
                        const messageRetrievalPromise = this.receiveMessage(connection, requestSession, successCallback, errorCallBack);
                        const messageSendPromise = this.sendSpeechConfig(requestSession.requestId, connection, this.privRecognizerConfig.platformConfig.serialize())
                            .onSuccessContinueWithPromise((_: boolean) => {
                                return this.sendSpeechContext(requestSession.requestId, connection, speechContextJson)
                                    .onSuccessContinueWithPromise((_: boolean) => {
                                        return this.sendAudio(requestSession.requestId, connection, audioNode, requestSession);
                                    });
                            });

                        const completionPromise = PromiseHelper.whenAll([messageRetrievalPromise, messageSendPromise]);

                        completionPromise.on((r: boolean) => {
                            requestSession.dispose();
                            this.sendTelemetryData(requestSession.requestId, connection, requestSession.getTelemetry());
                        }, (error: string) => {
                            requestSession.dispose(error);
                            this.sendTelemetryData(requestSession.requestId, connection, requestSession.getTelemetry());
                        });

                        return completionPromise;
                    }, (error: string) => {
                        this.connectionError(requestSession.sessionId, requestSession.requestId, error);
                    }).onSuccessContinueWithPromise(() => {
                        return requestSession.completionPromise;
                    });
            });
    }

    // Called when telemetry data is sent to the service.
    // Used for testing Telemetry capture.
    public static telemetryData: (json: string) => void;

    protected abstract connectionError(sessionId: string, requestId: string, error: string): void;

    protected abstract processTypeSpecificMessages(
        connectionMessage: SpeechConnectionMessage,
        requestSession: RequestSession,
        connection: IConnection,
        successCallback?: (e: SpeechRecognitionResult) => void,
        errorCallBack?: (e: string) => void): void;

    protected sendTelemetryData = (requestId: string, connection: IConnection, telemetryData: string) => {
        if (!!ServiceRecognizerBase.telemetryData) {
            try {
                ServiceRecognizerBase.telemetryData(telemetryData);
                /* tslint:disable:no-empty */
            } catch { }
        }

        return connection
            .send(new SpeechConnectionMessage(
                MessageType.Text,
                "telemetry",
                requestId,
                "application/json",
                telemetryData));
    }

    private errorCallback(error: string): void {
        throw new Error(error);
    }

    private fetchConnection = (requestSession: RequestSession, isUnAuthorized: boolean = false): Promise<IConnection> => {
        if (this.privConnectionFetchPromise) {
            if (this.privConnectionFetchPromise.result().isError
                || this.privConnectionFetchPromise.result().result.state() === ConnectionState.Disconnected) {
                this.privConnectionId = null;
                this.privConnectionFetchPromise = null;
                return this.fetchConnection(requestSession);
            } else {
                requestSession.onPreConnectionStart(this.privAuthFetchEventId, this.privConnectionId);
                requestSession.onConnectionEstablishCompleted(200);
                requestSession.listenForServiceTelemetry(this.privConnectionFetchPromise.result().result.events);
                return this.privConnectionFetchPromise;
            }
        }

        this.privAuthFetchEventId = createNoDashGuid();
        this.privConnectionId = createNoDashGuid();

        requestSession.onPreConnectionStart(this.privAuthFetchEventId, this.privConnectionId);

        const authPromise = isUnAuthorized ? this.privAuthentication.fetchOnExpiry(this.privAuthFetchEventId) : this.privAuthentication.fetch(this.privAuthFetchEventId);

        this.privConnectionFetchPromise = authPromise
            .continueWithPromise((result: PromiseResult<AuthInfo>) => {
                if (result.isError) {
                    requestSession.onAuthCompleted(true, result.error);
                    throw new Error(result.error);
                } else {
                    requestSession.onAuthCompleted(false);
                }

                if (this.privConnection) {
                    this.privConnection.dispose();
                    this.privConnection = undefined;
                }

                this.privConnection = this.privConnectionFactory.create(this.privRecognizerConfig, result.result, this.privConnectionId);
                requestSession.listenForServiceTelemetry(this.privConnection.events);

                return this.privConnection.open().onSuccessContinueWithPromise((response: ConnectionOpenResponse) => {
                    if (response.statusCode === 200) {
                        requestSession.onConnectionEstablishCompleted(response.statusCode);
                        return PromiseHelper.fromResult(this.privConnection);
                    } else if (response.statusCode === 403 && !isUnAuthorized) {
                        return this.fetchConnection(requestSession, true);
                    } else {
                        requestSession.onConnectionEstablishCompleted(response.statusCode, response.reason);
                        return PromiseHelper.fromError<IConnection>(`Unable to contact server. StatusCode: ${response.statusCode}, Reason: ${response.reason}`);
                    }
                });
            });

        return this.privConnectionFetchPromise;
    }

    private receiveMessage = (
        connection: IConnection,
        requestSession: RequestSession,
        successCallback: (e: SpeechRecognitionResult) => void,
        errorCallBack: (e: string) => void,
    ): Promise<boolean> => {
        return connection
            .read()
            .onSuccessContinueWithPromise((message: ConnectionMessage) => {
                // indicates we are draining the queue and it came with no message;
                if (!message) {
                    return PromiseHelper.fromResult(true);
                }

                const connectionMessage = SpeechConnectionMessage.fromConnectionMessage(message);

                if (connectionMessage.requestId.toLowerCase() === requestSession.requestId.toLowerCase()) {
                    switch (connectionMessage.path.toLowerCase()) {
                        case "turn.start":
                            const sessionStartEventArgs: SessionEventArgs = new SessionEventArgs(requestSession.sessionId);

                            if (!!this.privRecognizer.sessionStarted) {
                                this.privRecognizer.sessionStarted(this.privRecognizer, sessionStartEventArgs);
                            }
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

                            const speechStopEventArgs = new RecognitionEventArgs(speechStopDetected.Offset, requestSession.sessionId);

                            if (!!this.privRecognizer.speechEndDetected) {
                                this.privRecognizer.speechEndDetected(this.privRecognizer, speechStopEventArgs);
                            }

                            break;
                        case "turn.end":
                            const sessionStopEventArgs: SessionEventArgs = new SessionEventArgs(requestSession.sessionId);

                            if (!!this.privRecognizer.sessionStopped) {
                                this.privRecognizer.sessionStopped(this.privRecognizer, sessionStopEventArgs);
                            }
                            requestSession.onServiceTurnEndResponse();
                            return PromiseHelper.fromResult(true);
                        default:
                            this.processTypeSpecificMessages(
                                connectionMessage,
                                requestSession,
                                connection,
                                successCallback,
                                errorCallBack);
                    }
                }

                return this.receiveMessage(connection, requestSession, successCallback, errorCallBack);
            });
    }

    private sendSpeechConfig = (requestId: string, connection: IConnection, speechConfigJson: string) => {
        if (speechConfigJson && this.privConnectionId !== this.privSpeechConfigConnectionId) {
            this.privSpeechConfigConnectionId = this.privConnectionId;
            return connection
                .send(new SpeechConnectionMessage(
                    MessageType.Text,
                    "speech.config",
                    requestId,
                    "application/json",
                    speechConfigJson));
        }

        return PromiseHelper.fromResult(true);
    }

    private sendSpeechContext = (requestId: string, connection: IConnection, speechContextJson: string) => {
        if (speechContextJson) {
            return connection
                .send(new SpeechConnectionMessage(
                    MessageType.Text,
                    "speech.context",
                    requestId,
                    "application/json",
                    speechContextJson));
        }
        return PromiseHelper.fromResult(true);
    }

    private sendAudio = (
        requestId: string,
        connection: IConnection,
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
                audioStreamNode.read().on(
                    (audioStreamChunk: IStreamChunk<ArrayBuffer>) => {
                        // we have a new audio chunk to upload.
                        if (requestSession.isSpeechEnded) {
                            // If service already recognized audio end then dont send any more audio
                            deferred.resolve(true);
                            return;
                        }

                        const payload = (audioStreamChunk.isEnd) ? null : audioStreamChunk.buffer;
                        const uploaded = connection.send(
                            new SpeechConnectionMessage(
                                MessageType.Binary, "audio", requestId, null, payload));

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
                            deferred.resolve(true);
                        }
                    },
                    (error: string) => {
                        if (requestSession.isSpeechEnded) {
                            // For whatever reason, Reject is used to remove queue subscribers inside
                            // the Queue.DrainAndDispose invoked from DetachAudioNode down blow, which
                            // means that sometimes things can be rejected in normal circumstances, without
                            // any errors.
                            deferred.resolve(true); // TODO: remove the argument, it's is completely meaningless.
                        } else {
                            // Only reject, if there was a proper error.
                            deferred.reject(error);
                        }
                    });
            }
        };

        readAndUploadCycle(true);

        return deferred.promise();
    }
}
