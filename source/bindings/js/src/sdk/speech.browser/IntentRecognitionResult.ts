//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

export class IntentRecognitionResult {
    /**
     * A String that represents the intent identifier being recognized.
     * @return A String that represents the intent identifier being recognized.
     */
    public intentId: string;

    /**
     * A String that represents the intent including properties being recognized.
     * @return A String that represents the intent including properties being recognized.
     */
    public languageUnderstanding: string;

    /**
     * A String that represents the intent as json.
     * @return A String that represents the intent as json.
     */
    public json: string;

    /**
     * A String that represents error result in case the call failed.
     * @return A String that represents error result in case the call failed.
     */
    public errorDetails: string;
}
