//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//

package tests.unit;

import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.assertEquals;

import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.transcription.*;
import com.microsoft.cognitiveservices.speech.transcription.Conversation;
import com.microsoft.cognitiveservices.speech.translation.*;
import com.microsoft.cognitiveservices.speech.PropertyId;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeoutException;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;
import org.junit.Rule;
import tests.AudioUtterancesKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;
import tests.Retry;

public class ConversationTranslatorTests {

    @Rule
    public Retry retry = new Retry(Settings.TestRetryCount);

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        boolean isMac = operatingSystem.contains("mac") || operatingSystem.contains("darwin");
        org.junit.Assume.assumeFalse(isMac);
        Settings.LoadSettings();
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Test
    public void testJoinFailure() {
        log("testJoinFailure()");
        AudioConfig audioConfig = getAudioConfig();

        //audioConfig.setProperty(PropertyId.Speech_LogFilename, "carbonlog.txt");

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        try {
            ct.joinConversationAsync("_", "testJoinFailure", "en-US").get();

            log("testJoinFailure no exception. ");
            assertTrue("didn't get an exception when joining invalid room code", false);

        } catch (InterruptedException ex) {
            ex.printStackTrace();
            assertTrue("interrupted exception " + ex.getMessage(), false);
        } catch (ExecutionException ex) {
            ex.printStackTrace();
            //expecting a message like HTTP 400: {"error":{"code":400027,"message":"The room parameter is not valid."}}
            assertTrue("unexpected execution exception " + ex.getMessage(), ex.getMessage().contains("400027"));
        } finally {
            ct.close();
            audioConfig.close();
        }
        log("testJoinFailure complete. ");

    }

    @Test
    public void testSendFailure() {
        log("testSendFailure()");
        AudioConfig audioConfig = getAudioConfig();

        //audioConfig.setProperty(PropertyId.Speech_LogFilename, "carbonlog.txt");

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        try {
            ct.sendTextMessageAsync("a message").get();

            log("testSendFailure no exception. ");
            assertTrue("didn't get an exception when sending message to uninitialized", false);

        } catch (InterruptedException ex) {
            ex.printStackTrace();
            assertTrue("interrupted exception " + ex.getMessage(), false);
        } catch (ExecutionException ex) {
            //ex.printStackTrace();
            //expecting a message like: Exception with an error code: 0x1 (SPXERR_UNINITIALIZED)
            assertTrue("unexpected execution exception " + ex.getMessage(), ex.getMessage().contains("SPXERR_UNINITIALIZED"));
        } finally {
            ct.close();
            audioConfig.close();
        }
        log("testSendFailure complete. ");

    }

    @Test
    public void testStartTranscribeFailure() {
        log("testStartTranscribeFailure()");
        AudioConfig audioConfig = getAudioConfig();

        //audioConfig.setProperty(PropertyId.Speech_LogFilename, "carbonlog.txt");

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        try {
            ct.startTranscribingAsync().get();

            log("testStartTranscribeFailure no exception. ");
            assertTrue("didn't get an exception when starting transcribing uninitialized", false);

        } catch (InterruptedException ex) {
            ex.printStackTrace();
            assertTrue("interrupted exception " + ex.getMessage(), false);
        } catch (ExecutionException ex) {
            //ex.printStackTrace();
            //expecting a message like: Exception with an error code: 0x1 (SPXERR_UNINITIALIZED)
            assertTrue("unexpected execution exception " + ex.getMessage(), ex.getMessage().contains("SPXERR_UNINITIALIZED"));
        } finally {
            ct.close();
            audioConfig.close();
        }
        log("testStartTranscribeFailure complete. ");

    }

    @Test
    public void testStopTranscribeFailure() {
        log("testStopTranscribeFailure()");
        AudioConfig audioConfig = getAudioConfig();

        //audioConfig.setProperty(PropertyId.Speech_LogFilename, "carbonlog.txt");

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        try {
            ct.stopTranscribingAsync().get();

            log("testStopTranscribeFailure no exception. ");
            assertTrue("didn't get an exception when stopping transcribing uninitialized", false);

        } catch (InterruptedException ex) {
            ex.printStackTrace();
            assertTrue("interrupted exception " + ex.getMessage(), false);
        } catch (ExecutionException ex) {
            //ex.printStackTrace();
            //expecting a message like: Exception with an error code: 0x1 (SPXERR_UNINITIALIZED)
            assertTrue("unexpected execution exception " + ex.getMessage(), ex.getMessage().contains("SPXERR_UNINITIALIZED"));
        } finally {
            ct.close();
            audioConfig.close();
        }
        log("testStopTranscribeFailure complete. ");

    }

