// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import { ConsoleLoggingListener, LocalStorage, SessionStorage } from "./src/common.browser/Exports";
import { Events, Storage } from "./src/common/Exports";

// Common.Storage.SetLocalStorage(new Common.Browser.LocalStorage());
// Common.Storage.SetSessionStorage(new Common.Browser.SessionStorage());
Events.instance.attachListener(new ConsoleLoggingListener());

// Speech SDK API
export * from "./src/sdk/Exports";
