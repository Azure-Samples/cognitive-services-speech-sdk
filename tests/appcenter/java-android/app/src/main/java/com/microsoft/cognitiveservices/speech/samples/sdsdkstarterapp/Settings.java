// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

package com.microsoft.cognitiveservices.speech.samples.sdsdkstarterapp;

import com.microsoft.cognitiveservices.speech.SpeechConfig;

import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

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

        Settings.LoadSettings();
    };

    public static Map<String, String> DefaultSettingsMap;
    public static Map<String, SubscriptionRegion> SubscriptionsRegionsMap;
    public static Map<String, AudioEntry> AudioUtterancesMap;

    public static void LoadSettings() {
        if(!isSettingsInitialized)
        {
            DefaultSettingsMap = new HashMap<String,String>();
            SubscriptionsRegionsMap = new HashMap<String,SubscriptionRegion>();
            AudioUtterancesMap = new HashMap<String,AudioEntry>();

            LoadSettingsProperties();

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

        DefaultSettingsMap.put("WaveFile", System.getProperty("SampleAudioInput"));
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
