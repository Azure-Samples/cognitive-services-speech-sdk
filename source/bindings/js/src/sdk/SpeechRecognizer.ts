//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import {
    IAuthentication,
    IConnectionFactory,
    IDetailedSpeechPhrase,
    ISimpleSpeechPhrase,
    ISpeechFragment,
    OutputFormatPropertyName,
    PlatformConfig,
    RecognitionCompletionStatus,
    RecognitionEndedEvent,
    RecognitionMode,
    RecognitionStatus2,
    RecognizerConfig,
    ServiceRecognizerBase,
    SpeechRecognitionEvent,
    SpeechRecognitionResultEvent,
    SpeechServiceRecognizer,
} from "../common.speech/Exports";
import { SpeechConnectionFactory } from "../common.speech/SpeechConnectionFactory";
import { AudioConfigImpl } from "./Audio/AudioConfig";
import { Contracts } from "./Contracts";
import {
    AudioConfig,
    CancellationReason,
    KeywordRecognitionModel,
    OutputFormat,
    PropertyCollection,
    PropertyId,
    Recognizer,
    ResultReason,
    SpeechRecognitionCanceledEventArgs,
    SpeechRecognitionResult,
    SpeechRecognitionResultEventArgs,
} from "./Exports";
import { SpeechConfig, SpeechConfigImpl } from "./SpeechConfig";

/**
 * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
 * @class
 */
export class SpeechRecognizer extends Recognizer {
    private disposedSpeechRecognizer: boolean = false;
    private privProperties: PropertyCollection;

