//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.FileNotFoundException;
import java.io.UnsupportedEncodingException;

import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.Rule;

import com.google.gson.JsonIOException;
import com.google.gson.JsonSyntaxException;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;

import tests.AudioUtterancesKeys;
import tests.BaseJUnit;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.Retry;

public class RecognizerTests extends BaseJUnit {
    static SpeechConfig speechConfig;
    static SpeechTranslationConfig translationConfig;
    static AudioConfig audioConfig;

    @Rule
    public Retry retry = new Retry(Settings.TestRetryCount);

    @BeforeClass
    public static void setUpBeforeClass() throws JsonIOException, JsonSyntaxException, FileNotFoundException, UnsupportedEncodingException {
        Settings.LoadSettings();
        speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key, 
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        translationConfig = SpeechTranslationConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        translationConfig.addTargetLanguage("de");
        translationConfig.setSpeechRecognitionLanguage("en-US");
        audioConfig = AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
    }

    @Test
    public void testAsyncRecognitionAfterClosingSpeechRecognizer() {
        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        recognizer.close();
        try {
            recognizer.stopContinuousRecognitionAsync().get();
            fail("expected to get IllegalStateException");
        } catch (Exception e) {
            assertTrue(e.getCause() instanceof IllegalStateException);
        }
    }

    @Test
    public void testClosingSpeechRecognizerWhileAsyncRecognition() {
        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, audioConfig);
        try {
            recognizer.recognizeOnceAsync();
            Thread.sleep(100);
            recognizer.close();
        } catch (Exception e) {
            assertTrue(e instanceof IllegalStateException);
        }
    }

    @Test
    public void testAsyncRecognitionAfterClosingTranslationRecognizer() {
        TranslationRecognizer recognizer = new TranslationRecognizer(translationConfig, audioConfig);
        recognizer.close();
        try {
            recognizer.startContinuousRecognitionAsync().get();
            fail("expected to get IllegalStateException");
        } catch (Exception e) {
            assertTrue(e.getCause() instanceof IllegalStateException);
        }
    }

    @Test
    public void testClosingTranslationRecognizerWhileAsyncRecognition() {
        TranslationRecognizer recognizer = new TranslationRecognizer(translationConfig, audioConfig);
        try {
            recognizer.recognizeOnceAsync();
            Thread.sleep(100);
            recognizer.close();
        } catch (Exception e) {
            assertTrue(e instanceof IllegalStateException);
        }
    }

    @Test
    public void testAsyncRecognitionAfterClosingIntentRecognizer() {
        speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        IntentRecognizer recognizer = new IntentRecognizer(translationConfig, audioConfig);
        recognizer.addIntent("dummy speech");
        recognizer.close();
        try {
            recognizer.recognizeOnceAsync().get();
            fail("expected to get IllegalStateException");
        } catch (Exception e) {
            assertTrue(e.getCause() instanceof IllegalStateException);
        }
    }

    @Test
    public void testClosingIntentRecognizerWhileAsyncRecognition() {
        speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.LANGUAGE_UNDERSTANDING_SUBSCRIPTION).Region);

        IntentRecognizer recognizer = new IntentRecognizer(translationConfig, audioConfig);
        recognizer.addIntent("Turn on the lamp.");
        try {
            recognizer.recognizeOnceAsync();
            Thread.sleep(100);
            recognizer.close();
            fail("expected to get IllegalStateException");
        } catch (Exception e) {
            assertTrue(e instanceof IllegalStateException);
        }
    }
}
