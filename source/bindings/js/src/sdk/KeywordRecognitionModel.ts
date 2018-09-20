import { Contracts } from "./Contracts";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

/**
 * Represents a keyword recognition model for recognizing when
 * the user says a keyword to initiate further speech recognition.
 * @class KeywordRecognitionModel
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
     * @member KeywordRecognitionModel.fromFile
     * @function
     * @public
     * @param {string} fileName - A string that represents file name for the keyword recognition model.
     *                 Note, the file can point to a zip file in which case the model will be extracted from the zip.
     * @returns The keyword recognition model being created.
     */
    public static fromFile(fileName: string): KeywordRecognitionModel {
        Contracts.throwIfFileDoesNotExist(fileName, "fileName");

        throw new Error("Not yet implemented.");
    }

    /**
     * Creates a keyword recognition model using the specified filename.
     * @member KeywordRecognitionModel.fromStream
     * @function
     * @public
     * @param {string} file - A File that represents file for the keyword recognition model.
     *                 Note, the file can point to a zip file in which case the model will be extracted from the zip.
     * @returns The keyword recognition model being created.
     */
    public static fromStream(file: File): KeywordRecognitionModel {
        Contracts.throwIfNull(file, "file");

        throw new Error("Not yet implemented.");
    }

    /**
     * Dispose of associated resources.
     * @member KeywordRecognitionModel.prototype.close
     * @function
     * @public
     */
    public close(): void {
        if (this.disposed) {
            return;
        }

        this.disposed = true;
    }
}
