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
    RecognitionCompletionStatus,
    RecognitionEndedEvent,
    RecognitionMode,
    RecognitionStatus2,
    RecognizerConfig,
    ServiceRecognizerBase,
    SpeechConfig,
    SpeechRecognitionEvent,
    SpeechRecognitionResultEvent,
    SpeechServiceRecognizer,
} from "../speech/Exports";
import { SpeechConnectionFactory } from "../speech/SpeechConnectionFactory";
import { AudioConfigImpl } from "./Audio/AudioConfig";
import { Contracts } from "./Contracts";
import { AudioConfig, FactoryParameterNames, ISpeechProperties, KeywordRecognitionModel, OutputFormat, RecognitionErrorEventArgs, RecognitionStatus, Recognizer, RecognizerParameterNames, SpeechRecognitionResult, SpeechRecognitionResultEventArgs } from "./Exports";

/**
 * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
 */
export class SpeechRecognizer extends Recognizer {
    private disposedSpeechRecognizer: boolean = false;

    /**
     * SpeechRecognizer constructor.
     * @param parameters An set of initial properties for this recognizer
     * @param ais An optional audio input stream associated with the recognizer
     */
    public constructor(parameters: ISpeechProperties, audioInput: AudioConfig) {
        super(audioInput);

        this.parameters = parameters;
    }

    /**
     * The event IntermediateResultReceived signals that an intermediate recognition result is received.
     */
    public IntermediateResultReceived: (sender: Recognizer, event: SpeechRecognitionResultEventArgs) => void;

    /**
     * The event FinalResultReceived signals that a final recognition result is received.
     */
    public FinalResultReceived: (sender: Recognizer, event: SpeechRecognitionResultEventArgs) => void;

    /**
     * The event RecognitionErrorRaised signals that an error occurred during recognition.
     */
    public RecognitionErrorRaised: (sender: Recognizer, event: RecognitionErrorEventArgs) => void;

    /**
     * Gets the deployment id of a customized speech model that is used for speech recognition.
     * @return the deployment id of a customized speech model that is used for speech recognition.
     */
    public get deploymentId(): string {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        return this.parameters.get(RecognizerParameterNames.SpeechModelId, "00000000-0000-0000-0000-000000000000");
    }

