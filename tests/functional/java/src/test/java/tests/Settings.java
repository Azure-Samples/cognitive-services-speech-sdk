// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

package tests;

import java.io.*;
import java.lang.reflect.Type;
import java.util.HashMap;
import java.util.Map;

import com.google.gson.*;
import com.google.gson.reflect.TypeToken;
import com.microsoft.cognitiveservices.speech.*;

public class Settings {
    private static SpeechConfig config;

    private static Boolean isSettingsInitialized = false;
    public static Object s_settingsClassLock;

    public static String Keyword = "Computer";
    public static String KeywordModel = "/data/keyword/kws.table";
    public static int TestRetryCount = 3;

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
        } catch (JsonIOException | JsonSyntaxException | FileNotFoundException | UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    };

    public static Map<String, String> DefaultSettingsMap;
    public static Map<String, SubscriptionRegion> SubscriptionsRegionsMap;
    public static Map<String, AudioEntry> AudioUtterancesMap;

    private static void LoadSettingsJson() throws JsonIOException, JsonSyntaxException, FileNotFoundException, UnsupportedEncodingException {
        if (isSettingsInitialized) {
            return;
        }

        String cwd = System.getProperty("user.dir");
        System.out.println("Current working directory: " + cwd);

        Gson gson = new Gson();
        Type defaultSettingsType = new TypeToken<Map<String, String>>(){}.getType();
        Type subscriptionsRegionsType = new TypeToken<Map<String, SubscriptionRegion>>(){}.getType();
        Type audioEntryType = new TypeToken<Map<String, AudioEntry>>(){}.getType();

        // Default Settings
        File defaultSettingsFile = new File("test.defaults.json");
        Map<String, String> defaultSettingsMap;

        String defaultSettingsProperties = System.getProperty("defaultSettings");
        if(defaultSettingsProperties!= null) {
            DefaultSettingsMap = gson.fromJson(defaultSettingsProperties, defaultSettingsType);
        }
        System.out.println("Opening defaultSettingsFile");
        if(defaultSettingsFile.exists()) {
            System.out.println("Reading test.defaults.json");
            defaultSettingsMap = gson.fromJson(new FileReader("test.defaults.json"), defaultSettingsType);

            for (String key : defaultSettingsMap.keySet()) {
                System.out.println("Processing map key: " + key);
                // If the master map has this key registered
                if (DefaultSettingsMap.containsKey(key)) {
                    // And there is no value set for this key
                    System.out.println("The master map had a entry for " + key + ", checking if there's a value");
                    if (DefaultSettingsMap.get(key) == null) {
                        // Set this keys value
                        System.out.println("The master map had no value for " + key + ", setting it to " + defaultSettingsMap.get(key));
                        DefaultSettingsMap.put(key, defaultSettingsMap.get(key));
                    }
                } else {
                    // This key didn't exist in the master
                    System.out.println("Key " + key + ", did not exist in the master map, adding it with the value " + defaultSettingsMap.get(key));
                    DefaultSettingsMap.put(key, defaultSettingsMap.get(key));
                }
                System.out.println("Key " + key + " is set with value " + DefaultSettingsMap.get(key));
            }
        }
        // Subscriptions and regions
        File subscriptionsRegionsFile = new File("test.subscriptions.regions.json");
        Map<String, SubscriptionRegion> subscriptionsRegionsMap;

        String subscriptionsRegionsProperties = System.getProperty("subscriptionsRegions");
        if(subscriptionsRegionsProperties != null) {
            SubscriptionsRegionsMap = gson.fromJson(subscriptionsRegionsProperties, subscriptionsRegionsType);
        }
        System.out.println("Opening subscriptionsRegionsFile");
        if(subscriptionsRegionsFile.exists()) {
            System.out.println("Reading test.subscriptions.regions.json");
            subscriptionsRegionsMap = gson.fromJson(new FileReader("test.subscriptions.regions.json"), subscriptionsRegionsType);

            for (String key : subscriptionsRegionsMap.keySet()) {
                System.out.println("Processing map key: " + key);
                SubscriptionRegion subscriptionRegion = subscriptionsRegionsMap.get(key);

                // If the master map has this key registered
                if (!SubscriptionsRegionsMap.containsKey(key) || SubscriptionsRegionsMap.get(key).Key == null || SubscriptionsRegionsMap.get(key).Region == null) {
                    // And there is no value set for this key
                    SubscriptionsRegionsMap.put(key, subscriptionRegion);
                    System.out.println("SubscriptionsRegionsMap has added Key " + key + " with key " + SubscriptionsRegionsMap.get(key).Key + " and Region " + SubscriptionsRegionsMap.get(key).Region);
                }
            }
        }

        // Audio utterances
        File audioUtterancesFile = new File("test.audio.utterances.json");
        Map<String, AudioEntry> audioUtterancesMap;

        String audioUtterancesProperties = System.getProperty("audioUtterances");
        if(audioUtterancesProperties != null) {
            AudioUtterancesMap = gson.fromJson(audioUtterancesProperties, audioEntryType);
        }
        System.out.println("Opening audioUtterancesFile");
        if(audioUtterancesFile.exists()) {
            System.out.println("Reading test.audio.utterances.json");
            audioUtterancesMap = gson.fromJson(new InputStreamReader(new FileInputStream("test.audio.utterances.json"), "UTF-8"), audioEntryType);

            for (String key : audioUtterancesMap.keySet()) {
                System.out.println("Processing map key: " + key);
                AudioEntry audioEntry = audioUtterancesMap.get(key);

                // If the master map has this key registered
                if (!AudioUtterancesMap.containsKey("key") || AudioUtterancesMap.get(key).FilePath == null) {
                    AudioUtterancesMap.put(key, audioEntry);
                    System.out.println("AudioUtterancesMap has added Key " + key + " with value " + AudioUtterancesMap.get(key));
                }
            }
        }
    }

    public static void LoadSettings() throws JsonIOException, JsonSyntaxException, FileNotFoundException, UnsupportedEncodingException {
        if(!isSettingsInitialized)
        {
            DefaultSettingsMap = new HashMap<String,String>();
            SubscriptionsRegionsMap = new HashMap<String,SubscriptionRegion>();
            AudioUtterancesMap = new HashMap<String,AudioEntry>();

            LoadSettingsProperties();
            LoadSettingsJson();

            System.out.println("In LoadSettings using key " + SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key);
            System.out.println("In LoadSettings using key " + SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);
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

        AudioUtterancesMap.put("MultipleUtteranceEnglish", new AudioEntry()
        {{
            FilePath=System.getProperty("MultipleUtteranceWaveFile");
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

    public static String GetRootRelativePath(String input) {
        String rrPath = input;

        System.out.println("Fetching root relative path for " + input);
        try {
            rrPath = (DefaultSettingsMap.get(DefaultSettingsKeys.INPUT_DIR) + "/" + input).toString();
            File tempFile = new File(rrPath);

            if(!tempFile.exists()) {
                System.out.println("Android detected using base path " + System.getProperty("JsonConfigPath", "/data/local/tmp/"));
                System.out.println("Setting rrPath to " + (System.getProperty("JsonConfigPath", "/data/local/tmp/") + "/" + input).toString());
                rrPath = (System.getProperty("JsonConfigPath", "/data/local/tmp/") + "/" + input).toString();

                tempFile = new File(rrPath);
                if(!tempFile.exists())
                {
                    System.out.println("Failed to find a root path");
                    return input;
                }
            }
        } catch (Exception ex) {
            System.out.println("Failed to construct Root Relative Path");
        }

        return rrPath;
    }
}