    /**
     * SpeechRecognizer constructor.
     * @constructor
     * @param {SpeechConfig} speechConfig - An set of initial properties for this recognizer
     * @param {AudioConfig} audioConfig - An optional audio configuration associated with the recognizer
     */
    public constructor(speechConfig: SpeechConfig, audioConfig?: AudioConfig) {
        super(audioConfig);

        const speechConfigImpl: SpeechConfigImpl = speechConfig as SpeechConfigImpl;
        Contracts.throwIfNull(speechConfigImpl, "speechConfig");
        this.privProperties = speechConfigImpl.properties.clone();

        Contracts.throwIfNullOrWhitespace(speechConfigImpl.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage), PropertyId[PropertyId.SpeechServiceConnection_RecoLanguage]);

    }

    /**
     * The event recognizing signals that an intermediate recognition result is received.
     * @property
     */
    public recognizing: (sender: Recognizer, event: SpeechRecognitionResultEventArgs) => void;

    /**
     * The event recognized signals that a final recognition result is received.
     * @property
     */
    public recognized: (sender: Recognizer, event: SpeechRecognitionResultEventArgs) => void;

    /**
     * The event canceled signals that an error occurred during recognition.
     * @property
     */
    public canceled: (sender: Recognizer, event: SpeechRecognitionCanceledEventArgs) => void;

    /**
     * Gets the endpoint id of a customized speech model that is used for speech recognition.
     * @property
     * @returns the endpoint id of a customized speech model that is used for speech recognition.
     */
    public get endpointId(): string {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_EndpointId, "00000000-0000-0000-0000-000000000000");
    }

    /**
     * Sets the endpoint id of a customized speech model that is used for speech recognition.
     * @property
     * @param value The endpoint id of a customized speech model that is used for speech recognition.
     */
    public set endpointId(value: string) {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
        Contracts.throwIfNullOrWhitespace(value, "value");
        this.properties.setProperty(PropertyId.SpeechServiceConnection_EndpointId, value);
    }

    /**
     * Sets the authorization token used to communicate with the service.
     * @param token Authorization token.
     */
    public set authorizationToken(token: string) {
        Contracts.throwIfNullOrWhitespace(token, "token");
        this.properties.setProperty(PropertyId.SpeechServiceAuthorization_Token, token);
    }

    /**
     * Gets the authorization token used to communicate with the service.
     * @return Authorization token.
     */
    public get authorizationToken(): string {
        return this.properties.getProperty(PropertyId.SpeechServiceAuthorization_Token);
    }

    /**
     * Gets the spoken language of recognition.
     * @property
     * @returns The spoken language of recognition.
     */
    public get speechRecognitionLanguage(): string {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        return this.properties.getProperty(PropertyId.SpeechServiceConnection_RecoLanguage);
    }

    /**
     * Gets the output format of recognition.
     * @property
     * @returns The output format of recognition.
     */
    public get outputFormat(): OutputFormat {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        if (this.properties.getProperty(OutputFormatPropertyName, OutputFormat[OutputFormat.Simple]) === OutputFormat[OutputFormat.Simple]) {
            return OutputFormat.Simple;
        } else {
            return OutputFormat.Detailed;
        }
    }

    /**
     * The collection of properties and their values defined for this SpeechRecognizer.
     * @property
     * @returns The collection of properties and their values defined for this SpeechRecognizer.
     */
    public get properties(): PropertyCollection {
        return this.privProperties;
    }

    /**
     * Starts speech recognition, and stops after the first utterance is recognized. The task returns the recognition text as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition
     *       like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @member
     * @param cb - Callback that received the SpeechRecognitionResult.
     * @param err - Callback invoked in case of an error.
     */
    public recognizeOnceAsync(cb?: (e: SpeechRecognitionResult) => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();

        this.reco = this.implRecognizerSetup(
            RecognitionMode.Interactive,
            this.properties,
            this.audioConfig,
            new SpeechConnectionFactory());

        this.implRecognizerStart(this.reco, (event: SpeechRecognitionEvent) => {
            if (this.disposedSpeechRecognizer || !this.reco) {
                return;
            }

            this.implDispatchMessageHandler(event, cb, err);
        });
    }

    /**
     * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @member
     * @param cb - Callback that received the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();

        this.reco = this.implRecognizerSetup(
            RecognitionMode.Conversation,
            this.properties,
            this.audioConfig,
            new SpeechConnectionFactory());

        this.implRecognizerStart(this.reco, (event: SpeechRecognitionEvent) => {
            if (this.disposedSpeechRecognizer || !this.reco) {
                return;
            }

            // report result to promise.
            if (!!cb) {
                try {
                    cb();
                } catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
                cb = undefined;
            }

            this.implDispatchMessageHandler(event, undefined, err);
        });
    }

    /**
     * Stops continuous speech recognition.
     * @member
     * @param cb - Callback that received the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();

        if (!!cb) {
            try {
                cb();
            } catch (e) {
                if (!!err) {
                    err(e);
                }
            }
        }
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @member
     * @param {KeywordRecognitionModel} model - The keyword recognition model that specifies the keyword to be recognized.
     * @param cb - Callback that received the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    public startKeywordRecognitionAsync(model: KeywordRecognitionModel, cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
        Contracts.throwIfNull(model, "model");

        this.implCloseExistingRecognizer();
        throw new Error("keyword recognition not supported");
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @member
     * @param cb - Callback that received the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    public stopKeywordRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();
        throw new Error("keyword recognition not supported");
    }

    /**
     * closes all external resources held by an instance of this class.
     * @member
     */
    public close(): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.dispose(true);
    }

    /**
     * Disposes any resources held by the object.
     * @member
     * @param disposing - true if disposing the object.
     */
    protected dispose(disposing: boolean): void {
        if (this.disposedSpeechRecognizer) {
            return;
        }

        if (disposing) {
            this.implCloseExistingRecognizer();
            this.disposedSpeechRecognizer = true;
        }

        super.dispose(disposing);
    }

    protected CreateRecognizerConfig(speechConfig: PlatformConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        return new RecognizerConfig(
            speechConfig,
            recognitionMode,
            this.properties);
    }

    protected CreateServiceRecognizer(authentication: IAuthentication, connectionFactory: IConnectionFactory, audioConfig: AudioConfig, recognizerConfig: RecognizerConfig): ServiceRecognizerBase {
        const configImpl: AudioConfigImpl = audioConfig as AudioConfigImpl;
        return new SpeechServiceRecognizer(authentication, connectionFactory, configImpl, recognizerConfig);
    }

    // tslint:disable-next-line:member-ordering
    private reco: ServiceRecognizerBase;

    private implCloseExistingRecognizer(): void {
        if (this.reco) {
            this.reco.AudioSource.TurnOff();
            this.reco.Dispose();
            this.reco = undefined;
        }
    }

    private implDispatchMessageHandler(event: SpeechRecognitionEvent, cb?: (e: SpeechRecognitionResult) => void, err?: (e: string) => void): void {
        /*
         Alternative syntax for typescript devs.
         if (event instanceof SDK.RecognitionTriggeredEvent)
        */
        switch (event.Name) {
            case "RecognitionEndedEvent":
                {
                    const recoEndedEvent: RecognitionEndedEvent = event as RecognitionEndedEvent;
                    if (recoEndedEvent.Status !== RecognitionCompletionStatus.Success) {
                        const errorEvent: SpeechRecognitionCanceledEventArgs = new SpeechRecognitionCanceledEventArgs();

                        errorEvent.reason = CancellationReason.Error;
                        errorEvent.sessionId = recoEndedEvent.SessionId;
                        errorEvent.errorDetails = recoEndedEvent.Error;

                        if (this.canceled) {
                            this.canceled(this, errorEvent); // call error handler, if configured
                        }

                        if (!!err) {
                            err(recoEndedEvent.Error); // call error handler, if configured
                        }
                    }
                }
                break;

            case "SpeechSimplePhraseEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<ISimpleSpeechPhrase>;

                    const reason = this.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    if (reason === ResultReason.Canceled) {
                        const ev = new SpeechRecognitionCanceledEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.reason = this.implTranslateCancelResult(evResult.Result.RecognitionStatus);

                        if (!!this.canceled) {
                            this.canceled(this, ev);
                        }

                        // report result to promise.
                        if (!!err) {
                            err(evResult.Result.DisplayText);
                            err = undefined;
                        }
                    } else {
                        const ev = new SpeechRecognitionResultEventArgs();
                        ev.sessionId = evResult.SessionId;

                        ev.result = new SpeechRecognitionResult();
                        ev.result.json = JSON.stringify(evResult.Result);
                        ev.result.offset = evResult.Result.Offset;
                        ev.result.duration = evResult.Result.Duration;
                        ev.result.text = evResult.Result.DisplayText;
                        ev.result.reason = reason;

                        if (!!this.recognized) {
                            this.recognized(this, ev);
                        }

                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(ev.result);
                            } catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                            cb = undefined;
                        }
                    }
                }
                break;

            case "SpeechDetailedPhraseEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<IDetailedSpeechPhrase>;

                    const reason = this.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    if (reason === ResultReason.Canceled) {
                        const ev = new SpeechRecognitionCanceledEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.reason = this.implTranslateCancelResult(evResult.Result.RecognitionStatus);

                        if (!!this.canceled) {
                            this.canceled(this, ev);
                        }

                        // report result to promise.
                        if (!!err) {
                            err(JSON.stringify(evResult.Result));
                            err = undefined;
                        }
                    } else {
                        const ev = new SpeechRecognitionResultEventArgs();
                        ev.sessionId = evResult.SessionId;

                        ev.result = new SpeechRecognitionResult();
                        ev.result.json = JSON.stringify(evResult.Result);
                        ev.result.offset = evResult.Result.Offset;
                        ev.result.duration = evResult.Result.Duration;
                        ev.result.reason = reason;

                        if (!!this.recognized) {
                            this.recognized(this, ev);
                        }

                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(ev.result);
                            } catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                            cb = undefined;
                        }
                    }
                }
                break;
            case "SpeechHypothesisEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<ISpeechFragment>;

                    const ev = new SpeechRecognitionResultEventArgs();
                    ev.sessionId = evResult.SessionId;

                    ev.result = new SpeechRecognitionResult();
                    ev.result.json = JSON.stringify(evResult.Result);
                    ev.result.offset = evResult.Result.Offset;
                    ev.result.duration = evResult.Result.Duration;
                    ev.result.text = evResult.Result.Text;

                    if (!!this.recognizing) {
                        this.recognizing(this, ev);
                    }
                }
                break;
        }
    }

    private implTranslateRecognitionResult(recognitionStatus: RecognitionStatus2): ResultReason {
        let reason = ResultReason.Canceled;
        const recognitionStatus2: string = "" + recognitionStatus;
        const recstatus2 = (RecognitionStatus2 as any)[recognitionStatus2];
        switch (recstatus2) {
            case RecognitionStatus2.Success:
            case RecognitionStatus2.EndOfDictation:
                reason = ResultReason.RecognizedSpeech;
                break;

            case RecognitionStatus2.NoMatch:
                reason = ResultReason.NoMatch;
                break;

            case RecognitionStatus2.InitialSilenceTimeout:
                reason = ResultReason.Canceled;
                break;

            case RecognitionStatus2.BabbleTimeout:
                reason = ResultReason.Canceled;
                break;

            case RecognitionStatus2.Error:
            default:
                reason = ResultReason.Canceled;
                break;
        }

        return reason;
    }

    private implTranslateCancelResult(recognitionStatus: RecognitionStatus2): CancellationReason {
        let reason = CancellationReason.EndOfStream;
        const recognitionStatus2: string = "" + recognitionStatus;
        const recstatus2 = (RecognitionStatus2 as any)[recognitionStatus2];
        switch (recstatus2) {
            case RecognitionStatus2.Success:
            case RecognitionStatus2.EndOfDictation:
            case RecognitionStatus2.NoMatch:
                reason = CancellationReason.EndOfStream;
                break;
            case RecognitionStatus2.InitialSilenceTimeout:
            case RecognitionStatus2.BabbleTimeout:
            case RecognitionStatus2.Error:
            default:
                reason = CancellationReason.Error;
                break;
        }
        return reason;
    }
}
