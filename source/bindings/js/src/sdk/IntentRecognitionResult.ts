//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import { RecognitionResult } from "./Exports";

/**
 * Intent recognition result.
 * @class
 */
export class IntentRecognitionResult extends RecognitionResult {
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
}
