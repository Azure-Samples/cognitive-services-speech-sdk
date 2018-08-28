import {
    AudioSourceErrorEvent,
    AudioSourceEvent,
    AudioSourceInitializingEvent,
    AudioSourceOffEvent,
    AudioSourceReadyEvent,
    AudioStreamNodeAttachedEvent,
    AudioStreamNodeAttachingEvent,
    AudioStreamNodeDetachedEvent,
    AudioStreamNodeErrorEvent,
    CreateNoDashGuid,
    Deferred,
    Events,
    EventSource,
    IAudioSource,
    IAudioStreamNode,
    IStringDictionary,
    PlatformEvent,
    Promise,
    PromiseHelper,
    Stream,
    StreamReader,
} from "../common/Exports";
import { IRecorder } from "./IRecorder";

// Extending the default definition with browser specific definitions for backward compatibility
interface INavigatorUserMedia extends NavigatorUserMedia {
    webkitGetUserMedia?: (constraints: MediaStreamConstraints, successCallback: NavigatorUserMediaSuccessCallback, errorCallback: NavigatorUserMediaErrorCallback) => void;
    mozGetUserMedia?: (constraints: MediaStreamConstraints, successCallback: NavigatorUserMediaSuccessCallback, errorCallback: NavigatorUserMediaErrorCallback) => void;
    msGetUserMedia?: (constraints: MediaStreamConstraints, successCallback: NavigatorUserMediaSuccessCallback, errorCallback: NavigatorUserMediaErrorCallback) => void;
}

export class MicAudioSource implements IAudioSource {

    private streams: IStringDictionary<Stream<ArrayBuffer>> = {};

    private id: string;

    private events: EventSource<AudioSourceEvent>;

    private initializeDeferral: Deferred<boolean>;

    private recorder: IRecorder;

    private mediaStream: MediaStream;

    private context: AudioContext;

    public constructor(recorder: IRecorder, audioSourceId?: string) {
        this.id = audioSourceId ? audioSourceId : CreateNoDashGuid();
        this.events = new EventSource<AudioSourceEvent>();
        this.recorder = recorder;
    }

    public TurnOn = (): Promise<boolean> => {
        if (this.initializeDeferral) {
            return this.initializeDeferral.Promise();
        }

        this.initializeDeferral = new Deferred<boolean>();

        this.CreateAudioContext();

        const nav = window.navigator as INavigatorUserMedia;

        let getUserMedia = (
            nav.getUserMedia ||
            nav.webkitGetUserMedia ||
            nav.mozGetUserMedia ||
            nav.msGetUserMedia
        );

        if (!!nav.mediaDevices) {
            getUserMedia = (constraints: MediaStreamConstraints, successCallback: NavigatorUserMediaSuccessCallback, errorCallback: NavigatorUserMediaErrorCallback): void => {
                nav.mediaDevices
                    .getUserMedia(constraints)
                    .then(successCallback)
                    .catch(errorCallback);
            };
        }

        if (!getUserMedia) {
            const errorMsg = "Browser does not support getUserMedia.";
            this.initializeDeferral.Reject(errorMsg);
            this.OnEvent(new AudioSourceErrorEvent(errorMsg, "")); // mic initialized error - no streamid at this point
        } else {
            const next = () => {
                this.OnEvent(new AudioSourceInitializingEvent(this.id)); // no stream id
                getUserMedia(
                    { audio: true, video: false },
                    (mediaStream: MediaStream) => {
                        this.mediaStream = mediaStream;
                        this.OnEvent(new AudioSourceReadyEvent(this.id));
                        this.initializeDeferral.Resolve(true);
                    }, (error: MediaStreamError) => {
                        const errorMsg = `Error occurred during microphone initialization: ${error}`;
                        const tmp = this.initializeDeferral;
                        // HACK: this should be handled through onError callbacks of all promises up the stack.
                        // Unfortunately, the current implementation does not provide an easy way to reject promises
                        // without a lot of code replication.
                        // TODO: fix promise implementation, allow for a graceful reject chaining.
                        this.initializeDeferral = null;
                        tmp.Reject(errorMsg); // this will bubble up through the whole chain of promises,
                        // with each new level adding extra "Unhandled callback error" prefix to the error message.
                        // The following line is not guaranteed to be executed.
                        this.OnEvent(new AudioSourceErrorEvent(this.id, errorMsg));
                    });
            };

            if (this.context.state === "suspended") {
                // NOTE: On iOS, the Web Audio API requires sounds to be triggered from an explicit user action.
                // https://github.com/WebAudio/web-audio-api/issues/790
                this.context.resume().then(next, (reason: any) => {
                    this.initializeDeferral.Reject(`Failed to initialize audio context: ${reason}`);
                });
            } else {
                next();
            }
        }

        return this.initializeDeferral.Promise();
    }