    @Test
    public void testCreateSendTextTranscribe()
            throws InterruptedException, ExecutionException, TimeoutException {
        log("testCreateSendTextTranscribe()");
        AudioConfig audioConfig = getAudioConfig();

        Conversation conv = createConversation(getDefaultConfig());

        ConversationTranslator ct = new ConversationTranslator(audioConfig);

        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        ct.joinConversationAsync(conv, "IamHost").get();

        sleep(5000);

        assertTrue("no callbacks received after join ", cth.items.size() > callbackCount);
        callbackCount = cth.items.size();

        ct.sendTextMessageAsync("can you see this text message?").get();

        sleep(5000);

        log("callbacks received after sent text " + cth.items.size());
        callbackCount = cth.items.size();

        ct.startTranscribingAsync().get();

        sleep(5000);

        log("callbacks received after start transcribing " + cth.items.size());

        ct.stopTranscribingAsync().get();

        sleep(5000);

        log("callbacks received after stop transcribing " + cth.items.size());

        assertTrue(
                "no callbacks received after transcribing audio ",
                cth.items.size() > callbackCount);
        callbackCount = cth.items.size();

        ct.leaveConversationAsync().get();

        ct.close();
        conv.close();
        audioConfig.close();

        sleep(5000);

        log("testCreateSendTextTranscribe complete. " + cth.size());

        cleanup();
    }

    @Test
    public void testSpeechRecogLanguage1() 
                throws InterruptedException, ExecutionException, TimeoutException {
        log("testSpeechRecogLanguage()");
        String lang = "de-DE";

        AudioConfig audioConfig = getAudioConfig();

        SpeechTranslationConfig config = getDefaultConfig();
        config.setSpeechRecognitionLanguage(lang);
        Conversation conv = createConversation(config);

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        log(" lang0 " + ct.getSpeechRecognitionLanguage());

        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        ct.joinConversationAsync(conv, "IamHost").get();

        sleep(5000);

        log(" lang1 " + ct.getSpeechRecognitionLanguage());

        assertEquals(ct.getSpeechRecognitionLanguage(), lang);
        ct.close();
        conv.close();

        log("testSpeechRecogLanguage complete. " + cth.size());

        cleanup();
    }

    @Test
    public void testSpeechRecogLanguage2() 
                throws InterruptedException, ExecutionException, TimeoutException 
    {
        log("testSpeechRecogLanguage2()");
        String lang = "ja-JP";

        AudioConfig audioConfig = getAudioConfig();

        SpeechTranslationConfig config = getDefaultConfig();
        config.setSpeechRecognitionLanguage(lang);
        Conversation conv = createConversation(config);

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        log(" lang0 " + ct.getSpeechRecognitionLanguage());

        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        ct.joinConversationAsync(conv, "IamHost").get();

        sleep(5000);

        log(" lang1 " + ct.getSpeechRecognitionLanguage());

        assertEquals(ct.getSpeechRecognitionLanguage(), lang);
        ct.close();
        conv.close();

        log("testSpeechRecogLanguage2 complete. " + cth.size());

        cleanup();
    }

