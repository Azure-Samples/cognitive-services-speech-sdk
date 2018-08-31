//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { Settings } from "./Settings";
import { WaveFileAudioInputStream } from "./WaveFileAudioInputStream";
import { WaitForCondition } from "./Utilities";

beforeAll(() => {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

const FIRST_EVENT_ID: number = 1;
let eventIdentifier: number;


// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Ignore("TODO not working with microphone")
//@Test
test("TranslationRecognizer1", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizer("en-US", targets);
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});

//@Test
test("TranslationRecognizer2", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("GetSourceLanguage", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-us");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);
    expect(r.sourceLanguage).not.toBeUndefined();
    expect(r.sourceLanguage).not.toBeNull();
    expect(r.sourceLanguage).toEqual(language);

    r.close();
    s.close();
});

//@Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
//@Test
test("GetTargetLanguages", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-us");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);
    expect(r.targetLanguages).not.toBeUndefined();
    expect(r.targetLanguages).not.toBeNull();
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(language);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("GetOutputVoiceNameNoSetting", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const language: string = "en-US";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);

    expect(r.outputVoiceName).not.toBeUndefined();

    r.close();
    s.close();
});

//@Test
test("GetOutputVoiceName", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const language: string = "en-US";
    const voice: string = "de-DE-Katja";
    const r = s.createTranslationRecognizerWithStreamAndVoice(ais, language, targets, voice);

    expect(r.outputVoiceName).toEqual(voice);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
//@Test
test("GetParameters", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();

    expect(r.parameters).not.toBeUndefined();
    expect(r.sourceLanguage).toEqual(r.parameters.get(sdk.RecognizerParameterNames.TranslationFromLanguage, ""));

    // TODO this cannot be true, right? comparing an array with a string parameter???
    expect(r.targetLanguages.length).toEqual(1);
    expect(r.targetLanguages[0]).toEqual(r.parameters.get(sdk.RecognizerParameterNames.TranslationToLanguage + "0"));

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------
//@Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
//@Test
test("RecognizeAsync1", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-de");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);

    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.recognizeAsync(
        (res: sdk.TranslationTextResult) => {
            expect(res).not.toBeUndefined();
            expect(sdk.RecognitionStatus.Recognized).toEqual(res.translationStatus);
            expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));

            r.close();
            s.close();
            done();
        },
        (error: string) => {
            fail(error);

            r.close();
            s.close();
            done();
        });
}, 10000);

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------
//@Ignore("TODO why is number translations not 1 (FIX JAVA LIB FORWARD PROPERTY)")
//@Test
test("Translate Multiple Targets", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-de");
    targets.push("en-US");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithStream(ais, language, targets);

    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.recognizeAsync(
        (res: sdk.TranslationTextResult) => {
            expect(res).not.toBeUndefined();
            expect(sdk.RecognitionStatus.Recognized).toEqual(res.translationStatus);
            expect("Wie ist das Wetter?").toEqual(res.translations.get("de", ""));
            expect("What's the weather like?").toEqual(res.translations.get("en", ""));
            
            r.close();
            s.close();
            done();
        },
        (error: string) => {
            fail(error);

            r.close();
            s.close();
            done();
        });
}, 10000);


test("RecognizeAsync_badStream", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("de-de");

    const language: string = "en-us";
    const r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, language, targets);

    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.recognizeAsync(
        (res: sdk.TranslationTextResult) => {
            fail("Should have hit an error");
        },
        (error: string) => {
            r.close();
            s.close();
            done();
        });
}, 10000);

