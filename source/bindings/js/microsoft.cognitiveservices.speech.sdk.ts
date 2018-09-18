import { ConsoleLoggingListener, LocalStorage, SessionStorage } from "./src/common.browser/Exports";
import { Events, Storage } from "./src/common/Exports";

// Common.Storage.SetLocalStorage(new Common.Browser.LocalStorage());
// Common.Storage.SetSessionStorage(new Common.Browser.SessionStorage());
Events.Instance.AttachListener(new ConsoleLoggingListener());

// Speech SDK API
export * from "./src/sdk/Exports";