    @Test
    public void testJoinNamed() {
        log("testJoinNamed()");

        String conversationId = "";
        String propertyName = "ExistingConversationId";
        conversationId = System.getProperty(propertyName);

        log("found property " + conversationId);

        if (conversationId == null || conversationId.length() == 0) {
            log("Skipping test. conversation id not set, set property '" + propertyName + "'");
            return;
        }

        AudioConfig audioConfig = getAudioConfig();

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        try {

            ct.joinConversationAsync(conversationId, "testJoinNamed", "en-US").get();

            sleep(5000);

            assertTrue("no callbacks received after join ", cth.items.size() > callbackCount);
            callbackCount = cth.items.size();

            ct.sendTextMessageAsync("can you see this text message?").get();

            sleep(5000);

            log("callbacks received after sent text " + cth.items.size());
            callbackCount = cth.items.size();

            ct.startTranscribingAsync().get();

            sleep(5000);

            log("callbacks received after start transcribing " + cth.items.size());

            ct.stopTranscribingAsync().get();

            sleep(5000);

            log("callbacks received after stop transcribing " + cth.items.size());

            assertTrue(
                    "no callbacks received after transcribing audio ",
                    cth.items.size() > callbackCount);
            callbackCount = cth.items.size();

            ct.leaveConversationAsync().get();

            sleep(5000);

            log("callbacks received after stop leaving " + cth.items.size());

            assertTrue(
                    "no callbacks received after leave conversation ",
                    cth.items.size() > callbackCount);
            callbackCount = cth.items.size();

        } catch (InterruptedException ex) {
            ex.printStackTrace();
            assertTrue("interrupted exception " + ex.getMessage(), false);
        } catch (ExecutionException ex) {
            ex.printStackTrace();
            assertTrue("execution exception " + ex.getMessage(), false);
        } finally {
            ct.close();
        }
        log("testJoinNamed complete. " + callbackCount);
    }

    @Test
    public void testJoinLeaveExisting()
            throws InterruptedException, ExecutionException, TimeoutException {
        log("testJoinLeaveExisting()");

        String conversationId = "";
        String propertyName = "ExistingConversationId";
        conversationId = System.getProperty(propertyName);

        log("found property " + conversationId);

        if (conversationId == null || conversationId.length() == 0) {
            log("Skipping test. conversation id not set, set property '" + propertyName + "'");
            return;
        }

        AudioConfig audioConfig = getAudioConfig();

        ConversationTranslator ct = new ConversationTranslator(audioConfig);
        ConversationTranslatorHelper cth = ConversationTranslatorHelper.instance(ct);
        int callbackCount = 0;

        ct.joinConversationAsync(conversationId, "testJoinLeaveExisting", "en-US").get();

        sleep(5000);

        assertTrue("no callbacks received after join ", cth.items.size() > callbackCount);
        callbackCount = cth.items.size();

        ct.sendTextMessageAsync("can you see this text message?").get();

        sleep(5000);

        log("callbacks received after sent text " + cth.items.size());
        callbackCount = cth.items.size();

        ct.leaveConversationAsync().get();

        sleep(5000);

        assertTrue(
                "no callbacks received after leave conversation ",
                cth.items.size() > callbackCount);
        callbackCount = cth.items.size();

        log("testJoinLeaveExisting complete. " + cth.size());
    }

