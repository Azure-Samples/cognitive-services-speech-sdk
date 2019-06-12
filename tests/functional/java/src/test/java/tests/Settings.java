package tests;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

import com.microsoft.cognitiveservices.speech.SpeechConfig;

public class Settings {

    // Subscription
    public static String SpeechSubscriptionKey = "<<YOUR_SUBSCRIPTION_KEY>>";
    public static String SpeechRegion = "<<YOUR_REGION>>";


    public static String LuisSubscriptionKey = "<<YOUR_LUIS_SUBSCRIPTION_KEY>>";
    public static String LuisRegion = "<<YOUR_LUIS_REGION>>";
    public static String LuisAppId = "<<YOUR_LUIS_APP_KEY>>";

    public static String AudioInputDirectory = ".";
    public static String WavFile = "whatstheweatherlike.wav";
    public static String TwoSpeakersAudio = "katiesteve.wav";
    public static String TwoSpeakersAudioUtterance = "Good morning Steve.";

    public static String Keyword = "Computer";
    public static String KeywordModel = "/data/keyword/kws.table";

    public static String SerializedSpeechActivityFile = "activityWithSpeech.json";
    public static String SpeechSubscriptionKeyForVirtualAssistant = "<<YOUR_SUBSCRIPTION_KEY>>";
    public static String SpeechRegionForVirtualAssistant = "westus2";
    public static String SpeechChannelSecretForVirtualAssistant = "<<SPEECH_CHANNEL_SECRET>>";

    public static String ConversationTranscriptionEndpoint = "<<YOUR_CONVERSATION_TRANSCRIPTION_ENDPOINT>>";
    public static String ConversationTranscriptionPPEKey = "<<YOUR_CONVERSATION_TRANSCRIPTION_PPE_KEY>>";
    public static String ConversationTranscriptionPRODKey = "<<YOUR_CONVERSATION_TRANSCRIPTION_PROD_KEY>>";
    public static String SpeechRegionForConversationTranscription = "centralus";
    
    private static SpeechConfig config;

    private static Boolean isSettingsInitialized = false;
    public static Object s_settingsClassLock;

    static {
        try {
            Class.forName("com.microsoft.cognitiveservices.speech.SpeechConfig");
        } catch (ClassNotFoundException e) {
            throw new UnsatisfiedLinkError(e.toString());
        }

        // prevent classgc from reclaiming the settings class, thus
        // throwing away any custom setting value..
        s_settingsClassLock = new Settings();

        LoadSettings();
    };

    public static void LoadSettings() {
        if (isSettingsInitialized)
            return;

        SpeechSubscriptionKey = System.getProperty("SpeechSubscriptionKey", SpeechSubscriptionKey);
        SpeechRegion = System.getProperty("SpeechRegion", SpeechRegion);

        LuisSubscriptionKey = System.getProperty("LuisSubscriptionKey", LuisSubscriptionKey);
        LuisRegion = System.getProperty("LuisRegion", LuisRegion);
        LuisAppId = System.getProperty("LuisAppId", LuisAppId);

        AudioInputDirectory = System.getProperty("AudioInputDirectory", AudioInputDirectory);

        WavFile = System.getProperty("WaveFile", AudioInputDirectory + "/" + WavFile);
        TwoSpeakersAudio = System.getProperty("TwoSpeakersAudio", AudioInputDirectory + "/" + TwoSpeakersAudio);

        SerializedSpeechActivityFile = System.getProperty("SerializedSpeechActivityFile", AudioInputDirectory + "/" + SerializedSpeechActivityFile);

        SpeechSubscriptionKeyForVirtualAssistant = System.getProperty("SpeechSubscriptionKeyForVirtualAssistant", SpeechSubscriptionKeyForVirtualAssistant);
        SpeechRegionForVirtualAssistant = System.getProperty("SpeechRegionForVirtualAssistant", SpeechRegionForVirtualAssistant);
        SpeechChannelSecretForVirtualAssistant = System.getProperty("SpeechChannelSecretForVirtualAssistant", SpeechChannelSecretForVirtualAssistant);

        ConversationTranscriptionEndpoint = System.getProperty("ConversationTranscriptionEndpoint", ConversationTranscriptionEndpoint);
        ConversationTranscriptionPPEKey = System.getProperty("ConversationTranscriptionPPEKey", ConversationTranscriptionPPEKey);
        ConversationTranscriptionPRODKey = System.getProperty("ConversationTranscriptionPRODKey", ConversationTranscriptionPRODKey);
        SpeechRegionForConversationTranscription = System.getProperty("SpeechRegionForConversationTranscription", SpeechRegionForConversationTranscription);

        Keyword = System.getProperty("Keyword", Keyword);
        KeywordModel = System.getProperty("KeywordModel", KeywordModel);

        isSettingsInitialized = true;
    }


    public static SpeechConfig getSpeechConfig() {
        if (config == null) {
            try {
                config = SpeechConfig.fromSubscription(SpeechSubscriptionKey, SpeechRegion);

                // PMA parameters
                config.setProperty("DeviceGeometry", "Circular6+1");
                config.setProperty("SelectedGeometry", "Circular3+1");
            } catch (Exception ex) {
                System.out.println(ex.getMessage());
                displayException(ex);
                return null;
            }
        }

        return config;
    }

    public static void displayException(Exception ex) {
        System.out.println(ex.getMessage() + "\n");
        for (StackTraceElement item : ex.getStackTrace()) {
            System.out.println(item.toString());
        }
    }

    public static <T> void setOnTaskCompletedListener(final Future<T> task, final OnTaskCompletedListener<T> listener) {
        new FutureTask<Object>(() -> {
            listener.onCompleted(task.get());
            return true;
        }).run();
    }

    public interface OnTaskCompletedListener<T> {
        public void onCompleted(T taskResult);
    }
}