//@Ignore("TODO why is event order wrong?")
//@Test
test("Validate Event Ordering", done => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    expect(ais).not.toBeUndefined();

    const r = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    const eventsMap: { [id: string]: number; } = {};
    eventIdentifier = 1;

    r.FinalResultReceived = (o, e) => {
        eventsMap["FinalResultReceived"] = eventIdentifier++;
    };

    r.IntermediateResultReceived = (o, e) => {
        const now: number = eventIdentifier++;
        eventsMap["IntermediateResultReceived-" + Date.now().toPrecision(4)] = now;
        eventsMap["IntermediateResultReceived"] = now;
    };

    r.RecognitionErrorRaised = (o, e) => {
        eventsMap["RecognitionErrorRaised"] = eventIdentifier++;
    };

    // TODO eventType should be renamed and be a function getEventType()
    r.RecognitionEvent = (o, e) => {
        const now: number = eventIdentifier++;
        eventsMap[e.eventType.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap[e.eventType.toString()] = now;
    };

    r.SessionEvent = (o, e) => {
        const now: number = eventIdentifier++;
        eventsMap["Session:" + e.eventType.toString() + "-" + Date.now().toPrecision(4)] = now;
        eventsMap["Session:" + e.eventType.toPrecision()] = now;
    };

    // TODO there is no guarantee that SessionStoppedEvent comes before the recognizeAsync() call returns?!
    //      this is why below SessionStoppedEvent checks are conditional 
    r.recognizeAsync((res: sdk.TranslationTextResult) => {

        expect(res).not.toBeUndefined();
        expect(res.translations.get("en", "")).toEqual("What's the weather like?");

        // session events are first and last event
        const LAST_RECORDED_EVENT_ID: number = eventIdentifier;
        expect(LAST_RECORDED_EVENT_ID).toBeGreaterThan(FIRST_EVENT_ID);
        expect(FIRST_EVENT_ID).toEqual(eventsMap["Session:" + sdk.SessionEventType.SessionStartedEvent.toString()]);

        if ("Session:" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
            expect(LAST_RECORDED_EVENT_ID).toEqual(eventsMap["Session:" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
        }

        // end events come after start events.
        if ("Session:" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
            expect(eventsMap["Session:" + sdk.SessionEventType.SessionStartedEvent.toString()]).toBeLessThan(eventsMap["Session:" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
        }
       
        expect((FIRST_EVENT_ID + 1)).toEqual(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);
       
        //expect(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]).toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);
        // expect((LAST_RECORDED_EVENT_ID - 1)).toEqual(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]);

        // recognition events come after session start but before session end events
        expect(eventsMap["Session:" + sdk.SessionEventType.SessionStartedEvent.toString()]).toBeLessThan(eventsMap[sdk.RecognitionEventType.SpeechStartDetectedEvent.toString()]);

        if ("Session:" + sdk.SessionEventType.SessionStoppedEvent.toString() in eventsMap) {
            expect(eventsMap[sdk.RecognitionEventType.SpeechEndDetectedEvent.toString()]).toBeLessThan(eventsMap["Session:" + sdk.SessionEventType.SessionStoppedEvent.toString()]);
        }

        // there is no partial result reported after the final result
        // (and check that we have intermediate and final results recorded)
        expect(eventsMap["IntermediateResultReceived"]).toBeLessThan(eventsMap["FinalResultReceived"]);

        // make sure events we don't expect, don't get raised
        expect("RecognitionErrorRaised" in eventsMap).toEqual(false);
        r.close();
        s.close();
        done();
    
    }, (error: string) => {
        fail(error);
        r.close();
        s.close();
        done();
    });
});
/*
// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("StartContinuousRecognitionAsync", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    Future <?> future = r.startContinuousRecognitionAsync();
    assertNotNull(future);

    // Wait for max 30 seconds
    future.get(30, TimeUnit.SECONDS);

    assertFalse(future.isCancelled());
    assertTrue(future.isDone());

    r.close();
    s.close();
});

//@Test
test("StopContinuousRecognitionAsync", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    Future <?> future = r.startContinuousRecognitionAsync();
    assertNotNull(future);

    // Wait for max 30 seconds
    future.get(30, TimeUnit.SECONDS);

    assertFalse(future.isCancelled());
    assertTrue(future.isDone());

    // just wait one second
    Thread.sleep(1000);

    future = r.stopContinuousRecognitionAsync();
    assertNotNull(future);

    // Wait for max 30 seconds
    future.get(30, TimeUnit.SECONDS);

    assertFalse(future.isCancelled());
    assertTrue(future.isDone());

    r.close();
    s.close();
});

//@Test
test("StartStopContinuousRecognitionAsync", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    final ArrayList < String > rEvents = new ArrayList<>();

    r.FinalResultReceived.addEventListener((o, e) -> {
        rEvents.add("Result@" + System.currentTimeMillis());
    });

    Future <?> future = r.startContinuousRecognitionAsync();
    assertNotNull(future);

    // Wait for max 30 seconds
    future.get(30, TimeUnit.SECONDS);

    assertFalse(future.isCancelled());
    assertTrue(future.isDone());

    // wait until we get at least on final result
    long now = System.currentTimeMillis();
    while (((System.currentTimeMillis() - now) < 30000) &&
        (rEvents.isEmpty())) {
        Thread.sleep(200);
    }

    // test that we got one result
    // TODO multi-phrase test with several phrases in one session
    assertEquals(1, rEvents.size());

    future = r.stopContinuousRecognitionAsync();
    assertNotNull(future);

    // Wait for max 30 seconds
    future.get(30, TimeUnit.SECONDS);

    assertFalse(future.isCancelled());
    assertTrue(future.isDone());

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Ignore("TODO not working with microphone")
//@Test
test("GetRecoImpl", () => {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    expect(s).not.toBeUndefined();

    const targets: string[] = [];
    targets.push("en-US");

    const r = s.createTranslationRecognizer("en-US", targets);
    expect(r).not.toBeUndefined();
    //assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer).toEqual(true);

    r.close();
    s.close();
});
*/