    @Test
    public void testCallbacks2Participants()
            throws InterruptedException, ExecutionException, TimeoutException {
        log("testCallbacks2Participants()");

        String arch = System.getProperty("os.arch");

        if (!"amd64".equalsIgnoreCase(arch)) {
            log("skipping test, not sure if " + arch + " is supported");
            return;
        }

        AudioConfig audioConfig1 = getAudioConfig();
        Conversation conv = createConversation(getDefaultConfig());

        ConversationTranslator ct1 = new ConversationTranslator(audioConfig1);

        ConversationTranslatorHelper cth1 = ConversationTranslatorHelper.instance(ct1, "host  ");
        int callbackCount1 = 0;

        ct1.joinConversationAsync(conv, "CallbackHost").get();

        sleep(3000);

        assertTrue("no host callbacks received after join " + cth1.size(), cth1.size() > callbackCount1);
        callbackCount1 = cth1.size();

        String conversationId = conv.getConversationId();
        log("created conversation is " + conversationId + " trying to join with 2nd participant, callback count " + cth1.size());
        AudioConfig audioConfig2 = getAudioConfig();

        ConversationTranslator ct2 = new ConversationTranslator(audioConfig2);
        ConversationTranslatorHelper cth2 = ConversationTranslatorHelper.instance(ct2, "guest ");

        ct2.joinConversationAsync(conversationId, "SecondParticipant", "en-US").get();
        sleep(3000);

        assertTrue("no host callbacks received after 'SecondParticipant' join " + cth1.size(), cth1.size() > callbackCount1);
        callbackCount1 = cth1.size();

        ct2.sendTextMessageAsync("text from participant.").get();

        sleep(3000);

        assertTrue("no host callbacks received after 'SecondParticipant' text sent " + cth1.size(), cth1.size() > callbackCount1);
        callbackCount1 = cth1.size();

        ct2.startTranscribingAsync().get();

        sleep(5000);

        ct2.stopTranscribingAsync().get();

        sleep(3000);

        assertTrue("no host callbacks received after 'SecondParticipant' transcribe " + cth1.size(), cth1.size() > callbackCount1);
        callbackCount1 = cth1.size();

        int callbackCount2 = cth2.items.size();

        ct1.sendTextMessageAsync("can you see this text message?").get();

        sleep(3000);

        assertTrue("no guest callbacks on second participant after 'host' sent text" + cth2.items.size(), cth2.items.size() > callbackCount2);
        callbackCount2 = cth2.items.size();

        log("host callbacks received after sent text " + cth1.size());
        callbackCount1 = cth1.size();

        ct1.startTranscribingAsync().get();

        sleep(5000);

        log("host callbacks received after start transcribing host " + cth1.size());
        log("guest callbacks received after start transcribing host " + cth2.size());

        ct1.stopTranscribingAsync().get();

        sleep(3000);

        log("host callbacks received after stop transcribing host " + cth1.size());
        log("guest callbacks received after start transcribing host " + cth2.size());

        assertTrue(
                "no host callbacks received after transcribing host audio " + cth1.size(),
                cth1.size() > callbackCount1);
        callbackCount1 = cth1.size();

        assertTrue(
            "no guest callbacks received after transcribing host audio " + cth2.size(),
            cth2.size() > callbackCount2);

        ct1.leaveConversationAsync().get();

        ct1.close();
        conv.close();
        audioConfig1.close();

        ct2.close();
        audioConfig2.close();

        sleep(3000);

        log("testCallbacks2Participants complete. " + cth1.size());

        cleanup();

        sleep(2000);
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    private void log(String s) {
        System.out.println("ConversationTranslatorTest: " + s);
    }

    private void cleanup() {
        System.gc();
    }

    private void sleep(long millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException ex) {
            assertTrue("sleep interrupted", false);
        }
    }

    SpeechTranslationConfig getDefaultConfig() {
        return getConfig(SubscriptionsRegionsKeys.CONVERSATION_TRANSLATOR_SUBSCRIPTION);
    }

    SpeechTranslationConfig getConfig(String type) {
        SpeechTranslationConfig config =
                SpeechTranslationConfig.fromSubscription(
                        Settings.SubscriptionsRegionsMap.get(type).Key,
                        Settings.SubscriptionsRegionsMap.get(type).Region);

        assertNotNull(config);

        // config.setProperty(PropertyId.Speech_LogFilename, "carbonlog.txt");

        config.setSpeechRecognitionLanguage("en-US");
        config.addTargetLanguage("en-US");
        config.addTargetLanguage("de-DE");
        config.addTargetLanguage("ja-JP");

        return config;
    }

    private AudioConfig getAudioConfig() {
        String filename =
                Settings.GetRootRelativePath(
                        Settings.AudioUtterancesMap.get(
                                        AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH)
                                .FilePath);
        log(" Audioconfig from " + filename);
        return AudioConfig.fromWavFileInput(filename);
    }

    private Conversation createConversation(SpeechConfig speech_config)
            throws InterruptedException, ExecutionException, TimeoutException {
        return createConversation(speech_config, null);
    }

    private Conversation createConversation(SpeechConfig speech_config, String id)
            throws InterruptedException, ExecutionException, TimeoutException {
        assertNotNull(speech_config);

        // speech_config.setProperty("ConversationTranscriptionInRoomAndOnline", "true");

        Conversation conversation;
        if (id == null || id.length() == 0) {
            conversation = Conversation.createConversationAsync(speech_config).get();
        } else {
            conversation = Conversation.createConversationAsync(speech_config, id).get();
        }
        assertNotNull(conversation);
        assertNotNull(conversation.getImpl());

        String cid = conversation.getConversationId();
        assertTrue(
                "created conversation id is null or empty '" + cid + "'",
                cid != null && cid.length() > 0);

        log("Created conversation '" + conversation.getConversationId() + "'");

        conversation.startConversationAsync().get();

        log("started conversation '" + conversation.getConversationId() + "'");

        return conversation;
    }
}
