import { ArgumentNullError, IKeyValueStorage } from "../common/Exports";

export class LocalStorage implements IKeyValueStorage {

    public Get = (key: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        return localStorage.getItem(key);
    }

    public GetOrAdd = (key: string, valueToAdd: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        const value = localStorage.getItem(key);
        if (value === null || value === undefined) {
            localStorage.setItem(key, valueToAdd);
        }

        return localStorage.getItem(key);
    }

    public Set = (key: string, value: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        localStorage.setItem(key, value);
    }

    public Remove = (key: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        localStorage.removeItem(key);
    }
}
