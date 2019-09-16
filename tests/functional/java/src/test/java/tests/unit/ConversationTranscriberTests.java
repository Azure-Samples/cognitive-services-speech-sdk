//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.net.URI;
import java.util.ArrayList;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import com.microsoft.cognitiveservices.speech.Recognizer;
import com.microsoft.cognitiveservices.speech.CancellationReason;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.conversation.ConversationTranscriber;
import com.microsoft.cognitiveservices.speech.conversation.Participant;
import com.microsoft.cognitiveservices.speech.conversation.User;
import com.microsoft.cognitiveservices.speech.conversation.ResourceHandling;

import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import tests.Settings;

public class ConversationTranscriberTests {
    private static String inroomEndpoint = "";

    // Copy the Signature value from the Response body after calling the RESTFUL API at https://signature.centralus.cts.speech.microsoft.com
    // Voice signature format example: { "Version": <Numeric value>, "Tag": "string", "Data": "string" }
    private String voiceSignatureKatie = "{ \"Version\": 0, \"Tag\": \"VtZQ7sJp8np3AxQC+87WYyBHWsZohWFBN0zgWzzOnpw=\", \"Data\": \"BhRRgDCrg6ij5fylg5Jpf5ZW/o/uDWi199DYBbfL1Qdspj77qiuvsVKzG2g5Z9jxKtfdwtkKtaDxog9O6pGD7Ot/8mM1jUtt6LKNz4H9EFvznV/dlFk2Oisg8ZKx/RBlNFMYJkQJnxT/zLfhNWiIF5Y97jH4sgRh2orDg6/567FGktAgcESAbiDx1e7tf0TTLdwijw4p1vJ3qJ2cSCdNbXE9KeUd8sClQLDheCPo+et3rMs5W+Rju3W1SJE6ru9gAoH88CyAfI80+ysAecH3GPJYM+j1uhvmWoKIrSfS40BYOe6AUgLNb3a4Pue4oGAmuAyWfwpP1uezleSanpJc73HT91n2UsaHrQj5eK6uuBCjwmu+JI3FT+Vo6JhAARHecdb70U1wyW9vv5t0Q3tV1WNiN/30qSVydDtyrSVpgBiIwlj41Ua22JJCOMrPl7NKLnFmeZ4Hi4aIKoHAxDvrApteL60sxLX/ADAtYCB3Y6iagDyR1IOsIlbaPhL0rQDnC/0z65k7BDekietFNzvvPVoIwJ26GHrXFYYwZe3alVvCsXTpZGBknvSiaCalrixnyGqYo0nG/cd/LodEEIht/PWoFkNlbABqHMbToeI/6j+ICKuVJgTDtcsDQiWKSvrQp9kTSv+cF3LyPVkbks0JvbQhj4AkAv7Rvymgnsu6o8gGyxTX0kxujqCMPCzgFrVd\"}";
    private String voiceSignatureSteve = "{ \"Version\": 0, \"Tag\": \"HbIvzbfAWjeR/3R+WvUEoeid1AbDaHNOMWItgs7mTxc=\", \"Data\": \"DizY04Z7PH/sYu2Yw2EcL4Mvj1GnEDOWJ/DhXHGdQJsQ8/zDc13z1cwllbEo5OSr3oGoKEHLV95OUA6PgksZzvTkf42iOFEv3yifUNfYkZuIzStZoDxWu1H1BoFBejqzSpCYyvqLwilWOyUeMn+z+E4+zXjqHUCyYJ/xf0C3+58kCbmyA55yj7YZ6OtMVyFmfT2GLiXr4YshUB14dgwl3Y08SRNavnG+/QOs+ixf3UoZ6BC1VZcVQnC2tn2FB+8v6ehnIOTQedo++6RWIB0RYmQ8VaEeI0E4hkpA1OxQ9f2gBVtw3KZXWSWBz8sXig2igpwMsQoFRmmIOGsu+p6tM8/OThQpARZ7OyAxsurzmaSGZAaXYt0YwMdIIXKeDBF6/KnUyw+NNzku1875u2Fde/bxgVvCOwhrLPPuu/RZUeAkwVQge7nKYNW5YjDcz8mfg4LfqWEGOVCcmf2IitQtcIEjY3MwLVNvsAB6GT2es1/1QieCfQKy/Tdu8IUfEvekwSCxSlWhfVrLjRhGeWa9idCjsngQbNkqYUNdnIlidkn2DC4BavSTYXR5lVxV4SR/Vvj8h4N5nP/URPDhkzl7n7Tqd4CGFZDzZzAr7yRo3PeUBX0CmdrKLW3+GIXAdvpFAx592pB0ySCv5qBFhJNErEINawfGcmeWZSORxJg1u+agj51zfTdrHZeugFcMs6Be\"}";

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
        String operatingSystem = ("" + System.getProperty("os.name")).toLowerCase();
        System.out.println("Current operation system: " + operatingSystem);
        boolean isMac = operatingSystem.contains("mac") || operatingSystem.contains("darwin");
        org.junit.Assume.assumeFalse(isMac);
        Settings.LoadSettings();
        inroomEndpoint = Settings.ConversationTranscriptionEndpoint + "/multiaudio";
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
    public void testConversationIdWithAnsiOnly() {
        SpeechConfig s = SpeechConfig.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
        assertNotNull(s);

        ConversationTranscriber t = new ConversationTranscriber(s, AudioConfig.fromWavFileInput(Settings.TwoSpeakersAudio));
        assertNotNull(t);
        assertNotNull(t.getTranscriberImpl());
        assertTrue(t instanceof Recognizer);

        String myConversationId = "123 456";
        t.setConversationId(myConversationId);
        String gotId = t.getConversationId();
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
            a.setVoiceSignature("1.1, 2.2");
        }
        catch (Exception ex) {
            System.out.println("Got Exception in setVoiceSignature:" + ex.toString());
            exception = true;
        }
        assertEquals(exception, true);
    }

    @Ignore
    @Test
    public void testConversationAddParticipant() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.ConversationTranscriptionPPEKey);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.TwoSpeakersAudio);
        assertNotNull(ais);

        ConversationTranscriber t = new ConversationTranscriber(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(t);
        assertNotNull(t.getTranscriberImpl());
        assertTrue(t instanceof Recognizer);

        t.setConversationId("TestCreatingParticipantByUserClass");
        t.addParticipant("OneUserByUserId");

        User user = User.fromUserId("CreateUserFromId and then add it ");
        t.addParticipant(user);

        // Voice signature format as specified here https://aka.ms/cts/signaturegenservice
        Participant participant = Participant.from("userIdForParticipant", "en-us", voiceSignatureKatie);
        t.addParticipant(participant);

        String result = getFirstTranscriberResult(t);
        assertEquals(Settings.TwoSpeakersAudioUtterance, result);

        t.close();
        t.close();
    }

    @Ignore
    @Test
    public void testRemoveParticipant() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.ConversationTranscriptionPPEKey);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.TwoSpeakersAudio);
        assertNotNull(ais);

        ConversationTranscriber t = new ConversationTranscriber(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(t);
        assertNotNull(t.getTranscriberImpl());
        assertTrue(t instanceof Recognizer);

        t.setConversationId("TestCreatingParticipantByUserClass");
        Boolean exception = false;
        try {
            t.removeParticipant("NoneExist");
        }
        catch (Exception ex) {
            System.out.println("Got Exception in removeParticipant:" + ex.toString());
            exception = true;
        }
        assertEquals(exception, true);

        t.addParticipant("OneUserByUserId");
        t.removeParticipant("OneUserByUserId");

        User user = User.fromUserId("User object created from User.FromUserId");
        t.addParticipant(user);
        t.removeParticipant(user);

        // Voice signature format as specified here https://aka.ms/cts/signaturegenservice
        Participant participant = Participant.from("userIdForParticipant", "en-us", voiceSignatureKatie);
        t.addParticipant(participant);
        t.removeParticipant(participant);

        String result = getFirstTranscriberResult(t);
        assertEquals(Settings.TwoSpeakersAudioUtterance, result);

        t.close();
        t.close();
    }

    @Test
     public void testStartAndStopConversationTranscribingAsyncDestroyResources() throws InterruptedException, ExecutionException, TimeoutException {
        testStartAndStopConversationTranscribingAsyncInternal(true);
    }

    @Test
    public void testStartAndStopConversationTranscribingAsyncKeepResources() throws InterruptedException, ExecutionException, TimeoutException {
        testStartAndStopConversationTranscribingAsyncInternal(false);
    }

    public void testStartAndStopConversationTranscribingAsyncInternal(boolean destroyResource) throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.ConversationTranscriptionPPEKey);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.TwoSpeakersAudio);
        assertNotNull(ais);

        ConversationTranscriber t = new ConversationTranscriber(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(t);
        assertNotNull(t.getTranscriberImpl());
        assertTrue(t instanceof Recognizer);

        t.setConversationId("ConversationPullStreamTest");

        t.addParticipant("xyz@example.com");

        String result = getFirstTranscriberResult(t, destroyResource);
        assertTrue(result != "");

        t.close();
        s.close();
    }

    private String getFirstTranscriberResult(ConversationTranscriber t) throws InterruptedException, ExecutionException, TimeoutException {
        return getFirstTranscriberResultInternal(t, true, true);
    }

    private String getFirstTranscriberResult(ConversationTranscriber t, boolean destroyResource) throws InterruptedException, ExecutionException, TimeoutException {
        return getFirstTranscriberResultInternal(t, false, destroyResource);
    }

    private String getFirstTranscriberResultInternal(ConversationTranscriber t, boolean useDefaultStopTranscribing, boolean destroyResource) throws InterruptedException, ExecutionException, TimeoutException {
        String result;
        final ArrayList<String> rEvents = new ArrayList<>();

        t.recognizing.addEventListener((o, e) -> {
            System.out.println("Conversation transcriber recognizing:" + e.toString());
        });

        t.recognized.addEventListener((o, e) -> {
            rEvents.add(e.getResult().getText());
            System.out.println("Conversation transcriber recognized:" + e.toString());
        });

        t.canceled.addEventListener((o, e) -> {
            if (e.getReason() != CancellationReason.EndOfStream)
            {
                rEvents.add("Canceled event:" + e.toString());
                System.out.println("Conversation transcriber canceled:" + e.toString());
            }
        });

        Future<?> future = t.startTranscribingAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        // wait until we get at least on final result
        long now = System.currentTimeMillis();
        while(((System.currentTimeMillis() - now) < 30000) &&
                (rEvents.isEmpty())) {
            Thread.sleep(200);
        }

        assertTrue(1 <= rEvents.size());
        result = rEvents.get(0);
        System.out.println("Result:" + result);
        assertFalse(result.contains("Canceled event:"));

        if (useDefaultStopTranscribing)
        {
            future = t.stopTranscribingAsync();
        }
        else
        {
            future = t.stopTranscribingAsync(destroyResource ? ResourceHandling.DestroyResources  : ResourceHandling.KeepResources);
        }
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        return result;
    }
}
