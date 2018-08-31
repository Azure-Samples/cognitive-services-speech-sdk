//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
import { ArgumentNullError } from "../../common/Error";
import { Contracts } from "./Contracts";

export class LanguageUnderstandingModel {
    private constructor() {
    }

    /**
     * Creates an language understanding model using the specified endpoint.
     * @param uri A String that represents the endpoint of the language understanding model.
     * @return The language understanding model being created.
     */
    public static fromEndpoint(uri: URL): LanguageUnderstandingModel {
        Contracts.throwIfNull(uri, "uri");
        Contracts.throwIfNullOrWhitespace(uri.hostname, "uri");

        //  return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromEndpoint(uri));
        throw new ArgumentNullError("not supported");
    }

    /**
     * Creates an language understanding model using the application id of Language Understanding service.
     * @param appId A String that represents the application id of Language Understanding service.
     * @return The language understanding model being created.
     */
    public static fromAppId(appId: string): LanguageUnderstandingModel {
        Contracts.throwIfNullOrWhitespace(appId, "appId");

        //    return new LanguageUnderstandingModel(com.microsoft.cognitiveservices.speech.internal.LanguageUnderstandingModel.FromAppId(appId));
        throw new ArgumentNullError("not supported");
    }
}
