// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

/**
 * @export
 * @interface IDisposable
 */
export interface IDisposable {

    /**
     * @returns {boolean}
     *
     * @memberOf IDisposable
     */
    isDisposed(): boolean;

    /**
     * Performs cleanup operations on this instance
     *
     * @param {string} [reason] - optional reason for disposing the instance.
     * This will be used to throw errors when a operations are performed on the disposed object.
     *
     * @memberOf IDisposable
     */
    dispose(reason?: string): void;
}
