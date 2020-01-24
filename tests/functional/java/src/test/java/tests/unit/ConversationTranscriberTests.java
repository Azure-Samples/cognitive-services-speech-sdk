//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.net.URI;
import java.util.ArrayList;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.atomic.AtomicBoolean;

import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.transcription.ConversationTranscriber;
import com.microsoft.cognitiveservices.speech.transcription.Participant;
import com.microsoft.cognitiveservices.speech.transcription.User;
import com.microsoft.cognitiveservices.speech.transcription.Conversation;

import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import tests.AudioUtterancesKeys;
import tests.DefaultSettingsKeys;
import tests.Settings;
import tests.SubscriptionsRegionsKeys;

public class ConversationTranscriberTests {
    private static String inroomEndpoint = "";
    private static String onlineEndpoint = "";

    // Copy the Signature value from the Response body after calling the RESTFUL API at https://signature.centralus.cts.speech.microsoft.com
    // Voice signature format example: { "Version": <Numeric value>, "Tag": "string", "Data": "string" }
    // [SuppressMessage("Microsoft.Security", "CS002:SecretInNextLine", Justification="There are no secrets here")]
    private String voiceSignatureKatie = "{ \"Version\": 0, \"Tag\": \"VtZQ7sJp8np3AxQC+87WYyBHWsZohWFBN0zgWzzOnpw=\", \"Data\": \"BhRRgDCrg6ij5fylg5Jpf5ZW/o/uDWi199DYBbfL1Qdspj77qiuvsVKzG2g5Z9jxKtfdwtkKtaDxog9O6pGD7Ot/8mM1jUtt6LKNz4H9EFvznV/dlFk2Oisg8ZKx/RBlNFMYJkQJnxT/zLfhNWiIF5Y97jH4sgRh2orDg6/567FGktAgcESAbiDx1e7tf0TTLdwijw4p1vJ3qJ2cSCdNbXE9KeUd8sClQLDheCPo+et3rMs5W+Rju3W1SJE6ru9gAoH88CyAfI80+ysAecH3GPJYM+j1uhvmWoKIrSfS40BYOe6AUgLNb3a4Pue4oGAmuAyWfwpP1uezleSanpJc73HT91n2UsaHrQj5eK6uuBCjwmu+JI3FT+Vo6JhAARHecdb70U1wyW9vv5t0Q3tV1WNiN/30qSVydDtyrSVpgBiIwlj41Ua22JJCOMrPl7NKLnFmeZ4Hi4aIKoHAxDvrApteL60sxLX/ADAtYCB3Y6iagDyR1IOsIlbaPhL0rQDnC/0z65k7BDekietFNzvvPVoIwJ26GHrXFYYwZe3alVvCsXTpZGBknvSiaCalrixnyGqYo0nG/cd/LodEEIht/PWoFkNlbABqHMbToeI/6j+ICKuVJgTDtcsDQiWKSvrQp9kTSv+cF3LyPVkbks0JvbQhj4AkAv7Rvymgnsu6o8gGyxTX0kxujqCMPCzgFrVd\"}";
    private String voiceSignatureSteve = "{ \"Version\": 0, \"Tag\": \"HbIvzbfAWjeR/3R+WvUEoeid1AbDaHNOMWItgs7mTxc=\", \"Data\": \"DizY04Z7PH/sYu2Yw2EcL4Mvj1GnEDOWJ/DhXHGdQJsQ8/zDc13z1cwllbEo5OSr3oGoKEHLV95OUA6PgksZzvTkf42iOFEv3yifUNfYkZuIzStZoDxWu1H1BoFBejqzSpCYyvqLwilWOyUeMn+z+E4+zXjqHUCyYJ/xf0C3+58kCbmyA55yj7YZ6OtMVyFmfT2GLiXr4YshUB14dgwl3Y08SRNavnG+/QOs+ixf3UoZ6BC1VZcVQnC2tn2FB+8v6ehnIOTQedo++6RWIB0RYmQ8VaEeI0E4hkpA1OxQ9f2gBVtw3KZXWSWBz8sXig2igpwMsQoFRmmIOGsu+p6tM8/OThQpARZ7OyAxsurzmaSGZAaXYt0YwMdIIXKeDBF6/KnUyw+NNzku1875u2Fde/bxgVvCOwhrLPPuu/RZUeAkwVQge7nKYNW5YjDcz8mfg4LfqWEGOVCcmf2IitQtcIEjY3MwLVNvsAB6GT2es1/1QieCfQKy/Tdu8IUfEvekwSCxSlWhfVrLjRhGeWa9idCjsngQbNkqYUNdnIlidkn2DC4BavSTYXR5lVxV4SR/Vvj8h4N5nP/URPDhkzl7n7Tqd4CGFZDzZzAr7yRo3PeUBX0CmdrKLW3+GIXAdvpFAx592pB0ySCv5qBFhJNErEINawfGcmeWZSORxJg1u+agj51zfTdrHZeugFcMs6Be\"}";

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        boolean isMac = operatingSystem.contains("mac") || operatingSystem.contains("darwin");
        org.junit.Assume.assumeFalse(isMac);
        Settings.LoadSettings();
        inroomEndpoint = Settings.DefaultSettingsMap.get(DefaultSettingsKeys.CONVERSATION_TRANSCRIPTION_ENDPOINT) + "/multiaudio";
        onlineEndpoint = "wss://westus2.online.princetondev.customspeech.ai/recognition/onlinemeeting/v1";
    }

    // -----------------------------------------------------------------------
    // ---
    // -----------------------------------------------------------------------

    @Ignore
    @Test
    public void testDispose() {
        // TODO: make dispose method public
        fail("dispose not yet public");
    }

    @Test
    public void testConversationIdWithAnsiOnly() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Key,
            Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.UNIFIED_SPEECH_SUBSCRIPTION).Region);

        assertNotNull(s);
        String myConversationId = "123 456";

        Conversation conversation = CreateConversation(s, myConversationId);
        assertNotNull(conversation);
        assertNotNull(conversation.getConversationImpl());

        String gotId = conversation.getConversationId();
        assertEquals(myConversationId, gotId);
    }

    @Test
    public void testConversationCreateUsers() {
        String myId = "xyz@example.com";
        User user = User.fromUserId(myId);
        assertEquals(myId, user.getId());
    }

    @Test
    public void testConversationCreateParticipants() {
        Participant a = Participant.from("xyz@example.com");
        a.setPreferredLanguage("zh-cn");
        boolean exception = false;
        try {
            a.setPreferredLanguage("");
        }
        catch (Exception ex) {
            System.out.println("Got Exception in setPreferredLanguage:" + ex.toString());
            exception = true;
        }
        assertEquals(exception, true);

        // Set invalid voice signature.
        exception = false;
        try {
            a.setVoiceSignature("1.1, 2.2. expecting throwing an exception here");
        }
        catch (Exception ex) {
            System.out.println("Got Exception in setVoiceSignature:" + ex.toString());
            exception = true;
        }
        assertEquals(exception, true);
    }

    @Test
    public void testConversationAddParticipant() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION).Key);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH).FilePath));
        assertNotNull(ais);

        String conversationId = "test_from_cts_add_participant_in_java";

        Conversation conversation = CreateConversation(s, conversationId);
        assertNotNull(conversation);
        assertNotNull(conversation.getConversationImpl());

        ConversationTranscriber ct = new ConversationTranscriber(AudioConfig.fromStreamInput(ais));
        assertNotNull(ct);
        assertNotNull(ct.getTranscriberImpl());
        assertTrue(ct instanceof Recognizer);

        // Wait for join the conversation for max 30 seconds
        Future<?> future = ct.joinConversationAsync(conversation);
        assertNotNull(future);
        future.get(30, TimeUnit.SECONDS);

        Future<?> add_future = conversation.addParticipantAsync("OneUserByUserId");
        assertNotNull(add_future);
        future.get(30, TimeUnit.SECONDS);

        User user = User.fromUserId("CreateUserFromId and then add it ");
        conversation.addParticipantAsync(user).get();

        // Voice signature format as specified here https://aka.ms/cts/signaturegenservice
        Participant participant = Participant.from("userIdForParticipant", "en-us", voiceSignatureKatie);
        conversation.addParticipantAsync(participant);

        boolean found = false;
        ArrayList<String> phrases = getTranscriberResult(ct);
        for( String phrase: phrases) {
          System.out.println("phrase comes back from CTS is '" + phrase + "'");
          String cleanedPhrase = phrase.replaceAll("[^a-zA-Z0-9 ]", "");
          String reference = Settings.AudioUtterancesMap.get(AudioUtterancesKeys.CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH).Utterances.get("en-US")[0].Text.replaceAll("[^a-zA-Z0-9 ]", "").toLowerCase();
          System.out.println("cleaned version of Settings.TwoSpeakersAudioUtterance is '" + reference + "'");
          if(cleanedPhrase.toLowerCase().startsWith(reference) )  {
              found = true;
              break;
           }
        }
        System.out.println("found is " + found);
        assertEquals(found, true);

        ct.close();
        conversation.close();        
    }

    @Test
    public void testRemoveParticipant() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION).Key);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH).FilePath));
        assertNotNull(ais);

        String conversationId = "test_from_cts_remove_participant_in_java";

        Conversation conversation = CreateConversation(s, conversationId);
        assertNotNull(conversation);
        assertNotNull(conversation.getConversationImpl());

        ConversationTranscriber ct = new ConversationTranscriber(AudioConfig.fromStreamInput(ais));
        assertNotNull(ct);
        assertNotNull(ct.getTranscriberImpl());
        assertTrue(ct instanceof Recognizer);

        // Wait for join the conversation for max 30 seconds
        Future<?> future = ct.joinConversationAsync(conversation);
        assertNotNull(future);
        future.get(30, TimeUnit.SECONDS);

        Boolean exception = false;
        try {
            conversation.removeParticipantAsync("NoneExist_AnExceptionIsExpected").get();
        }
        catch (Exception ex) {
            System.out.println("Got Exception in removeParticipant:" + ex.toString());
            exception = true;
        }
        assertEquals(exception, true);

        conversation.addParticipantAsync("OneUserByUserId").get();
        conversation.removeParticipantAsync("OneUserByUserId").get();

        User user = User.fromUserId("User object created from User.FromUserId");
        conversation.addParticipantAsync(user).get();

        boolean found = false;
        ArrayList<String> phrases = getTranscriberResult(ct);
        for( String phrase: phrases) {
          System.out.println("phrase comes back from CTS is '" + phrase + "'");
          String cleanedPhrase = phrase.replaceAll("[^a-zA-Z0-9 ]", "");
          String reference = Settings.AudioUtterancesMap.get(AudioUtterancesKeys.CONVERSATION_BETWEEN_TWO_PERSONS_ENGLISH).Utterances.get("en-US")[0].Text.replaceAll("[^a-zA-Z0-9 ]", "").toLowerCase();
          System.out.println("cleaned version of Settings.TwoSpeakersAudioUtterance is '" + reference + "'");
          if(cleanedPhrase.toLowerCase().startsWith(reference) )  {
              found = true;
              break;
           }
        }
        System.out.println("found is " + found);
        assertEquals(found, true);

        ct.close();
        conversation.close();
    }

    /* We knew Android http delete for some reason does not work.
     * Under Windows, I got a 500 error as well.
     */
    @Ignore
    @Test
    public void testEndConversation() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(onlineEndpoint), Settings.SubscriptionsRegionsMap.get(SubscriptionsRegionsKeys.CONVERSATION_TRANSCRIPTION_PPE_SUBSCRIPTION).Key);
        assertNotNull(s);
        s.setProperty("iCalUid", "040000008200E00074C5B7101A82E008000000001003D722197CD4010000000000000000100000009E970FDF583F9D4FB999E607891A2F66");

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.GetRootRelativePath(Settings.AudioUtterancesMap.get(AudioUtterancesKeys.SINGLE_UTTERANCE_ENGLISH).FilePath));
        assertNotNull(ais);

        String conversationId = "test_from_end_conversation_in_java";

        Conversation conversation = CreateConversation(s, conversationId);
        assertNotNull(conversation);
        assertNotNull(conversation.getConversationImpl());

        ConversationTranscriber ct = new ConversationTranscriber(AudioConfig.fromStreamInput(ais));
        assertNotNull(ct);
        assertNotNull(ct.getTranscriberImpl());
        assertTrue(ct instanceof Recognizer);

        // Wait for join the conversation for max 30 seconds
        Future<?> future = ct.joinConversationAsync(conversation);
        assertNotNull(future);
        future.get(30, TimeUnit.SECONDS);

        conversation.addParticipantAsync("UserFromEndConversationTest").get();

        boolean found = false;
        ArrayList<String> phrases = getTranscriberResult(ct);
        for( String phrase: phrases) {
          System.out.println("phrase comes back from CTS is '" + phrase + "'");
          if(phrase.toLowerCase().startsWith("what's the weather like") )  {
              found = true;
           }
        }
        assertEquals(found, true);

        boolean exception = false;
        try {
            conversation.endConversationAsync().get();
        }
        catch(Exception ex){
            exception = true;
        }
        assertEquals(exception, false);

        // the second time, it throws an exception.
        exception = false;
        try {
            conversation.endConversationAsync().get();
        }
        catch(Exception ex){
            exception = true;
        }
        assertEquals(exception, true);

        ct.close();
        conversation.close();
    }

    private ArrayList<String> getTranscriberResult(ConversationTranscriber ct) throws InterruptedException, ExecutionException, TimeoutException {
        String result;
        final ArrayList<String> phrases = new ArrayList<>();

        AtomicBoolean allDone = new AtomicBoolean(false);

        ct.transcribing.addEventListener((o, e) -> {
            //System.out.println("Conversation transcriber transcibing:" + e.toString());
        });

        ct.transcribed.addEventListener((o, e) -> {
            phrases.add(e.getResult().getText());
            System.out.println("Conversation transcriber transcribed:" + e.toString());
        });

        // SDK does not send out sessionstopped when there is an error.
        ct.canceled.addEventListener((o, e) -> {
            if (e.getReason() != CancellationReason.EndOfStream)
            {
                allDone.set(true);
                System.out.println("Conversation transcriber canceled:" + e.toString());
            }
        });

        ct.sessionStopped.addEventListener((o, e) -> {
            allDone.set(true);
            System.out.println("Conversation transcriber stopped:" + e.toString());
        });

        ct.startTranscribingAsync().get();

        // wait for 300 seconds for sessionStopped or cancelled, othewise we time out
        int timeoutInMillis = 300000;
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < timeoutInMillis) && (allDone.get() == false ) ) {
            Thread.sleep(200);
        }

        if( allDone.get() == false) {
            System.out.println("timeout!" + timeoutInMillis);
    }

        ct.stopTranscribingAsync().get();
        return phrases;
    }

    private Conversation CreateConversation(SpeechConfig speech_config, String id) throws InterruptedException, ExecutionException, TimeoutException {
        assertNotNull(speech_config);

        speech_config.setProperty("ConversationTranscriptionInRoomAndOnline", "true");

        Conversation conversation = Conversation.createConversationAsync(speech_config, id).get();
        assertNotNull(conversation);
        assertNotNull(conversation.getConversationImpl());

        return conversation;
    }
}
