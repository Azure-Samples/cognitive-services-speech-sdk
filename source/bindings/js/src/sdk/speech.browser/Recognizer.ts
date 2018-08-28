import { MicAudioSource, PcmRecorder } from "../../common.browser/Exports";
import { IAudioSource } from "../../common/Exports";
import { CognitiveSubscriptionKeyAuthentication, Context, Device, OS, RecognitionMode, Recognizer, RecognizerConfig, SpeechConfig, SpeechRecognitionEvent, SpeechResultFormat } from "../speech/Exports";
import { SpeechConnectionFactory } from "./Exports";
import { AudioInputStream } from "./SpeechFactory";

export enum SessionEventType {
    SessionStartedEvent = 0,
    SessionStoppedEvent,
}

export enum RecognitionEventType {
    SpeechStartDetectedEvent = 0,
    SpeechEndDetectedEvent,
}

// tslint:disable-next-line:max-classes-per-file
export class ISpeechProperties {
    private keys: string[] = [] as string[];
    private values: string[] = [] as string[];

    public get(key: string, def: string): string {
        for (let n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === key) {
                return this.values[n];
            }
        }

        return def;
    }

    public set(key: string, value: string): void {
        for (let n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === key) {
                this.values[n] = value;
                return;
            }
        }

        this.keys.push(key);
        this.values.push(value);
    }

    public has(key: string): boolean {
        // tslint:disable-next-line:prefer-for-of
        for (let n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === key) {
                return true;
            }
        }

        return false;
    }

    public clone(): ISpeechProperties {
        const clonedMap = new ISpeechProperties();

        for (let n = 0; n < this.keys.length; n++) {
            clonedMap.keys.push(this.keys[n]);
            clonedMap.values.push(this.values[n]);
        }

        return clonedMap;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class SessionEventArgs {
    /**
     * Represents the event type.
     * @return Represents the event type.
     */
    public eventType: SessionEventType;

    /**
     * Represents the session identifier.
     * @return Represents the session identifier.
     */
    public SessionId: string;
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionEventArgs {
    /**
     * Represents the event type.
     */
    public eventType: RecognitionEventType;

    /**
     * Represents the session identifier.
     */
    public sessionId: string;

    /**
     * Represents the message offset
     */
    public offset: number;
}

export enum RecognitionStatus {
    /**
     * Indicates the result is a phrase that has been successfully recognized.
     */
    Recognized,

    /**
     * Indicates the result is a hypothesis text that has been recognized.
     */
    IntermediateResult,

    /**
     * Indicates that speech was detected in the audio stream, but no words from the target language were matched.
     * Possible reasons could be wrong setting of the target language or wrong format of audio stream.
     */
    NoMatch,

    /**
     * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
     */
    InitialSilenceTimeout,

    /**
     * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
     */
    InitialBabbleTimeout,

    /**
     * Indicates that an error occurred during recognition. The getErrorDetails() method returns detailed error reasons.
     */
    Canceled,
}

// tslint:disable-next-line:max-classes-per-file
export class RecognitionErrorEventArgs {
    /**
     * Specifies the error reason.
     * @return Specifies the error reason.
     */
    public status: RecognitionStatus;

    /**
     * Specifies the session identifier.
     * @return Specifies the session identifier.
     */
    public sessionId: string;
}

// tslint:disable-next-line:max-classes-per-file
export class KeywordRecognitionModel {
    private disposed: boolean = false;

    private constructor() {
    }

    /**
     * Creates a keyword recognition model using the specified filename.
     * @param fileName A string that represents file name for the keyword recognition model.
     *                 Note, the file can point to a zip file in which case the model will be extracted from the zip.
     * @return The keyword recognition model being created.
     */
    public static fromFile(fileName: string): KeywordRecognitionModel {
        // Contracts.throwIfFileDoesNotExist(fileName, "fileName");
        return new KeywordRecognitionModel();
    }

    /**
     * Dispose of associated resources.
     */
    public close(): void {
        if (this.disposed) {
            return;
        }

        this.disposed = true;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class RecognizerBase {
    private disposed: boolean = false;
    private sessionEventHandler: Array<(sender: RecognizerBase, event: SessionEventArgs) => void> = Array();
    private recognitionEventHandler: Array<(sender: RecognizerBase, event: RecognitionEventArgs) => void> = Array();

    protected audioInputStreamHolder: AudioInputStream;

    /**
     * Creates and initializes an instance of a Recognizer
     * @param ais An optional audio input stream associated with the recognizer
     */
    protected constructor(ais: AudioInputStream) {
        // Note: Since ais is optional, no test for null reference
        this.audioInputStreamHolder = ais;

        // this.sessionStartedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStartedEvent);
        // this.sessionStoppedHandler = new SessionEventHandlerImpl(this, SessionEventType.SessionStoppedEvent);
        // this.speechStartDetectedHandler = new RecognitionEventHandlerImpl(this, RecognitionEventType.SpeechStartDetectedEvent);
        // this.speechEndDetectedHandler = new RecognitionEventHandlerImpl(this, RecognitionEventType.SpeechEndDetectedEvent);
    }

    /**
     * Defines event handler for session events, e.g., SessionStartedEvent and SessionStoppedEvent.
     */
    public SessionEvent(onEventCallback: (sender: RecognizerBase, event: SessionEventArgs) => void): void {
        this.sessionEventHandler.push(onEventCallback);
    }

    /**
     * Defines event handler for session events, e.g., SpeechStartDetectedEvent and SpeechEndDetectedEvent.
     */
    public RecognitionEvent(onEventCallback: (sender: RecognizerBase, event: RecognitionEventArgs) => void): void {
        this.recognitionEventHandler.push(onEventCallback);
    }

    /**
     * Dispose of associated resources.
     */
    public close(): void {
        this.dispose(true);
    }

    /**
     * This method performs cleanup of resources.
     * The Boolean parameter disposing indicates whether the method is called from Dispose (if disposing is true) or from the finalizer (if disposing is false).
     * Derived classes should override this method to dispose resource if needed.
     * @param disposing Flag to request disposal.
     */
    protected dispose(disposing: boolean): void {
        if (this.disposed) {
            return;
        }

        if (disposing) {
            // disconnect
        }

        this.disposed = true;
    }

    //
    // ################################################################################################################
    // IMPLEMENTATION.
    // Move to independent class
    // ################################################################################################################
    //

    // Setup the recognizer
    protected implRecognizerSetup(recognitionMode: RecognitionMode, language: string, format: SpeechResultFormat, subscriptionKey: string, audioSource: IAudioSource): Recognizer {

        const recognizerConfig = new RecognizerConfig(
            new SpeechConfig(
                new Context(
                    new OS("navigator.userAgent", "Browser", null),
                    new Device("SpeechSample", "SpeechSample", "1.0.00000"))),
            recognitionMode,
            language, // Supported languages are specific to each recognition mode. Refer to docs.
            format); // SDK.SpeechResultFormat.Simple (Options - Simple/Detailed)

        const authentication = new CognitiveSubscriptionKeyAuthentication(subscriptionKey);
        const speechConnectionFactory = new SpeechConnectionFactory();

        if (!audioSource) {
            const pcmRecorder = new PcmRecorder();
            audioSource = new MicAudioSource(pcmRecorder);
        }

        return new Recognizer(
            authentication,
            speechConnectionFactory,
            audioSource,
            recognizerConfig);
    }

    // Start the recognition
    protected implRecognizerStart(recognizer: Recognizer, cb: (event: SpeechRecognitionEvent) => void): void {
        recognizer.Recognize((event: SpeechRecognitionEvent) => {
            if (this.disposed) {
                return;
            }

            let sessionEvent: SessionEventArgs;
            let recognitionEvent: RecognitionEventArgs;

            /*
                Alternative syntax for typescript devs.
                if (event instanceof SDK.RecognitionTriggeredEvent)
            */
            // TODO: The mapping of internal service events to API surface events is... bad. Needs to be cleaned up to have a common mapping
            // that's understandable.
            switch (event.Name) {
                case "RecognitionTriggeredEvent":
                case "ListeningStartedEvent":
                    // Internal events, ignore
                    break;

                case "RecognitionStartedEvent": // Fires when the client connects to the service successfuly.
                    sessionEvent = new SessionEventArgs();
                    sessionEvent.SessionId = event.SessionId;
                    sessionEvent.eventType = SessionEventType.SessionStartedEvent;
                    this.sessionEventHandler.forEach((cb2: (s: RecognizerBase, e: SessionEventArgs) => void) => {
                        cb2(this, sessionEvent);
                    });
                    break;

                case "RecognitionEndedEvent":
                    sessionEvent = new SessionEventArgs();
                    sessionEvent.SessionId = event.SessionId;
                    sessionEvent.eventType = SessionEventType.SessionStoppedEvent;
                    this.sessionEventHandler.forEach((cb2: (s: RecognizerBase, e: SessionEventArgs) => void) => {
                        cb2(this, sessionEvent);
                    });
                    break;

                case "SpeechStartDetectedEvent":
                    recognitionEvent = new RecognitionEventArgs();
                    recognitionEvent.sessionId = event.SessionId;
                    recognitionEvent.eventType = RecognitionEventType.SpeechStartDetectedEvent;
                    recognitionEvent.offset = 0; // TODO
                    this.recognitionEventHandler.forEach((cb2: (s: RecognizerBase, e: RecognitionEventArgs) => void) => {
                        cb2(this, recognitionEvent);
                    });
                    break;

                case "SpeechEndDetectedEvent":
                    recognitionEvent = new RecognitionEventArgs();
                    recognitionEvent.sessionId = event.SessionId;
                    recognitionEvent.eventType = RecognitionEventType.SpeechEndDetectedEvent;
                    recognitionEvent.offset = 0; // TODO
                    this.recognitionEventHandler.forEach((cb2: (s: RecognizerBase, e: RecognitionEventArgs) => void) => {
                        cb2(this, recognitionEvent);
                    });
                    break;

                default:
                    if (cb) {
                        cb(event); // call continuation, if configured.
                    }
            }
        });
    }
}
