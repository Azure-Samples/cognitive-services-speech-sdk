import { ObjectDisposedError } from "./Error";
import { CreateNoDashGuid } from "./Guid";
import { IDetachable } from "./IDetachable";
import { IStringDictionary } from "./IDictionary";
import { IEventListener, IEventSource } from "./IEventSource";
import { PlatformEvent } from "./PlatformEvent";

export class EventSource<TEvent extends PlatformEvent> implements IEventSource<TEvent> {
    private eventListeners: IStringDictionary<(event: TEvent) => void> = {};
    private metadata: IStringDictionary<string>;
    private isDisposed: boolean = false;

    constructor(metadata?: IStringDictionary<string>) {
        this.metadata = metadata;
    }

    public OnEvent = (event: TEvent): void => {
        if (this.IsDisposed()) {
            throw (new ObjectDisposedError("EventSource"));
        }

        if (this.Metadata) {
            for (const paramName in this.Metadata) {
                if (paramName) {
                    if (event.Metadata) {
                        if (!event.Metadata[paramName]) {
                            event.Metadata[paramName] = this.Metadata[paramName];
                        }
                    }
                }
            }
        }

        for (const eventId in this.eventListeners) {
            if (eventId && this.eventListeners[eventId]) {
                this.eventListeners[eventId](event);
            }
        }
    }

    public Attach = (onEventCallback: (event: TEvent) => void): IDetachable => {
        const id = CreateNoDashGuid();
        this.eventListeners[id] = onEventCallback;
        return {
            Detach: () => {
                delete this.eventListeners[id];
            },
        };
    }

    public AttachListener = (listener: IEventListener<TEvent>): IDetachable => {
        return this.Attach(listener.OnEvent);
    }

    public IsDisposed = (): boolean => {
        return this.isDisposed;
    }

    public Dispose = (): void => {
        this.eventListeners = null;
        this.isDisposed = true;
    }

    public get Metadata(): IStringDictionary<string> {
        return this.metadata;
    }
}
