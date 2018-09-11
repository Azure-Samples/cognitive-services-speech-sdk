//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Represents a keyword recognition model for recognizing when
 * the user says a keyword to initiate further speech recognition.
 * @class
 */
export class KeywordRecognitionModel {
    private disposed: boolean = false;

    /**
     * Create and initializes a new instance.
     * @constructor
     */
    private constructor() {
    }

    /**
     * Creates a keyword recognition model using the specified filename.
     * @member
     * @param {string} fileName - A string that represents file name for the keyword recognition model.
     *                 Note, the file can point to a zip file in which case the model will be extracted from the zip.
     * @returns The keyword recognition model being created.
     */
    public static fromFile(fileName: string): KeywordRecognitionModel {
        // Contracts.throwIfFileDoesNotExist(fileName, "fileName");
        return new KeywordRecognitionModel();
    }

    /**
     * Dispose of associated resources.
     * @member
     */
    public close(): void {
        if (this.disposed) {
            return;
        }

        this.disposed = true;
    }
}