    public Id = (): string => {
        return this.id;
    }

    public Attach = (audioNodeId: string): Promise<IAudioStreamNode> => {
        this.OnEvent(new AudioStreamNodeAttachingEvent(this.id, audioNodeId));

        return this.Listen(audioNodeId).OnSuccessContinueWith<IAudioStreamNode>(
            (streamReader: StreamReader<ArrayBuffer>) => {
                this.OnEvent(new AudioStreamNodeAttachedEvent(this.id, audioNodeId));
                return {
                    Detach: () => {
                        streamReader.Close();
                        delete this.streams[audioNodeId];
                        this.OnEvent(new AudioStreamNodeDetachedEvent(this.id, audioNodeId));
                        this.TurnOff();
                    },
                    Id: () => {
                        return audioNodeId;
                    },
                    Read: () => {
                        return streamReader.Read();
                    },
                };
            });
    }

    public Detach = (audioNodeId: string): void => {
        if (audioNodeId && this.streams[audioNodeId]) {
            this.streams[audioNodeId].Close();
            delete this.streams[audioNodeId];
            this.OnEvent(new AudioStreamNodeDetachedEvent(this.id, audioNodeId));
        }
    }

    public TurnOff = (): Promise<boolean> => {
        for (const streamId in this.streams) {
            if (streamId) {
                const stream = this.streams[streamId];
                if (stream) {
                    stream.Close();
                }
            }
        }

        this.OnEvent(new AudioSourceOffEvent(this.id)); // no stream now
        this.initializeDeferral = null;

        this.DestroyAudioContext();

        return PromiseHelper.FromResult(true);
    }

    public get Events(): EventSource<AudioSourceEvent> {
        return this.events;
    }

    private Listen = (audioNodeId: string): Promise<StreamReader<ArrayBuffer>> => {
        return this.TurnOn()
            .OnSuccessContinueWith<StreamReader<ArrayBuffer>>((_: boolean) => {
                const stream = new Stream<ArrayBuffer>(audioNodeId);
                this.streams[audioNodeId] = stream;

                try {
                    this.recorder.Record(this.context, this.mediaStream, stream);
                } catch (error) {
                    this.OnEvent(new AudioStreamNodeErrorEvent(this.id, audioNodeId, error));
                    throw error;
                }

                return stream.GetReader();
            });
    }

    private OnEvent = (event: AudioSourceEvent): void => {
        this.events.OnEvent(event);
        Events.Instance.OnEvent(event);
    }

    private CreateAudioContext = (): void => {
        if (!!this.context) {
            return;
        }

        // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext
        const AudioContext = ((window as any).AudioContext)
        || ((window as any).webkitAudioContext)
        || false;

        if (!AudioContext) {
            throw new Error("Browser does not support Web Audio API (AudioContext is not available).");
        }

        this.context = new AudioContext();
    }

    private DestroyAudioContext = (): void => {
        if (!this.context) {
            return;
        }

        this.recorder.ReleaseMediaResources(this.context);

        if ("close" in this.context) {
            this.context.close();
            this.context = null;
        } else if (this.context.state === "running") {
            // Suspend actually takes a callback, but analogous to the
            // resume method, it'll be only fired if suspend is called
            // in a direct response to a user action. The later is not always
            // the case, as TurnOff is also called, when we receive an
            // end-of-speech message from the service. So, doing a best effort
            // fire-and-forget here.
            this.context.suspend();
        }
    }
}
