import { ArgumentNullError } from "./Error";
import { IStringDictionary } from "./IDictionary";
import { IKeyValueStorage } from "./IKeyValueStorage";

export class InMemoryStorage implements IKeyValueStorage {

    private store: IStringDictionary<string> = {};

    public Get = (key: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        return this.store[key];
    }

    public GetOrAdd = (key: string, valueToAdd: string): string => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        if (this.store[key] === undefined) {
            this.store[key] = valueToAdd;
        }

        return this.store[key];
    }

    public Set = (key: string, value: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        this.store[key] = value;
    }

    public Remove = (key: string): void => {
        if (!key) {
            throw new ArgumentNullError("key");
        }

        if (this.store[key] !== undefined) {
            delete this.store[key];
        }
    }
}
