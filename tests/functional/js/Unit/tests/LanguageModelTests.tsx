// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import * as sdk from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";
import { LanguageUnderstandingModelImpl } from "../../../../../source/bindings/js/src/sdk/LanguageUnderstandingModel";
import { Settings } from "./Settings";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

test("HappyModel", () => {
    const lang: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromAppId(Settings.LuisAppId);
    expect(lang instanceof sdk.LanguageUnderstandingModel).toEqual(true);
});

test("NullModel", () => {
    expect(() => sdk.LanguageUnderstandingModel.fromAppId(null)).toThrow();
});

test("Happy URL", () => {
    const fakeRegion: string = "fakeregion";

    // Just get some random numbers. There should be no guid / key format validation in the fromEndpoint.
    const appId: string = Math.random().toString().substr(3);
    const subKey: string = Math.random().toString().substr(3);

    const url: string = "https://" + fakeRegion + ".api.cognitive.microsoft.com/luis/v2.0/apps/" + appId + "?subscription-key=" + subKey + "&timezoneOffset=-360";

    const lang: sdk.LanguageUnderstandingModel = sdk.LanguageUnderstandingModel.fromEndpoint(new URL(url));
    expect(lang instanceof sdk.LanguageUnderstandingModel).toEqual(true);

    // Now, be all sneaky and reach into the internal class to make sure the parsing worked correctly.
    expect(lang instanceof LanguageUnderstandingModelImpl).toEqual(true);

    const langImpl: LanguageUnderstandingModelImpl = lang as LanguageUnderstandingModelImpl;

    expect(langImpl.appId).toEqual(appId);
    expect(langImpl.subscriptionKey).toEqual(subKey);
    expect(langImpl.region).toEqual(fakeRegion);
});

test("null URL", () => {
    expect(() => sdk.LanguageUnderstandingModel.fromEndpoint(null)).toThrow();
});
