//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";

export class Settings {

    // subscription
    public static SpeechSubscriptionKey: string = "<<YOUR_SUBSCRIPTION_KEY>>";
    public static SpeechRegion: string = "<<YOUR_REGION>>";

    public static SpeechAuthorizationToken: string = "<<YOUR_AUTH_TOKEN>>";

    public static LuisSubscriptionKey: string = "<<YOUR_LUIS_SUBSCRIPTION_KEY>>";
    public static LuisRegion: string = "<<YOUR_LUIS_REGION>>";
    public static LuisAppId: string = "<<YOUR_LUIS_APP_KEY>>";

    public static WaveFile: string = "../../../input/audio/whatstheweatherlike.wav";
    public static LongFile: string = "../../../input/audio/batman.wav";

    public static Keyword: string = "Computer";
    public static KeywordModel: string = "/data/keyword/kws.table";

    private static factory: sdk.SpeechFactory;

    private static IsSettingsInitialized: boolean = false;
    public static SettingsClassLock: Settings;

    public static initialize(): void {
        Settings.SettingsClassLock = new Settings();

        Settings.LoadSettings();
    }

    public static LoadSettings = () => {
        if (Settings.IsSettingsInitialized) {
            return;
        }

        Settings.IsSettingsInitialized = true;
    }
}
Settings.initialize();
