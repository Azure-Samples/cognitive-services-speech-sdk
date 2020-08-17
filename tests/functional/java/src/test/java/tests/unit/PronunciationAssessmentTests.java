//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import java.util.concurrent.ExecutionException;

import static org.junit.Assert.*;

import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.rules.ExpectedException;

import com.microsoft.cognitiveservices.speech.audio.*;

import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentConfig;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentGradingSystem;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentGranularity;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentResult;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import tests.AudioUtterancesKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.TestHelper;

public class PronunciationAssessmentTests {

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testPronunciationAssessmentConfig() {
        PronunciationAssessmentConfig pronConfig = new PronunciationAssessmentConfig("reference", PronunciationAssessmentGradingSystem.HundredMark, PronunciationAssessmentGranularity.Phoneme);
        assertNotNull(pronConfig);
        assertEquals("reference", pronConfig.getReferenceText());
        assertTrue(pronConfig.toJson().contains("HundredMark"));
        assertTrue(pronConfig.toJson().contains("Phoneme"));
        assertTrue(pronConfig.toJson().contains("Comprehensive"));
        assertTrue(pronConfig.toJson().contains("reference"));
        pronConfig.close();

        pronConfig = new PronunciationAssessmentConfig("reference", PronunciationAssessmentGradingSystem.HundredMark, PronunciationAssessmentGranularity.Word, true, "id");
        assertNotNull(pronConfig);
        pronConfig.setReferenceText("new reference");
        assertTrue(pronConfig.toJson().contains("HundredMark"));
        assertTrue(pronConfig.toJson().contains("Word"));

        PronunciationAssessmentConfig pronConfig2 = PronunciationAssessmentConfig.fromJson(pronConfig.toJson());
        assertNotNull(pronConfig2);
        assertEquals(pronConfig.toJson(), pronConfig2.toJson());
        
        pronConfig.close();
        pronConfig2.close();
    }

    @Test
    public void testPronunciationAssessmentConfigApplyTo() {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE).FilePath)));
        assertNotNull(recognizer);
        assertNotNull(recognizer.getRecoImpl());
        PronunciationAssessmentConfig pronConfig = new PronunciationAssessmentConfig("reference");
        assertNotNull(pronConfig);
        pronConfig.applyTo(recognizer);
        assertEquals(pronConfig.toJson(), recognizer.getProperties().getProperty(PropertyId.PronunciationAssessment_Params));

        pronConfig.close();
        recognizer.close();
        speechConfig.close();
    }

    @Test(expected = IllegalArgumentException.class)
    public void verifyInvalidJsonForPronunciationAssessmentConfig() throws ExecutionException {
        PronunciationAssessmentConfig pronConfig = PronunciationAssessmentConfig.fromJson("invalid json");
        
    }

    public void testPronunciationAssessment(boolean useReferenceText) throws InterruptedException, ExecutionException {
        SpeechConfig speechConfig = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);
        assertNotNull(speechConfig);
        speechConfig.setSpeechRecognitionLanguage("zh-CN");

        SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig, AudioConfig.fromWavFileInput(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE).FilePath)));
        assertNotNull(recognizer);
        assertNotNull(recognizer.getRecoImpl());
        PronunciationAssessmentConfig pronConfig = new PronunciationAssessmentConfig(
            useReferenceText ? Settings.AudioUtterancesMap.get(AudioUtterancesKeys.PRONUNCIATION_ASSESSMENT_GOOD_PRONUNCIATION_CHINESE).Utterances.get("zh-CN")[0].Text : "");
        assertNotNull(pronConfig);
        pronConfig.applyTo(recognizer);

        SpeechRecognitionResult res = recognizer.recognizeOnceAsync().get();
        assertNotNull(res);
        TestHelper.OutputResult(res);
        assertEquals(ResultReason.RecognizedSpeech, res.getReason());
        if (useReferenceText) {
            assertEquals(pronConfig.getReferenceText(), res.getText().replace("，", "").replace("。", ""));
        }

        PronunciationAssessmentResult pronResult = PronunciationAssessmentResult.fromResult(res);
        assertTrue(pronResult.getAccuracyScore() > 0);
        assertTrue(pronResult.getPronunciationScore() > 0);
        assertTrue(pronResult.getCompletenessScore() > 0);
        assertTrue(pronResult.getFluencyScore() > 0);

        res.close();
        pronConfig.close();
        speechConfig.close();
        recognizer.close();
    }

    @Ignore("TODO: enable this after pronunciation assessment is deployed on north europe.")
    @Test
    public void testPronunciationAssessmentWithReferenceText() throws InterruptedException, ExecutionException {
        testPronunciationAssessment(true);
    }
    
    @Test
    public void testPronunciationAssessmentWithoutReferenceText() throws InterruptedException, ExecutionException {
        testPronunciationAssessment(false);
    }
}