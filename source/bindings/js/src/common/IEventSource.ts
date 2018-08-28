import { IDetachable } from "./IDetachable";
import { IStringDictionary } from "./IDictionary";
import { IDisposable } from "./IDisposable";
import { PlatformEvent } from "./PlatformEvent";

export interface IEventListener<TEvent extends PlatformEvent> {
    OnEvent(e: TEvent): void;
}

export interface IEventSource<TEvent extends PlatformEvent> extends IDisposable {
    Metadata: IStringDictionary<string>;

    OnEvent(e: TEvent): void;

    Attach(onEventCallback: (event: TEvent) => void): IDetachable;

    AttachListener(listener: IEventListener<TEvent>): IDetachable;
}
