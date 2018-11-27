// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { PropertyId } from "./Exports";

/**
 * Represents collection of properties and their values.
 * @class PropertyCollection
 */
export class PropertyCollection {
    private privKeys: string[] = [] as string[];
    private privValues: string[] = [] as string[];

    /**
     * Returns the property value in type String. The parameter must have the same type as String.
     * Currently only String, int and bool are allowed.
     * If the name is not available, the specified defaultValue is returned.
     * @member PropertyCollection.prototype.getProperty
     * @function
     * @public
     * @param {string} key - The parameter name.
     * @param {string} def - The default value which is returned if the parameter
     *        is not available in the collection.
     * @returns {string} value of the parameter.
     */
    public getProperty(key: PropertyId | string, def?: string): string {
        let keyToUse: string;

        if (typeof key === "string") {
            keyToUse = key;
        } else {
            keyToUse = PropertyId[key];
        }

        for (let n = 0; n < this.privKeys.length; n++) {
            if (this.privKeys[n] === keyToUse) {
                return this.privValues[n];
            }
        }

        return def;
    }

    /**
     * Sets the String value of the parameter specified by name.
     * @member PropertyCollection.prototype.setProperty
     * @function
     * @public
     * @param {string} key - The parameter name.
     * @param {string} value - The value of the parameter.
     */
    public setProperty(key: string | PropertyId, value: string): void {
        let keyToUse: string;

        if (typeof key === "string") {
            keyToUse = key;
        } else {
            keyToUse = PropertyId[key];
        }

        for (let n = 0; n < this.privKeys.length; n++) {
            if (this.privKeys[n] === keyToUse) {
                this.privValues[n] = value;
                return;
            }
        }

        this.privKeys.push(keyToUse);
        this.privValues.push(value);
    }

    /**
     * Clones the collection.
     * @member PropertyCollection.prototype.clone
     * @function
     * @public
     * @returns {PropertyCollection} A copy of the collection.
     */
    public clone(): PropertyCollection {
        const clonedMap = new PropertyCollection();

        for (let n = 0; n < this.privKeys.length; n++) {
            clonedMap.privKeys.push(this.privKeys[n]);
            clonedMap.privValues.push(this.privValues[n]);
        }

        return clonedMap;
    }
}
