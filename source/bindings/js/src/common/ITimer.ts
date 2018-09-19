//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

export interface ITimer {
    /**
     * start timer
     *
     * @param {number} delay
     * @param {(...args: any[]) => any} successCallback
     * @returns {*}
     *
     * @memberOf ITimer
     */
    start(): void;

    /**
     * stops timer
     *
     * @param {*} timerId
     *
     * @memberOf ITimer
     */
    stop(): void;
}
