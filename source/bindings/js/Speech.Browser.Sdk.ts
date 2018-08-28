import { ConsoleLoggingListener, LocalStorage, SessionStorage } from "./src/common.browser/Exports";
import { Events, Storage } from "./src/common/Exports";

// Common.Storage.SetLocalStorage(new Common.Browser.LocalStorage());
// Common.Storage.SetSessionStorage(new Common.Browser.SessionStorage());
Events.Instance.AttachListener(new ConsoleLoggingListener());

export * from "./src/common/Exports";
export * from "./src/common.browser/Exports";
export * from "./src/sdk/speech/Exports";
export * from "./src/sdk/speech.browser/Exports";
