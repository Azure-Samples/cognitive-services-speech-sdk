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
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.conversation.ConversationTranscriber;
import com.microsoft.cognitiveservices.speech.conversation.Participant;
import com.microsoft.cognitiveservices.speech.conversation.User;

import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

import tests.Settings;

public class ConversationTranscriberTests {
    private static String inroomEndpoint = "";
    // Voice signature format as specified here https://aka.ms/cts/signaturegenservice
    private String voiceSignature = "{ \"Version\": 0,\"Tag\": \"9cS5rFEgDkqaXSkFRoI0ab2/SDtqJjFlsRoI2Tht3fg=\",         \"Data\": \"r4tJwSq280QIBWRX8tKcjxYwDySvX6VZFGkqLLroFV3HIlARgA1xXdFcVK9a2xbylLNQUSNwdUUsIpBDB+jlz6W97XgJ9GlBYLf6xVzUmBg1Qhac32DH3c810HDtpwJk3FkEveM7ohLjhvnYKwjBNqbAVGUONyLYpO28kcxRhvSOxe5/2PeVOgpXMGMcBt3IKN3OmNSOokg4QkqoRUNuRMg5jdoq7BraOyr7CEOP2/GsicmUcONNhFaLuEwy97WRUXE0RWTdDxeR9dn2ngSESq+vYiCkudDi/TGh0ZhxABTxU6EiFQl7uiYG28drjosWdrOV5FPGe2pP8omEoBgtc+yOxYa40HG/yQ160Enqv8umCTcTeW6bkA9CZJ7K8740oZkA8pdpsWkurpFJlMDK3e3Y6w/W1/P55gz/jegYTusDDoz5fINcoWj1zbyLMaFgig3PlEDLKG2hb09Jy4OhEeaBgVqEXiUTEX/R44pd7nUK49xrRJ9yM2gfUq8S+229hJ40N5ZMe+9G848jtsGOziPs20KNlqpL6tiXGAeynhclHyt3pITJjOJi9/cYKYbNm3dR+PtxuLL1WAgIuaK65aGhyW0NmFYm/r7hfAK9a2nTNJIgTsFLG32jljkpaurtwvHuAtIhK8KnopeN6OPXjGl2q06bqI2U92eBxKRroeGUEq3PiXHwVk9DOIFzOAdz\"}";

    @BeforeClass
    static public void setUpBeforeClass() throws Exception {
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

        ConversationTranscriber t = new ConversationTranscriber(s, AudioConfig.fromWavFileInput(Settings.WavFile8Channels));
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

    @Test
    public void testConversationAddParticipant() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.ConversationTranscriptionKey);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile8Channels);
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
        Participant participant = Participant.from("userIdForParticipant", "en-us", voiceSignature);
        t.addParticipant(participant);

        String result = getFirstTranscriberResult(t);
        assertEquals(Settings.WavFile8ChannelsUtterance, result);

        t.close();
        t.close();
    }

    @Test
    public void testRemoveParticipant() throws InterruptedException, ExecutionException, TimeoutException {

        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.ConversationTranscriptionKey);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile8Channels);
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
        Participant participant = Participant.from("userIdForParticipant", "en-us", voiceSignature);
        t.addParticipant(participant);
        t.removeParticipant(participant);

        String result = getFirstTranscriberResult(t);
        assertEquals(Settings.WavFile8ChannelsUtterance, result);

        t.close();
        t.close();
    }

    @Test
    public void testStartAndStopConversationTranscribingAsync() throws InterruptedException, ExecutionException, TimeoutException {
        SpeechConfig s = SpeechConfig.fromEndpoint(URI.create(inroomEndpoint), Settings.ConversationTranscriptionKey);
        assertNotNull(s);

        WavFileAudioInputStream ais = new WavFileAudioInputStream(Settings.WavFile8Channels);
        assertNotNull(ais);

        ConversationTranscriber t = new ConversationTranscriber(s, AudioConfig.fromStreamInput(ais));
        assertNotNull(t);
        assertNotNull(t.getTranscriberImpl());
        assertTrue(t instanceof Recognizer);

        t.setConversationId("ConversationPullStreamTest");

        t.addParticipant("xyz@example.com");

        String result = getFirstTranscriberResult(t);
        assertEquals(Settings.WavFile8ChannelsUtterance, result);

        t.close();
        s.close();
    }

    private String getFirstTranscriberResult(ConversationTranscriber t) throws InterruptedException, ExecutionException, TimeoutException {
        String result;
        final ArrayList<String> rEvents = new ArrayList<>();

        t.recognizing.addEventListener((o, e) -> {
            System.out.println("Conversation transcriber recognizing:" + e.toString());
        });

        t.recognized.addEventListener((o, e) -> {
            rEvents.add(e.getResult().getText());
            System.out.println("Conversation transcriber recognized:" + e.toString());
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

        assertEquals(1, rEvents.size());
        result = rEvents.get(0);

        future = t.stopTranscribingAsync();
        assertNotNull(future);

        // Wait for max 30 seconds
        future.get(30, TimeUnit.SECONDS);

        assertFalse(future.isCancelled());
        assertTrue(future.isDone());

        return result;
    }
}
