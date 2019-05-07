//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.endtoend;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.BeforeClass;
import org.junit.Test;

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.translation.SpeechTranslationConfig;
import com.microsoft.cognitiveservices.speech.translation.TranslationRecognizer;

import tests.Settings;

public class RecognizerTests {
    static SpeechConfig speechConfig;
    static SpeechTranslationConfig translationConfig;
    static AudioConfig audioConfig;

    @BeforeClass
    public static void setUpBeforeClass() {
        Settings.LoadSettings();
        speechConfig = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        translationConfig = SpeechTranslationConfig.fromSubscription(Settings.SpeechSubscriptionKey,Settings.SpeechRegion);
        translationConfig.addTargetLanguage("de");
        translationConfig.setSpeechRecognitionLanguage("en-US");
        audioConfig = AudioConfig.fromWavFileInput(Settings.WavFile);
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
        speechConfig = SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
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
        speechConfig = SpeechConfig.fromSubscription(Settings.LuisSubscriptionKey, Settings.LuisRegion);
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
