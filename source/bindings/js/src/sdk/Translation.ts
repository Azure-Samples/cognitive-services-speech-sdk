//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

import { PropertyCollection } from "./Exports";

/**
 * Represents collection of parameters and their values.
 * @class
 */
export class Translation {
    // Use an PropertyCollection internally, just wrapping it to hide the | enum syntax it has.
    private map: PropertyCollection = new PropertyCollection();

    /**
     * Returns the parameter value in type String. The parameter must have the same type as String.
     * Currently only String, int and bool are allowed.
     * If the name is not available, the specified defaultValue is returned.
     * @member
     * @param {string} key - The parameter name.
     * @param {string} def - The default value which is returned if the parameter is not available in the collection.
     * @returns value of the parameter.
     */
    public get(key: string, def?: string): string {
        return this.map.getProperty(key, def);
    }

    /**
     * Sets the String value of the parameter specified by name.
     * @member
     * @param {string} key - The parameter name.
     * @param {string} value - The value of the parameter.
     */
    public set(key: string, value: string): void {
        this.map.setProperty(key, value);
    }
}
