// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { ITimer } from "../common/Exports";

export class Timer implements ITimer {
    private privDelayInMillisec: number;
    private privTimerId: number;
    private privSuccessCallback: any;

    constructor(delayInMillisec: number, successCallback: any) {
        this.privDelayInMillisec = delayInMillisec;
        this.privSuccessCallback = successCallback;
    }
    public start = (...params: any[]): void => {
        if (this.privTimerId) {
            this.stop();
        }
        this.privTimerId = setTimeout(this.privSuccessCallback, this.privDelayInMillisec, params);
    }

    public stop = (): void => {
        clearTimeout(this.privTimerId);
    }
}
