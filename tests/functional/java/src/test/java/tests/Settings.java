// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

package tests;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.lang.reflect.Type;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

import com.google.gson.*;
import com.google.gson.reflect.TypeToken;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

public class Settings {
    private static SpeechConfig config;

    private static Boolean isSettingsInitialized = false;
    public static Object s_settingsClassLock;

    public static String Keyword = "Computer";
    public static String KeywordModel = "/data/keyword/kws.table";

    static {
        try {
            Class.forName("com.microsoft.cognitiveservices.speech.SpeechConfig");
        } catch (ClassNotFoundException e) {
            throw new UnsatisfiedLinkError(e.toString());
        }

        // prevent classgc from reclaiming the settings class, thus
        // throwing away any custom setting value..
        s_settingsClassLock = new Settings();

        try {
            LoadSettings();
        } catch (JsonIOException | JsonSyntaxException | FileNotFoundException e) {
            e.printStackTrace();
        }
    };

    public static Map<String, String> DefaultSettingsMap;
    public static Map<String, SubscriptionRegion> SubscriptionsRegionsMap;
    public static Map<String, AudioEntry> AudioUtterancesMap;

    private static void LoadSettingsJson() throws JsonIOException, JsonSyntaxException, FileNotFoundException {
        if (isSettingsInitialized) {
            return;
        }

        String cwd = System.getProperty("user.dir");
        System.out.println("Current working directory: " + cwd);

        Gson gson = new Gson();
        Type defaultSettingsType = new TypeToken<Map<String, String>>(){}.getType();
        Type subscriptionsRegionsType = new TypeToken<Map<String, SubscriptionRegion>>(){}.getType();
        Type audioEntryType = new TypeToken<Map<String, AudioEntry>>(){}.getType();
        
        File defaultSettingsFile = new File("test.defaults.json");
        
        if(defaultSettingsFile.exists()) {
            Map<String, String> defaultSettingsMap = gson.fromJson(new FileReader("test.defaults.json"), defaultSettingsType);

            defaultSettingsMap.forEach((key, value) -> DefaultSettingsMap.merge(key, value, (v1, v2) -> v2));
        }

        File subscriptionsRegionsFile = new File("test.subscriptions.regions.json");

        if(subscriptionsRegionsFile.exists()) {
            Map<String, SubscriptionRegion> subscriptionsRegionsMap = gson.fromJson(new FileReader("test.subscriptions.regions.json"), subscriptionsRegionsType);

            subscriptionsRegionsMap.forEach((key, value) -> SubscriptionsRegionsMap.merge(key, value, (v1, v2) -> v2));
        }

        File audioUtterancesFile = new File("test.audio.utterances.json");

        if(audioUtterancesFile.exists()) {
            Map<String, AudioEntry> audioUtterancesMap = gson.fromJson(new FileReader("test.audio.utterances.json"), audioEntryType);

            audioUtterancesMap.forEach((key, value) -> AudioUtterancesMap.merge(key, value, (v1, v2) -> v2));
        }
    }

    public static void LoadSettings() throws JsonIOException, JsonSyntaxException, FileNotFoundException {
        if(!isSettingsInitialized)
        {
            DefaultSettingsMap = new HashMap<String,String>();
            SubscriptionsRegionsMap = new HashMap<String,SubscriptionRegion>();
            AudioUtterancesMap = new HashMap<String,AudioEntry>();

            LoadSettingsProperties();
            LoadSettingsJson();

            isSettingsInitialized = true;
        }
    }

    private static void LoadSettingsProperties() {
        SubscriptionsRegionsMap.put("UnifiedSpeechSubscription", 
            new SubscriptionRegion()
            {{ Key=System.getProperty("SpeechSubscriptionKey");
               Region= System.getProperty("SpeechRegion");
            }});

        SubscriptionsRegionsMap.put("LanguageUnderstandingSubscription",
            new SubscriptionRegion()
            {{ Key=System.getProperty("LuisSubscriptionKey");
               Region=System.getProperty("LuisRegion");
            }});

        DefaultSettingsMap.put("LanguageUnderstandingHomeAutomationAppId", System.getProperty("LuisAppId"));

        DefaultSettingsMap.put("InputDir", System.getProperty("AudioInputDirectory"));

        AudioUtterancesMap.put("SingleUtteranceEnglish", new AudioEntry()
        {{
            FilePath=System.getProperty("WaveFile");
        }});

        AudioUtterancesMap.put("ConversationBetweenTwoPersonsEnglish", new AudioEntry()
        {{
            FilePath=System.getProperty("TwoSpeakersAudio");
        }});

        AudioUtterancesMap.put("IntentUtterance", new AudioEntry()
        {{
            FilePath=System.getProperty("TurnOnTheLampAudio");
        }});

        DefaultSettingsMap.put("SerializedSpeechActivityFile", System.getProperty("SerializedSpeechActivityFile"));

        SubscriptionsRegionsMap.put("DialogSubscription", new SubscriptionRegion()
        {{
            Key=System.getProperty("SpeechSubscriptionKeyForVirtualAssistant");
            Region=System.getProperty("SpeechRegionForVirtualAssistant");
        }});

        DefaultSettingsMap.put("DialogFunctionalTestBot", System.getProperty("SpeechChannelSecretForVirtualAssistant"));
        DefaultSettingsMap.put("ConversationTranscriptionEndpoint", System.getProperty("ConversationTranscriptionEndpoint"));
        SubscriptionsRegionsMap.put("ConversationTranscriptionPPESubscription", new SubscriptionRegion()
        {{
            Key=System.getProperty("ConversationTranscriptionPPEKey");
            Region=System.getProperty("SpeechRegionForConversationTranscription");
        }});

        SubscriptionsRegionsMap.put("ConversationTranscriptionPRODSubscription", new SubscriptionRegion()
        {{
            Key=System.getProperty("ConversationTranscriptionPRODKey");
            Region=System.getProperty("SpeechRegionForConversationTranscription");
        }});

        DefaultSettingsMap.put("Keyword", System.getProperty("Keyword"));
        DefaultSettingsMap.put("KeywordModel", System.getProperty("KeywordModel"));
    }

    public static SpeechConfig getSpeechConfig() {
        if (config == null) {
            try {
                config = SpeechConfig.fromSubscription(SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
                    SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

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

    public static String GetRootRelativePath(String input) {
        return Paths.get(DefaultSettingsMap.get(DefaultSettingsKeys.INPUT_DIR), input).toString();
    }
}
