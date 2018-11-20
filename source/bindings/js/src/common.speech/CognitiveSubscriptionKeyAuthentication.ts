// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import { ArgumentNullError, Promise, PromiseHelper } from "../common/Exports";
import { AuthInfo, IAuthentication } from "./IAuthentication";

const AuthHeader: string = "Ocp-Apim-Subscription-Key";

/**
 * @class
 */
export class CognitiveSubscriptionKeyAuthentication implements IAuthentication {
    private authInfo: AuthInfo;

    /**
     * Creates and initializes an instance of the CognitiveSubscriptionKeyAuthentication class.
     * @constructor
     * @param {string} subscriptionKey - The subscription key
     */
    constructor(subscriptionKey: string) {
        if (!subscriptionKey) {
            throw new ArgumentNullError("subscriptionKey");
        }

        this.authInfo = new AuthInfo(AuthHeader, subscriptionKey);
    }

    /**
     * Fetches the subscription key.
     * @member
     * @function
     * @public
     * @param {string} authFetchEventId - The id to fetch.
     */
    public Fetch = (authFetchEventId: string): Promise<AuthInfo> => {
        return PromiseHelper.FromResult(this.authInfo);
    }

    /**
     * Fetches the subscription key.
     * @member
     * @function
     * @public
     * @param {string} authFetchEventId - The id to fetch.
     */
    public FetchOnExpiry = (authFetchEventId: string): Promise<AuthInfo> => {
        return PromiseHelper.FromResult(this.authInfo);
    }
}
