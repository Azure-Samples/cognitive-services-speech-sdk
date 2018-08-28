import { ArgumentNullError } from "./Error";
import { EventSource } from "./EventSource";
import { IEventSource } from "./IEventSource";
import { PlatformEvent } from "./PlatformEvent";

export class Events {
    private static instance: IEventSource<PlatformEvent> = new EventSource<PlatformEvent>();

    public static SetEventSource = (eventSource: IEventSource<PlatformEvent>): void => {
        if (!eventSource) {
            throw new ArgumentNullError("eventSource");
        }

        Events.instance = eventSource;
    }

    public static get Instance(): IEventSource<PlatformEvent> {
        return Events.instance;
    }
}
