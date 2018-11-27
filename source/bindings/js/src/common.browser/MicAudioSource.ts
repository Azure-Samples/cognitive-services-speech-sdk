// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { AudioStreamFormatImpl } from "../../src/sdk/Audio/AudioStreamFormat";
import { AudioStreamFormat } from "../../src/sdk/Exports";
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
    createNoDashGuid,
    Deferred,
    Events,
    EventSource,
    IAudioSource,
    IAudioStreamNode,
    IStringDictionary,
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

    private static readonly AUDIOFORMAT: AudioStreamFormatImpl = AudioStreamFormat.getDefaultInputFormat() as AudioStreamFormatImpl;

    private privStreams: IStringDictionary<Stream<ArrayBuffer>> = {};

    private privId: string;

    private privEvents: EventSource<AudioSourceEvent>;

    private privInitializeDeferral: Deferred<boolean>;

    private privRecorder: IRecorder;

    private privMediaStream: MediaStream;

    private privContext: AudioContext;

    public constructor(recorder: IRecorder, audioSourceId?: string) {
        this.privId = audioSourceId ? audioSourceId : createNoDashGuid();
        this.privEvents = new EventSource<AudioSourceEvent>();
        this.privRecorder = recorder;
    }

    public get format(): AudioStreamFormat {
        return MicAudioSource.AUDIOFORMAT;
    }

    public turnOn = (): Promise<boolean> => {
        if (this.privInitializeDeferral) {
            return this.privInitializeDeferral.promise();
        }

        this.privInitializeDeferral = new Deferred<boolean>();

        this.createAudioContext();

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
            this.privInitializeDeferral.reject(errorMsg);
            this.onEvent(new AudioSourceErrorEvent(errorMsg, "")); // mic initialized error - no streamid at this point
        } else {
            const next = () => {
                this.onEvent(new AudioSourceInitializingEvent(this.privId)); // no stream id
                getUserMedia(
                    { audio: true, video: false },
                    (mediaStream: MediaStream) => {
                        this.privMediaStream = mediaStream;
                        this.onEvent(new AudioSourceReadyEvent(this.privId));
                        this.privInitializeDeferral.resolve(true);
                    }, (error: MediaStreamError) => {
                        const errorMsg = `Error occurred during microphone initialization: ${error}`;
                        const tmp = this.privInitializeDeferral;
                        // HACK: this should be handled through onError callbacks of all promises up the stack.
                        // Unfortunately, the current implementation does not provide an easy way to reject promises
                        // without a lot of code replication.
                        // TODO: fix promise implementation, allow for a graceful reject chaining.
                        this.privInitializeDeferral = null;
                        tmp.reject(errorMsg); // this will bubble up through the whole chain of promises,
                        // with each new level adding extra "Unhandled callback error" prefix to the error message.
                        // The following line is not guaranteed to be executed.
                        this.onEvent(new AudioSourceErrorEvent(this.privId, errorMsg));
                    });
            };

            if (this.privContext.state === "suspended") {
                // NOTE: On iOS, the Web Audio API requires sounds to be triggered from an explicit user action.
                // https://github.com/WebAudio/web-audio-api/issues/790
                this.privContext.resume().then(next, (reason: any) => {
                    this.privInitializeDeferral.reject(`Failed to initialize audio context: ${reason}`);
                });
            } else {
                next();
            }
        }

        return this.privInitializeDeferral.promise();
    }

    public id = (): string => {
        return this.privId;
    }

    public attach = (audioNodeId: string): Promise<IAudioStreamNode> => {
        this.onEvent(new AudioStreamNodeAttachingEvent(this.privId, audioNodeId));

        return this.listen(audioNodeId).onSuccessContinueWith<IAudioStreamNode>(
            (streamReader: StreamReader<ArrayBuffer>) => {
                this.onEvent(new AudioStreamNodeAttachedEvent(this.privId, audioNodeId));
                return {
                    detach: () => {
                        streamReader.close();
                        delete this.privStreams[audioNodeId];
                        this.onEvent(new AudioStreamNodeDetachedEvent(this.privId, audioNodeId));
                        this.turnOff();
                    },
                    id: () => {
                        return audioNodeId;
                    },
                    read: () => {
                        return streamReader.read();
                    },
                };
            });
    }

    public detach = (audioNodeId: string): void => {
        if (audioNodeId && this.privStreams[audioNodeId]) {
            this.privStreams[audioNodeId].close();
            delete this.privStreams[audioNodeId];
            this.onEvent(new AudioStreamNodeDetachedEvent(this.privId, audioNodeId));
        }
    }

    public turnOff = (): Promise<boolean> => {
        for (const streamId in this.privStreams) {
            if (streamId) {
                const stream = this.privStreams[streamId];
                if (stream) {
                    stream.close();
                }
            }
        }

        this.onEvent(new AudioSourceOffEvent(this.privId)); // no stream now
        this.privInitializeDeferral = null;

        this.destroyAudioContext();

        return PromiseHelper.fromResult(true);
    }

    public get events(): EventSource<AudioSourceEvent> {
        return this.privEvents;
    }

    private listen = (audioNodeId: string): Promise<StreamReader<ArrayBuffer>> => {
        return this.turnOn()
            .onSuccessContinueWith<StreamReader<ArrayBuffer>>((_: boolean) => {
                const stream = new Stream<ArrayBuffer>(audioNodeId);
                this.privStreams[audioNodeId] = stream;

                try {
                    this.privRecorder.record(this.privContext, this.privMediaStream, stream);
                } catch (error) {
                    this.onEvent(new AudioStreamNodeErrorEvent(this.privId, audioNodeId, error));
                    throw error;
                }

                return stream.getReader();
            });
    }

    private onEvent = (event: AudioSourceEvent): void => {
        this.privEvents.onEvent(event);
        Events.instance.onEvent(event);
    }

    private createAudioContext = (): void => {
        if (!!this.privContext) {
            return;
        }

        // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext
        const AudioContext = ((window as any).AudioContext)
            || ((window as any).webkitAudioContext)
            || false;

        if (!AudioContext) {
            throw new Error("Browser does not support Web Audio API (AudioContext is not available).");
        }

        this.privContext = new AudioContext();
    }

    private destroyAudioContext = (): void => {
        if (!this.privContext) {
            return;
        }

        this.privRecorder.releaseMediaResources(this.privContext);

        if ("close" in this.privContext) {
            this.privContext.close();
            this.privContext = null;
        } else if (this.privContext.state === "running") {
            // Suspend actually takes a callback, but analogous to the
            // resume method, it'll be only fired if suspend is called
            // in a direct response to a user action. The later is not always
            // the case, as TurnOff is also called, when we receive an
            // end-of-speech message from the service. So, doing a best effort
            // fire-and-forget here.
            this.privContext.suspend();
        }
    }
}
