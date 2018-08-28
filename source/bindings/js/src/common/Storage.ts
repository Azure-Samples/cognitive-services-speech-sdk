import { ArgumentNullError } from "./Error";
import { IKeyValueStorage } from "./IKeyValueStorage";
import { InMemoryStorage } from "./InMemoryStorage";

export class Storage {
    private static sessionStorage: IKeyValueStorage = new InMemoryStorage();
    private static localStorage: IKeyValueStorage = new InMemoryStorage();

    public static SetSessionStorage = (sessionStorage: IKeyValueStorage): void => {
        if (!sessionStorage) {
            throw new ArgumentNullError("sessionStorage");
        }

        Storage.sessionStorage = sessionStorage;
    }

    public static SetLocalStorage = (localStorage: IKeyValueStorage): void => {
        if (!localStorage) {
            throw new ArgumentNullError("localStorage");
        }

        Storage.localStorage = localStorage;
    }

    public static get Session(): IKeyValueStorage {
        return Storage.sessionStorage;
    }

    public static get Local(): IKeyValueStorage {
        return Storage.localStorage;
    }
}
