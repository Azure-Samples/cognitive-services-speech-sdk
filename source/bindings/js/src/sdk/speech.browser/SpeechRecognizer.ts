import { FileAudioSource } from "../../common.browser/Exports";
import { ArgumentNullError, Deferred, Promise } from "../../common/Exports";
import { ISimpleSpeechPhrase, RecognitionMode, RecognitionStatus2, Recognizer, SpeechRecognitionEvent, SpeechRecognitionResultEvent, SpeechResultFormat } from "../speech/Exports";
import { IDetailedSpeechPhrase, ISpeechFragment } from "../speech/SpeechResults";
import { AudioInputStream, OutputFormat } from "./Exports";
import { ISpeechProperties, KeywordRecognitionModel, RecognitionErrorEventArgs, RecognitionStatus, RecognizerBase } from "./Recognizer";

// tslint:disable-next-line:max-classes-per-file
export class SpeechRecognitionResultEventArgs {
    /**
     * Specifies the recognition result.
     * @return the recognition result.
     */
    public result: SpeechRecognitionResult;

    /**
     * Specifies the session identifier.
     * @return the session identifier.
     */
    public sessionId: string;
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechRecognitionResult {
    /**
     * Specifies the result identifier.
     * @return Specifies the result identifier.
     */
    public resultId: string;

    /**
     * Specifies status of the result.
     * @return Specifies status of the result.
     */
    public reason: RecognitionStatus;

    /**
     * Presents the recognized text in the result.
     * @return Presents the recognized text in the result.
     */
    public text: string;

    /**
     * Duration of recognized speech in milliseconds.
     * @return Duration of recognized speech in milliseconds.
     */
    public duration: number;

    /**
     * Offset of recognized speech in milliseconds.
     * @return Offset of recognized speech in milliseconds.
     */
    public offset: number;

    /**
     * In case of an unsuccessful recognition, provides a brief description of an occurred error.
     * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
     * @return a brief description of an error.
     */
    public errorDetails: string;

    /**
     * A string containing Json serialized recognition result as it was received from the service.
     * @return Json serialized representation of the result.
     */
    public json: string;

    /**
     *  The set of properties exposed in the result.
     * @return The set of properties exposed in the result.
     */
    public properties: ISpeechProperties;
}

// tslint:disable-next-line:max-classes-per-file
export class SpeechRecognizer extends RecognizerBase {
    private disposedSpeechRecognizer: boolean = false;
    private intermediateResultReceivedHandler: Array<(sender: RecognizerBase, event: SpeechRecognitionResultEventArgs) => void> = Array();
    private finalResultReceivedHandler: Array<(sender: RecognizerBase, event: SpeechRecognitionResultEventArgs) => void> = {} = Array();
    private recognitionErrorRaisedHandler: Array<(sender: RecognizerBase, event: RecognitionErrorEventArgs) => void> = Array();

    /**
     * SpeechRecognizer constructor.
     * @param parameters An set of initial properties for this recognizer
     * @param ais An optional audio input stream associated with the recognizer
     */
    public constructor(parameters: ISpeechProperties, ais: AudioInputStream) {
        super(ais);

        this.parameters = parameters;

        //  this.intermediateResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ false);
        //  this.finalResultHandler = new ResultHandlerImpl(this, /*isFinalResultHandler:*/ true);
        //  this.errorHandler = new ErrorHandlerImpl(this);

        //  this.recoImpl.getSessionStarted().AddEventListener(this.sessionStartedHandler);
        //  this.recoImpl.getSessionStopped().AddEventListener(this.sessionStoppedHandler);
        //  this.recoImpl.getSpeechStartDetected().AddEventListener(this.speechStartDetectedHandler);
        //  this.recoImpl.getSpeechEndDetected().AddEventListener(this.speechEndDetectedHandler);
    }

    /**
     * The event IntermediateResultReceived signals that an intermediate recognition result is received.
     */
    public IntermediateResultReceived(onEventCallback: (sender: RecognizerBase, event: SpeechRecognitionResultEventArgs) => void): void {
        this.intermediateResultReceivedHandler.push(onEventCallback);
    }

    /**
     * The event FinalResultReceived signals that a final recognition result is received.
     */
    public FinalResultReceived(onEventCallback: (sender: RecognizerBase, event: SpeechRecognitionResultEventArgs) => void): void {
        this.finalResultReceivedHandler.push(onEventCallback);
    }

    /**
     * The event RecognitionErrorRaised signals that an error occurred during recognition.
     */
    public RecognitionErrorRaised(onEventCallback: (sender: RecognizerBase, event: RecognitionErrorEventArgs) => void): void {
        this.recognitionErrorRaisedHandler.push(onEventCallback);
    }

    /**
     * Gets the deployment id of a customized speech model that is used for speech recognition.
     * @return the deployment id of a customized speech model that is used for speech recognition.
     */
    public get deploymentId(): string {
        return this.parameters.get("deploymentId", undefined); // "_Parameters.getString(RecognizerParameterNames.SpeechModelId)";
    }

