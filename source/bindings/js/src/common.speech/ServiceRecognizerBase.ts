// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import {
    ArgumentNullError,
    ConnectionMessage,
    ConnectionOpenResponse,
    ConnectionState,
    CreateNoDashGuid,
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
    private authentication: IAuthentication;
    private connectionFactory: IConnectionFactory;
    private connection: IConnection;
    private audioSource: IAudioSource;
    private speechConfigConnectionId: string;
    private connectionFetchPromise: Promise<IConnection>;
    private connectionId: string;
    private authFetchEventId: string;
    private isDisposed: boolean;
    private recognizer: Recognizer;

    protected recognizerConfig: RecognizerConfig;

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

        this.authentication = authentication;
        this.connectionFactory = connectionFactory;
        this.audioSource = audioSource;
        this.recognizerConfig = recognizerConfig;
        this.isDisposed = false;
        this.recognizer = recognizer;
    }

    public get AudioSource(): IAudioSource {
        return this.audioSource;
    }

    public IsDisposed(): boolean {
        return this.isDisposed;
    }
    public Dispose(reason?: string): void {
        this.isDisposed = true;
        if (this.connection) {
            this.connection.Dispose(reason);
            this.connection = undefined;
        }
    }

    public Recognize(
        speechContextJson: string,
        successCallback: (e: SpeechRecognitionResult) => void,
        errorCallBack: (e: string) => void,
    ): Promise<boolean> {

        const requestSession = new RequestSession(this.audioSource.Id());

        requestSession.ListenForServiceTelemetry(this.audioSource.Events);

        return this.audioSource
            .Attach(requestSession.AudioNodeId)
            .ContinueWithPromise<boolean>((result: PromiseResult<IAudioStreamNode>) => {
                if (result.IsError) {
                    this.ErrorCallback(result.Error);
                    throw new Error(result.Error);
                } else {
                    requestSession.OnAudioSourceAttachCompleted(result.Result, false);
                }

                const audioNode = result.Result;

                return this.FetchConnection(requestSession)
                    .On((connection: IConnection) => {
                        const messageRetrievalPromise = this.ReceiveMessage(connection, requestSession, successCallback, errorCallBack);
                        const messageSendPromise = this.SendSpeechConfig(requestSession.RequestId, connection, this.recognizerConfig.PlatformConfig.Serialize())
                            .OnSuccessContinueWithPromise((_: boolean) => {
                                return this.SendSpeechContext(requestSession.RequestId, connection, speechContextJson)
                                    .OnSuccessContinueWithPromise((_: boolean) => {
                                        return this.SendAudio(requestSession.RequestId, connection, audioNode, requestSession);
                                    });
                            });

                        const completionPromise = PromiseHelper.WhenAll([messageRetrievalPromise, messageSendPromise]);

                        completionPromise.On((r: boolean) => {
                            requestSession.Dispose();
                            this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                        }, (error: string) => {
                            requestSession.Dispose(error);
                            this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                        });

                        return completionPromise;
                    }, (error: string) => {
                        this.ConnectionError(requestSession.SessionId, requestSession.RequestId, error);
                    }).OnSuccessContinueWithPromise(() => {
                        return requestSession.CompletionPromise;
                    });
            });
    }

    // Called when telemetry data is sent to the service.
    // Used for testing Telemetry capture.
    public static TelemetryData: (json: string) => void;

    protected abstract ConnectionError(sessionId: string, requestId: string, error: string): void;

    protected abstract ProcessTypeSpecificMessages(
        connectionMessage: SpeechConnectionMessage,
        requestSession: RequestSession,
        connection: IConnection,
        successCallback?: (e: SpeechRecognitionResult) => void,
        errorCallBack?: (e: string) => void): void;

    protected SendTelemetryData = (requestId: string, connection: IConnection, telemetryData: string) => {
        if (!!ServiceRecognizerBase.TelemetryData) {
            try {
                ServiceRecognizerBase.TelemetryData(telemetryData);
                /* tslint:disable:no-empty */
            } catch { }
        }

        return connection
            .Send(new SpeechConnectionMessage(
                MessageType.Text,
                "telemetry",
                requestId,
                "application/json",
                telemetryData));
    }

    private ErrorCallback(error: string): void {
        throw new Error(error);
    }

    private FetchConnection = (requestSession: RequestSession, isUnAuthorized: boolean = false): Promise<IConnection> => {
        if (this.connectionFetchPromise) {
            if (this.connectionFetchPromise.Result().IsError
                || this.connectionFetchPromise.Result().Result.State() === ConnectionState.Disconnected) {
                this.connectionId = null;
                this.connectionFetchPromise = null;
                return this.FetchConnection(requestSession);
            } else {
                requestSession.OnPreConnectionStart(this.authFetchEventId, this.connectionId);
                requestSession.OnConnectionEstablishCompleted(200);
                requestSession.ListenForServiceTelemetry(this.connectionFetchPromise.Result().Result.Events);
                return this.connectionFetchPromise;
            }
        }

        this.authFetchEventId = CreateNoDashGuid();
        this.connectionId = CreateNoDashGuid();

        requestSession.OnPreConnectionStart(this.authFetchEventId, this.connectionId);

        const authPromise = isUnAuthorized ? this.authentication.FetchOnExpiry(this.authFetchEventId) : this.authentication.Fetch(this.authFetchEventId);

        this.connectionFetchPromise = authPromise
            .ContinueWithPromise((result: PromiseResult<AuthInfo>) => {
                if (result.IsError) {
                    requestSession.OnAuthCompleted(true, result.Error);
                    throw new Error(result.Error);
                } else {
                    requestSession.OnAuthCompleted(false);
                }

                if (this.connection) {
                    this.connection.Dispose();
                    this.connection = undefined;
                }

                this.connection = this.connectionFactory.Create(this.recognizerConfig, result.Result, this.connectionId);
                requestSession.ListenForServiceTelemetry(this.connection.Events);

                return this.connection.Open().OnSuccessContinueWithPromise((response: ConnectionOpenResponse) => {
                    if (response.StatusCode === 200) {
                        requestSession.OnConnectionEstablishCompleted(response.StatusCode);
                        return PromiseHelper.FromResult(this.connection);
                    } else if (response.StatusCode === 403 && !isUnAuthorized) {
                        return this.FetchConnection(requestSession, true);
                    } else {
                        requestSession.OnConnectionEstablishCompleted(response.StatusCode, response.Reason);
                        return PromiseHelper.FromError<IConnection>(`Unable to contact server. StatusCode: ${response.StatusCode}, Reason: ${response.Reason}`);
                    }
                });
            });

        return this.connectionFetchPromise;
    }

    private ReceiveMessage = (
        connection: IConnection,
        requestSession: RequestSession,
        successCallback: (e: SpeechRecognitionResult) => void,
        errorCallBack: (e: string) => void,
    ): Promise<boolean> => {
        return connection
            .Read()
            .OnSuccessContinueWithPromise((message: ConnectionMessage) => {
                // indicates we are draining the queue and it came with no message;
                if (!message) {
                    return PromiseHelper.FromResult(true);
                }

                const connectionMessage = SpeechConnectionMessage.FromConnectionMessage(message);

                if (connectionMessage.RequestId.toLowerCase() === requestSession.RequestId.toLowerCase()) {
                    switch (connectionMessage.Path.toLowerCase()) {
                        case "turn.start":
                            const sessionStartEventArgs: SessionEventArgs = new SessionEventArgs(requestSession.SessionId);

                            if (!!this.recognizer.sessionStarted) {
                                this.recognizer.sessionStarted(this.recognizer, sessionStartEventArgs);
                            }
                            break;
                        case "speech.startdetected":
                            const speechStartDetected: SpeechDetected = SpeechDetected.FromJSON(connectionMessage.TextBody);

                            const speechStartEventArgs = new RecognitionEventArgs(speechStartDetected.Offset, requestSession.SessionId);

                            if (!!this.recognizer.speechStartDetected) {
                                this.recognizer.speechStartDetected(this.recognizer, speechStartEventArgs);
                            }

                            break;
                        case "speech.enddetected":

                            let json: string;

                            if (connectionMessage.TextBody.length > 0) {
                                json = connectionMessage.TextBody;
                            } else {
                                // If the request was empty, the JSON returned is empty.
                                json = "{ Offset: 0 }";
                            }

                            const speechStopDetected: SpeechDetected = SpeechDetected.FromJSON(json);

                            const speechStopEventArgs = new RecognitionEventArgs(speechStopDetected.Offset, requestSession.SessionId);

                            if (!!this.recognizer.speechEndDetected) {
                                this.recognizer.speechEndDetected(this.recognizer, speechStopEventArgs);
                            }

                            break;
                        case "turn.end":
                            const sessionStopEventArgs: SessionEventArgs = new SessionEventArgs(requestSession.SessionId);

                            if (!!this.recognizer.sessionStopped) {
                                this.recognizer.sessionStopped(this.recognizer, sessionStopEventArgs);
                            }
                            requestSession.OnServiceTurnEndResponse();
                            return PromiseHelper.FromResult(true);
                        default:
                            this.ProcessTypeSpecificMessages(
                                connectionMessage,
                                requestSession,
                                connection,
                                successCallback,
                                errorCallBack);
                    }
                }

                return this.ReceiveMessage(connection, requestSession, successCallback, errorCallBack);
            });
    }

    private SendSpeechConfig = (requestId: string, connection: IConnection, speechConfigJson: string) => {
        if (speechConfigJson && this.connectionId !== this.speechConfigConnectionId) {
            this.speechConfigConnectionId = this.connectionId;
            return connection
                .Send(new SpeechConnectionMessage(
                    MessageType.Text,
                    "speech.config",
                    requestId,
                    "application/json",
                    speechConfigJson));
        }

        return PromiseHelper.FromResult(true);
    }

    private SendSpeechContext = (requestId: string, connection: IConnection, speechContextJson: string) => {
        if (speechContextJson) {
            return connection
                .Send(new SpeechConnectionMessage(
                    MessageType.Text,
                    "speech.context",
                    requestId,
                    "application/json",
                    speechContextJson));
        }
        return PromiseHelper.FromResult(true);
    }

    private SendAudio = (
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

        const audioFormat: AudioStreamFormatImpl = this.audioSource.Format as AudioStreamFormatImpl;

        const readAndUploadCycle = (_: boolean) => {
            // If speech is done, stop sending audio.
            if (!this.isDisposed && !requestSession.IsSpeechEnded && !requestSession.IsCompleted) {
                audioStreamNode.Read().On(
                    (audioStreamChunk: IStreamChunk<ArrayBuffer>) => {
                        // we have a new audio chunk to upload.
                        if (requestSession.IsSpeechEnded) {
                            // If service already recognized audio end then dont send any more audio
                            deferred.Resolve(true);
                            return;
                        }

                        const payload = (audioStreamChunk.IsEnd) ? null : audioStreamChunk.Buffer;
                        const uploaded = connection.Send(
                            new SpeechConnectionMessage(
                                MessageType.Binary, "audio", requestId, null, payload));

                        if (!audioStreamChunk.IsEnd) {

                            // Caculate any delay to the audio stream needed. /2 to allow 2x real time transmit rate max.
                            const minSendTime = ((payload.byteLength / audioFormat.avgBytesPerSec) / 2) * 1000;

                            const delay: number = Math.max(0, (lastSendTime - Date.now() + minSendTime));

                            uploaded.OnSuccessContinueWith((result: boolean) => {
                                setTimeout(() => {
                                    lastSendTime = Date.now();
                                    readAndUploadCycle(result);
                                }, delay);
                            });
                        } else {
                            // the audio stream has been closed, no need to schedule next
                            // read-upload cycle.
                            deferred.Resolve(true);
                        }
                    },
                    (error: string) => {
                        if (requestSession.IsSpeechEnded) {
                            // For whatever reason, Reject is used to remove queue subscribers inside
                            // the Queue.DrainAndDispose invoked from DetachAudioNode down blow, which
                            // means that sometimes things can be rejected in normal circumstances, without
                            // any errors.
                            deferred.Resolve(true); // TODO: remove the argument, it's is completely meaningless.
                        } else {
                            // Only reject, if there was a proper error.
                            deferred.Reject(error);
                        }
                    });
            }
        };

        readAndUploadCycle(true);

        return deferred.Promise();
    }
}
