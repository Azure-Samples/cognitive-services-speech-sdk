//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/microsoft.cognitiveservices.speech.sdk";

export class Settings {

    // subscription
    public static SpeechSubscriptionKey: string = "<<YOUR_SUBSCRIPTION_KEY>>";
    public static SpeechRegion: string = "<<YOUR_REGION>>";
    public static SpeechAuthorizationToken: string = "<<YOUR_AUTH_TOKEN>>";

    public static SpeechTestEndpointId: string = "<<YOUR_TEST_ENDPOINT_ID>>";

    public static LuisSubscriptionKey: string = "<<YOUR_LUIS_SUBSCRIPTION_KEY>>";
    public static LuisRegion: string = "<<YOUR_LUIS_REGION>>";
    public static LuisAppEndPointHref: string = "<<YOUR_LUIS_APP_URL>>";

    public static InputDir: string = "../../../input/audio/";

    /*
     * The intent behing this setting is that at test execution time the WaveFile below will contain speech
     * that the LUIS app above will recognize as an intent with this ID.
     *
     * Since the default wave file asks "What's the weather like?", an intent with the Id of "Weather" seems reasonable.
     */
    public static LuisValidIntentId: string = "HomeAutomation.TurnOn";
    public static LuisAppKey: string;
    public static LuisWaveFileLanguage: string = "en-US";
    public static LuisWaveFile: string = Settings.InputDir + "TurnOnTheLamp.wav";
    public static LuisWavFileText: string = "Turn on the lamp.";
    public static LuisWaveFileDuration: number = 11000000;
    public static LuisWaveFileOffset: number = 4000000;

    // Currently the other bindings read the app key out of the shell environment setup by
    // $\ci\set-test-variables.sh and not a secret passed by VSTS. So the question isn't
    // should we set it in code, but where...
    public static LuisAppId: string = "b687b851-56c5-4d31-816f-35a741a3f0be";

    public static WaveFile: string = Settings.InputDir + "whatstheweatherlike.wav";
    public static WaveFileLanguage: string = "en-US";
    public static WaveFileDuration: number = 12900000;
    public static WaveFileOffset: number = 1000000;
    public static WaveFileText: string = "What's the weather like?";

    public static LongFile: string = "../../../input/audio/batman.wav";

    public static Keyword: string = "Computer";
    public static KeywordModel: string = "/data/keyword/kws.table";

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

        if (undefined === Settings.LuisAppKey) {
            Settings.LuisAppKey = Settings.LuisSubscriptionKey;
        }

        Settings.IsSettingsInitialized = true;
    }
}
Settings.initialize();