    /**
     * Sets the deployment id of a customized speech model that is used for speech recognition.
     * @param value The deployment id of a customized speech model that is used for speech recognition.
     */
    public set deploymentId(value: string) {
        // Contracts.throwIfNullOrWhitespace(value, "value");

        //  _Parameters.set(RecognizerParameterNames.SpeechModelId, value);
        this.parameters.set("deploymentId", value);
    }

    /**
     * Gets the spoken language of recognition.
     * @return The spoken language of recognition.
     */
    public get language(): string {
        return this.parameters.get("SPEECH-RecoLanguage", "en-us"); // return "_Parameters.getString(RecognizerParameterNames.SpeechRecognitionLanguage)";
    }

    /**
     * Gets the output format of recognition.
     * @return The output format of recognition.
     */
    public get outputFormat(): OutputFormat {
        if (this.parameters.get("outputFormat", "SIMPLE") === "SIMPLE") {
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
     * Note: RecognizeAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @return A task representing the recognition operation. The task returns a value of SpeechRecognitionResult
     */
    public recognizeAsync(): Promise<SpeechRecognitionResult> {
        this.implCloseExistingRecognizer();

        let deferred = new Deferred<SpeechRecognitionResult>();
        const promise = deferred.Promise();

        let audioSource;
        if (this.audioInputStreamHolder) {
            audioSource = new FileAudioSource(this.audioInputStreamHolder.file);
        } else if (this.parameters.has("audioFile")) {
            const file = new File([""], this.parameters.get("audioFile", undefined));
            audioSource = new FileAudioSource(file);
        }

        this.reco = this.implRecognizerSetup(
            RecognitionMode.Interactive,
            this.parameters.get("SPEECH-RecoLanguage", "en-us"),
            this.parameters.get("outputFormat", "SIMPLE") ? SpeechResultFormat.Simple : SpeechResultFormat.Detailed,
            this.parameters.get("SPEECH-SubscriptionKey", undefined),
            audioSource);

        this.implRecognizerStart(this.reco,  (event: SpeechRecognitionEvent) => {
            if (this.disposedSpeechRecognizer || !this.reco) {
                return;
            }

            /*
             Alternative syntax for typescript devs.
             if (event instanceof SDK.RecognitionTriggeredEvent)
            */
            switch (event.Name) {
                case "SpeechSimplePhraseEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<ISimpleSpeechPhrase>;

                    const reason = this.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    if (reason === RecognitionStatus.Canceled) {
                        const ev = new RecognitionErrorEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.status = reason;

                        this.recognitionErrorRaisedHandler.forEach((cb: (s: RecognizerBase, e: RecognitionErrorEventArgs) => void) => {
                            cb(this, ev);
                        });

                        // report result to promise.
                        if (deferred !== undefined) {
                            deferred.Reject(evResult.Result.DisplayText);
                            deferred = undefined;
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

                        this.finalResultReceivedHandler.forEach((cb: (s: RecognizerBase, e: SpeechRecognitionResultEventArgs) => void) => {
                            cb(this, ev);
                        });

                        // report result to promise.
                        if (deferred !== undefined) {
                            deferred.Resolve(ev.result);
                            deferred = undefined;
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

                        this.recognitionErrorRaisedHandler.forEach((cb: (s: RecognizerBase, e: RecognitionErrorEventArgs) => void) => {
                            cb(this, ev);
                        });

                        // report result to promise.
                        if (deferred !== undefined) {
                            deferred.Reject(JSON.stringify(evResult.Result));
                            deferred = undefined;
                        }
                    } else {
                        const ev = new SpeechRecognitionResultEventArgs();
                        ev.sessionId = evResult.SessionId;

                        ev.result = new SpeechRecognitionResult();
                        ev.result.json = JSON.stringify(evResult.Result);
                        ev.result.offset = evResult.Result.Offset;
                        ev.result.duration = evResult.Result.Duration;
                        ev.result.reason = reason;

                        this.finalResultReceivedHandler.forEach((cb: (s: RecognizerBase, e: SpeechRecognitionResultEventArgs) => void) => {
                            cb(this, ev);
                        });

                        // report result to promise.
                        if (deferred !== undefined) {
                            deferred.Resolve(ev.result);
                            deferred = undefined;
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

                    this.intermediateResultReceivedHandler.forEach((cb: (s: RecognizerBase, e: SpeechRecognitionResultEventArgs) => void) => {
                        cb(this, ev);
                    });
                }
                break;
            }
        });

        return promise;
    }

    /**
     * Starts speech recognition on a continuous audio stream, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public startContinuousRecognitionAsync(): Promise<void> {
        this.implCloseExistingRecognizer();

        let deferred = new Deferred<void>();
        const promise = deferred.Promise();

        let audioSource;
        if (this.audioInputStreamHolder) {
            audioSource = new FileAudioSource(this.audioInputStreamHolder.file);
        } else if (this.parameters.has("audioFile")) {
            const file = new File([""], this.parameters.get("audioFile", undefined));
            audioSource = new FileAudioSource(file);
        }

        this.reco = this.implRecognizerSetup(
            RecognitionMode.Conversation,
            this.parameters.get("language", "en-us"),
            this.parameters.get("outputFormat", "SIMPLE") ? SpeechResultFormat.Simple : SpeechResultFormat.Detailed,
            this.parameters.get("SPEECH-SubscriptionKey", undefined),
            audioSource);

        this.implRecognizerStart(this.reco,  (event: SpeechRecognitionEvent) => {
            if (this.disposedSpeechRecognizer || !this.reco) {
                return;
            }

            // report result to promise.
            if (deferred !== undefined) {
                deferred.Resolve(undefined);
                deferred = undefined;
            }

            /*
             Alternative syntax for typescript devs.
             if (event instanceof SDK.RecognitionTriggeredEvent)
            */
            switch (event.Name) {
                case "SpeechSimplePhraseEvent":
                {
                    const evResult = event as SpeechRecognitionResultEvent<ISimpleSpeechPhrase>;

                    const reason = this.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    if (reason === RecognitionStatus.Canceled) {
                        const ev = new RecognitionErrorEventArgs();
                        ev.sessionId = evResult.SessionId;
                        ev.status = reason;

                        this.recognitionErrorRaisedHandler.forEach((cb: (s: RecognizerBase, e: RecognitionErrorEventArgs) => void) => {
                            cb(this, ev);
                        });

                        // report result to promise.
                        if (deferred !== undefined) {
                            deferred.Reject(evResult.Result.DisplayText);
                            deferred = undefined;
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

                        this.finalResultReceivedHandler.forEach((cb: (s: RecognizerBase, e: SpeechRecognitionResultEventArgs) => void) => {
                            cb(this, ev);
                        });
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

                        this.recognitionErrorRaisedHandler.forEach((cb: (s: RecognizerBase, e: RecognitionErrorEventArgs) => void) => {
                            cb(this, ev);
                        });

                        // report result to promise.
                        if (deferred !== undefined) {
                            deferred.Reject(JSON.stringify(evResult.Result));
                            deferred = undefined;
                        }
                    } else {
                        const ev = new SpeechRecognitionResultEventArgs();
                        ev.sessionId = evResult.SessionId;

                        ev.result = new SpeechRecognitionResult();
                        ev.result.json = JSON.stringify(evResult.Result);
                        ev.result.offset = evResult.Result.Offset;
                        ev.result.duration = evResult.Result.Duration;
                        ev.result.reason = reason;

                        this.finalResultReceivedHandler.forEach((cb: (s: RecognizerBase, e: SpeechRecognitionResultEventArgs) => void) => {
                            cb(this, ev);
                        });
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

                    this.intermediateResultReceivedHandler.forEach((cb: (s: RecognizerBase, e: SpeechRecognitionResultEventArgs) => void) => {
                        cb(this, ev);
                    });
                }
                break;
            }
        });

        return promise;
    }

    /**
     * Stops continuous speech recognition.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public stopContinuousRecognitionAsync(): Promise<void> {
        const deferred = new Deferred<void>();

        this.implCloseExistingRecognizer();

        deferred.Resolve(undefined);

        return deferred.Promise();
    }

    /**
     * Starts speech recognition on a continuous audio stream with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @param model The keyword recognition model that specifies the keyword to be recognized.
     * @return A task representing the asynchronous operation that starts the recognition.
     */
    public startKeywordRecognitionAsync(model: KeywordRecognitionModel): Promise<void> {
        //  Contracts.throwIfNull(model, "model");

        //  return s_executorService.submit(() -> {
        //          recoImpl.StartKeywordRecognition(model.getModelImpl());
        //          return null;
        //      });
        this.implCloseExistingRecognizer();
        throw new ArgumentNullError("keyword recognition not supported");
    }

    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Cognitive Services Device SDK. This functionality is currently not included in the SDK itself.
     * @return A task representing the asynchronous operation that stops the recognition.
     */
    public stopKeywordRecognitionAsync(): Promise<void> {
        //  return s_executorService.submit(() -> {
        //          recoImpl.StopKeywordRecognition();
        //          return null;
        //      });
        this.implCloseExistingRecognizer();
        throw new ArgumentNullError("keyword recognition not supported");
    }

    /**
     * closes all external resources held by an instance of this class.
     */
    public close(): void {
        this.dispose(true);
    }

    protected dispose(disposing: boolean): void {
        if (this.disposedSpeechRecognizer) {
            return;
        }

        if (disposing) {
            //  intermediateResultHandler.delete();
            //  finalResultHandler.delete();
            //  errorHandler.delete();
            //  getRecoImpl().delete();
            //  _Parameters.close();
            this.implCloseExistingRecognizer();
            this.disposedSpeechRecognizer = true;
        }

        super.dispose(disposing);
    }

    // tslint:disable-next-line:member-ordering
    private reco: Recognizer;

    private implCloseExistingRecognizer(): void {
        if (this.reco) {
            this.reco.AudioSource.TurnOff();
            this.reco = undefined;
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
