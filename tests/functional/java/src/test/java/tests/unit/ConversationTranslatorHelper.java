//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license
// information.
//

package tests.unit;

import com.microsoft.cognitiveservices.speech.transcription.ConversationTranslator;
import com.microsoft.cognitiveservices.speech.transcription.Participant;
import java.util.ArrayList;

public class ConversationTranslatorHelper {

    ArrayList<String> items;
    private String tag = "";

    ConversationTranslatorHelper() {
        items = new ArrayList<String>();
    }

    ConversationTranslatorHelper(String tag) {
        items = new ArrayList<String>();
        this.tag = tag;
    }

    static ConversationTranslatorHelper instance(ConversationTranslator ct, String tag) {
        ConversationTranslatorHelper cth = new ConversationTranslatorHelper(tag);
        cth.logEvents(ct);
        return cth;
    }

    static ConversationTranslatorHelper instance(ConversationTranslator ct) {
        ConversationTranslatorHelper cth = new ConversationTranslatorHelper();
        cth.logEvents(ct);
        return cth;
    }

    public void logEvents(ConversationTranslator ct) {
        clearItems();

        ct.transcribing.addEventListener(
                (o, e) -> {
                    log(" transcribing: " + e.toString());
                    items.add("transcribing:" + e.toString());
                });

        ct.transcribed.addEventListener(
                (o, e) -> {
                    log(" transcribed: " + e.toString());
                    items.add("transcribed:" + e.toString());
                });

        ct.canceled.addEventListener(
                (o, e) -> {
                    log(" canceled: " + e.toString());
                    items.add("canceled:" + e.toString());
                });

        ct.conversationExpiration.addEventListener(
                (o, e) -> {
                    log(" conversationExpiration: " + e.toString());
                    items.add("conversationExpiration:" + e.toString());
                });

        ct.participantsChanged.addEventListener(
                (o, e) -> {
                    log(" participantsChanged: " + e.toString());
                    items.add("participantsChanged:" + e.toString());
                    int i = 0;
                    for (Participant p : e.getParticipants()) {
                        log(String.format("   Participant[%d]:  %s, disp %s, host %b, muted %b, tts %b, avatar %s",
                         i, p.getId(), p.getDisplayName(), p.isHost(), p.isMuted(), p.isUsingTts(), p.getAvatar()));
                        i++;
                    }
                });

        ct.textMessageReceived.addEventListener(
                (o, e) -> {
                    log(" textMessageReceived: " + e.toString());
                    items.add("textMessageReceived:" + e.toString());
                });

        ct.sessionStarted.addEventListener(
                (o, e) -> {
                    log(" sessionStarted: " + e.toString());
                    items.add("sessionStarted:" + e.toString());
                });

        ct.sessionStopped.addEventListener(
                (o, e) -> {
                    log(" sessionStopped: " + e.toString());
                    items.add("sessionStopped:" + e.toString());
                });
    }

    public void clearItems() {
        items.clear();
    }

    public int size() {
        return items.size();
    }

    private void log(String message) {
        System.out.format("%s ConversationTranslatorHelper[%2d]: %s\n", tag, items.size(), message);
    }
}
