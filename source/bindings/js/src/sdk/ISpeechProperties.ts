//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Represents collection of parameters and their values.
 * @class
 */
export class ISpeechProperties {
    private keys: string[] = [] as string[];
    private values: string[] = [] as string[];

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
        for (let n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === key) {
                return this.values[n];
            }
        }

        return def;
    }

    /**
     * Sets the String value of the parameter specified by name.
     * @member
     * @param {string} key - The parameter name.
     * @param {string} value - The value of the parameter.
     */
    public set(key: string, value: string): void {
        for (let n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === key) {
                this.values[n] = value;
                return;
            }
        }

        this.keys.push(key);
        this.values.push(value);
    }

    /**
     * Checks whether the parameter specified by name has a String value.
     * @member
     * @param {string} key - The parameter name.
     * @returns true if the parameter has a value, and false otherwise.
     */
    public has(key: string): boolean {
        // tslint:disable-next-line:prefer-for-of
        for (let n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === key) {
                return true;
            }
        }

        return false;
    }

    /**
     * Clones the collection.
     * @member
     * @returns A copy of the collection.
     */
    public clone(): ISpeechProperties {
        const clonedMap = new ISpeechProperties();

        for (let n = 0; n < this.keys.length; n++) {
            clonedMap.keys.push(this.keys[n]);
            clonedMap.values.push(this.values[n]);
        }

        return clonedMap;
    }
}
