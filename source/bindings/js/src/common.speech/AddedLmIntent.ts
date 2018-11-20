// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { LanguageUnderstandingModelImpl } from "../sdk/LanguageUnderstandingModel";

/**
 * @class AddedLmIntent
 */
// tslint:disable-next-line:max-classes-per-file
export class AddedLmIntent {
    public modelImpl: LanguageUnderstandingModelImpl;
    public intentName: string;

    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param modelImpl - The model.
     * @param intentName - The intent name.
     */
    public constructor(modelImpl: LanguageUnderstandingModelImpl, intentName: string) {
        this.modelImpl = modelImpl;
        this.intentName = intentName;
    }
}
