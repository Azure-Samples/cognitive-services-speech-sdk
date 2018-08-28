import { ITimer } from "../common/Exports";

export class Timer implements ITimer {
    private delayInMillisec: number;
    private timerId: number;
    private successCallback: any;
    constructor(delayInMillisec: number, successCallback: any) {
        this.delayInMillisec = delayInMillisec;
        this.successCallback = successCallback;
    }
    public start = (...params: any[]): void => {
        if (this.timerId) {
            this.stop();
        }
        this.timerId = setTimeout(this.successCallback, this.delayInMillisec, params);
    }

    public stop = (): void => {
        clearTimeout(this.timerId);
    }
}
