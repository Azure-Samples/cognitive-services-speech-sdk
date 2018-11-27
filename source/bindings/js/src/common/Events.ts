// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ArgumentNullError } from "./Error";
import { EventSource } from "./EventSource";
import { IEventSource } from "./IEventSource";
import { PlatformEvent } from "./PlatformEvent";

export class Events {
    private static privInstance: IEventSource<PlatformEvent> = new EventSource<PlatformEvent>();

    public static setEventSource = (eventSource: IEventSource<PlatformEvent>): void => {
        if (!eventSource) {
            throw new ArgumentNullError("eventSource");
        }

        Events.privInstance = eventSource;
    }

    public static get instance(): IEventSource<PlatformEvent> {
        return Events.privInstance;
    }
}
