//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Intent recognition result.
 * @class
 */
export class IntentRecognitionResult {
    /**
     * A String that represents the intent identifier being recognized.
     * @property
     * @returns A String that represents the intent identifier being recognized.
     */
    public intentId: string;

    /**
     * A String that represents the intent including properties being recognized.
     * @property
     * @returns A String that represents the intent including properties being recognized.
     */
    public languageUnderstanding: string;

    /**
     * A String that represents the intent as json.
     * @property
     * @returns A String that represents the intent as json.
     */
    public json: string;

    /**
     * A String that represents error result in case the call failed.
     * @property
     * @returns A String that represents error result in case the call failed.
     */
    public errorDetails: string;
}