    /**
     * Sets the deployment id of a customized speech model that is used for speech recognition.
     * @param value The deployment id of a customized speech model that is used for speech recognition.
     */
    public set deploymentId(value: string) {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
        Contracts.throwIfNullOrWhitespace(value, "value");

        this.parameters.set(RecognizerParameterNames.SpeechModelId, value);
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public get language(): string {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        return this.parameters.get(RecognizerParameterNames.SpeechRecognitionLanguage, "en-us");
    }

    /**
     * Gets the output format of recognition.
     * @return The output format of recognition.
     */
    public get outputFormat(): OutputFormat {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        if (this.parameters.get(RecognizerParameterNames.OutputFormat, "SIMPLE") === "SIMPLE") {
            return OutputFormat.Simple;
        } else {
            return OutputFormat.Detailed;
        }
    }

    /**
     * The collection of parameters and their values defined for this SpeechRecognizer.
     * @return The collection of parameters and their values defined for this SpeechRecognizer.
     */
    public parameters: ISpeechProperties;

    /**
     * Starts speech recognition, and stops after the first utterance is recognized. The task returns the recognition text as result.
     * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition
     *       like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @param cb Callback that received the SpeechRecognitionResult.
     * @param err Callback invoked in case of an error.
     */
    public recognizeAsync(cb?: (e: SpeechRecognitionResult) => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();

        this.reco = this.implRecognizerSetup(
            RecognitionMode.Interactive,
            this.parameters,
            this.audioInputStreamHolder,
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
     * @param cb Callback that received the recognition has started.
     * @param err Callback invoked in case of an error.
     */
    public startContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();

        this.reco = this.implRecognizerSetup(
            RecognitionMode.Conversation,
            this.parameters,
            this.audioInputStreamHolder,
            new SpeechConnectionFactory());

        this.implRecognizerStart(this.reco, (event: SpeechRecognitionEvent) => {
            if (this.disposedSpeechRecognizer || !this.reco) {
                return;
            }

            // report result to promise.
            if (!!cb) {
                cb();
                cb = undefined;
            }

            this.implDispatchMessageHandler(event, undefined, err);
        });
    }

    /**
     * Stops continuous speech recognition.
     * @param cb Callback that received the recognition has stopped.
     * @param err Callback invoked in case of an error.
     */
    public stopContinuousRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();

        if (!!cb) {
            cb();
        }
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @param cb Callback that received the recognition has started.
     * @param err Callback invoked in case of an error.
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
     * @param cb Callback that received the recognition has stopped.
     * @param err Callback invoked in case of an error.
     */
    public stopKeywordRecognitionAsync(cb?: () => void, err?: (e: string) => void): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.implCloseExistingRecognizer();
        throw new Error("keyword recognition not supported");
    }

    /**
     * closes all external resources held by an instance of this class.
     */
    public close(): void {
        Contracts.throwIfDisposed(this.disposedSpeechRecognizer);

        this.dispose(true);
    }

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

    protected CreateRecognizerConfig(speechConfig: SpeechConfig, recognitionMode: RecognitionMode): RecognizerConfig {
        return new RecognizerConfig(
            speechConfig,
            recognitionMode,
            this.parameters.get(RecognizerParameterNames.SpeechRecognitionLanguage, "en-us"),
            this.parameters);
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
                        const errorEvent: RecognitionErrorEventArgs = new RecognitionErrorEventArgs();

                        errorEvent.status = RecognitionStatus.Canceled;
                        errorEvent.sessionId = recoEndedEvent.SessionId;
                        errorEvent.error = recoEndedEvent.Error;

                        if (this.RecognitionErrorRaised) {
                            this.RecognitionErrorRaised(this, errorEvent); // call error handler, if configured
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
                    if (reason === RecognitionStatus.Canceled) {
                        const ev = new RecognitionErrorEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.status = reason;

                        if (!!this.RecognitionErrorRaised) {
                            this.RecognitionErrorRaised(this, ev);
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

                        if (!!this.FinalResultReceived) {
                            this.FinalResultReceived(this, ev);
                        }

                        // report result to promise.
                        if (!!cb) {
                            cb(ev.result);
                            cb = undefined;
                        }
                    }
                }
                break;

            case "SpeechDetailedPhraseEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<IDetailedSpeechPhrase>;

                    const reason = this.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    if (reason === RecognitionStatus.Canceled) {
                        const ev = new RecognitionErrorEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.status = reason;

                        if (!!this.RecognitionErrorRaised) {
                            this.RecognitionErrorRaised(this, ev);
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

                        if (!!this.FinalResultReceived) {
                            this.FinalResultReceived(this, ev);
                        }

                        // report result to promise.
                        if (!!cb) {
                            cb(ev.result);
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

                    if (!!this.IntermediateResultReceived) {
                        this.IntermediateResultReceived(this, ev);
                    }
                }
                break;
        }
    }

    private implTranslateRecognitionResult(recognitionStatus: RecognitionStatus2): RecognitionStatus {
        let reason = RecognitionStatus.Canceled;
        const recognitionStatus2: string = "" + recognitionStatus;
        const recstatus2 = (RecognitionStatus2 as any)[recognitionStatus2];
        switch (recstatus2) {
            case RecognitionStatus2.Success:
            case RecognitionStatus2.EndOfDictation:
                reason = RecognitionStatus.Recognized;
                break;

            case RecognitionStatus2.NoMatch:
                reason = RecognitionStatus.NoMatch;
                break;

            case RecognitionStatus2.InitialSilenceTimeout:
                reason = RecognitionStatus.InitialSilenceTimeout;
                break;

            case RecognitionStatus2.BabbleTimeout:
                reason = RecognitionStatus.InitialBabbleTimeout;
                break;

            case RecognitionStatus2.Error:
            default:
                reason = RecognitionStatus.Canceled;
                break;
        }

        return reason;
    }
}
