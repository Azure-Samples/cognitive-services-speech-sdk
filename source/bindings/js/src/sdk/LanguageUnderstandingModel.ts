//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { ArgumentNullError } from "../common/Error";
import { Contracts } from "./Contracts";

/**
 * Language understanding model
 * @class
 */
export class LanguageUnderstandingModel {
    /**
     * Creates and initializes a new instance
     * @constructor
     */
    private constructor() {
    }

    /**
     * Creates an language understanding model using the specified endpoint.
     * @member
     * @param {URL} uri - A String that represents the endpoint of the language understanding model.
     * @returns The language understanding model being created.
     */
    public static fromEndpoint(uri: URL): LanguageUnderstandingModel {
        Contracts.throwIfNull(uri, "uri");
        Contracts.throwIfNullOrWhitespace(uri.hostname, "uri");

        //  return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromEndpoint(uri));
        throw new ArgumentNullError("not supported");
    }

    /**
     * Creates an language understanding model using the application id of Language Understanding service.
     * @member
     * @param {string} appId - A String that represents the application id of Language Understanding service.
     * @returns The language understanding model being created.
     */
    public static fromAppId(appId: string): LanguageUnderstandingModel {
        Contracts.throwIfNullOrWhitespace(appId, "appId");

        //    return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromAppId(appId));
        throw new ArgumentNullError("not supported");
    }
}
