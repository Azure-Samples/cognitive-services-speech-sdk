//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { Promise } from "../../../../../source/bindings/js/Speech.Browser.Sdk";

export class Settings {

    // Subscription
    public static SpeechSubscriptionKey :string = "<<YOUR_SUBSCRIPTION_KEY>>";
    public static SpeechRegion :string = "<<YOUR_REGION>>";

    public static SpeechAuthorizationToken :string = "<<YOUR_AUTH_TOKEN>>";

    public static LuisSubscriptionKey :string = "<<YOUR_LUIS_SUBSCRIPTION_KEY>>";
    public static LuisRegion :string = "<<YOUR_LUIS_REGION>>";
    public static LuisAppId :string = "<<YOUR_LUIS_APP_KEY>>";

    public static WaveFile: string = "../../../input/audio/whatstheweatherlike.wav";

    public static Keyword :string = "Computer";
    public static KeywordModel :string = "/data/keyword/kws.table";

    private static factory :sdk.SpeechFactory;

    private static isSettingsInitialized :boolean = false;
    public static s_settingsClassLock :Object;
    
    static initialize(){
       /* TODO: How do we validate the type exists in TypeScript?
       try {
            Class.forName("com.microsoft.cognitiveservices.speech.SpeechFactory");
        } catch (ClassNotFoundException e) {
            throw new UnsatisfiedLinkError(e.toString());
        }

        */
        // prevent classgc from reclaiming the settings class, thus
        // throwing away any custom setting value..
        Settings.s_settingsClassLock = new Settings();
        
        Settings.LoadSettings();
    };

    public static LoadSettings = () => {
        if(Settings.isSettingsInitialized)
            return;
        
        /* TODO: What's the equivlant parameter passing story?
        Settings.SpeechAuthorizationToken = System.getProperty("SpeechAuthorizationToken", SpeechAuthorizationToken);
        Settings.SpeechSubscriptionKey = System.getProperty("SpeechSubscriptionKey", SpeechSubscriptionKey);
        Settings.SpeechRegion = System.getProperty("SpeechRegion", SpeechRegion);

        Settings.LuisSubscriptionKey = System.getProperty("LuisSubscriptionKey", LuisSubscriptionKey);
        Settings.LuisRegion = System.getProperty("LuisRegion", LuisRegion);
        Settings.LuisAppId = System.getProperty("LuisAppId", LuisAppId);

        Settings.WaveFile = System.getProperty("WaveFile", WaveFile);

        Settings.Keyword = System.getProperty("Keyword", Keyword);
        Settings.KeywordModel = System.getProperty("KeywordModel", KeywordModel);
        */

        Settings.isSettingsInitialized = true;
    }

    
    public static getFactory() :sdk.SpeechFactory {
        if (Settings.factory == null) {
            try {
                Settings.factory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

                // PMA parameters
                Settings.factory.parameters.set("DeviceGeometry", "Circular6+1");
                Settings.factory.parameters.set("SelectedGeometry", "Circular3+1");
            } catch (ex) {
                console.error(ex.getMessage());
                Settings.displayException(ex);
                return null;
            }
        }

        return Settings.factory;
    }

    public static displayException(ex) {
        console.error(ex.getMessage() + "\n");
        for (let item of ex.getStackTrace()) {
            console.error(item.toString());
        }
    }

    public static setOnTaskCompletedListener<T>(task: Promise<T>, listener: OnTaskCompletedListener<T>)  {
       // new FutureTask<Object>(() -> {
       //     listener.onCompleted(task.get());
       //     return true;
       // }).run();
    }


}
Settings.initialize();

interface OnTaskCompletedListener<T> {
    onCompleted(taskResult: T);
}
