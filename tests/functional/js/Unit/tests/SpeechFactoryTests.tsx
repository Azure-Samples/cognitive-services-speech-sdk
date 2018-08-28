//
//Copyright (c) Microsoft. All rights reserved.
//Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import * as sdk from "../../../../../source/bindings/js/Speech.Browser.Sdk";
import { Settings } from "./Settings";
import {WaveFileAudioInputStream} from "./WaveFileAudioInputStream";

import { isString } from "util";

beforeAll(()=> {
    // Override inputs, if necessary
    Settings.LoadSettings();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

////@Test(expected = NullPointerException.class)
test("testFromSubscription1", ()=> {
    expect(()=>sdk.SpeechFactory.fromSubscription(null, null)).toThrowError();
});

////@Test(expected = NullPointerException.class)
test("testFromSubscription2()", ()=> {
    expect(()=>sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, null)).toThrowError();
});

////@Test(expected = NullPointerException.class)
test("testFromSubscription3", ()=> {
    expect(()=>sdk.SpeechFactory.fromSubscription(null, Settings.SpeechRegion)).toThrowError();
});

////@Ignore("TODO why does illegal key succeed?")
////@Test(expected = NullPointerException.class)
test.skip("testFromSubscription4", ()=> {
    expect(()=>sdk.SpeechFactory.fromSubscription("illegal", "illegal")).toThrowError();
});

////@Test() 
test("testFromSubscriptionSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s).not.toBeUndefined();
    
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

test("testGetSubscriptionKey", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s).not.toBeUndefined();
    expect(Settings.SpeechSubscriptionKey == s.subscriptionKey);
    
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

test("testGetRegion", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s).not.toBeUndefined();
    expect(Settings.SpeechRegion == s.region);
    
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test(expected = NullPointerException.class)
test("testFromEndpoint1", ()=> {
    expect(()=> sdk.SpeechFactory.fromEndpoint(null, null)).toThrowError();
});

//@Test(expected = NullPointerException.class)
test("testFromEndpoint2", ()=> {
    expect(()=> sdk.SpeechFactory.fromEndpoint(null, Settings.SpeechRegion)).toThrowError();
});

//@Test(expected = NullPointerException.class)
test("testFromEndpoint3", ()=> {
    expect(()=> sdk.SpeechFactory.fromEndpoint(new URL("http://www.example.com"), null)).toThrowError();
});

////@Ignore("TODO why does illegal token not fail?")
//@Test(expected = exception.class)
test.skip("testFromEndpoint4", ()=> {
    expect(()=> sdk.SpeechFactory.fromEndpoint(new URL("http://www.example.com"), "illegal-subscription")).toThrowError();
});

//@Test
test("testFromEndpointSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromEndpoint(new URL("http://www.example.com"), Settings.SpeechSubscriptionKey);
    
    expect(s).not.toBeUndefined();
    
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testGetParameters1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    
    expect(s.parameters).not.toBeNull();
    expect(s.parameters).not.toBeUndefined();

 //   expect(s.region).toEqual(s.parameters.get(sdk.FactoryParameterNames.Region));
 //   expect(s.subscriptionKey == s.parameters.get(FactoryParameterNames.SubscriptionKey));
    expect(s.region).toEqual(s.parameters.get("SPEECH-Region", null));
    expect(s.subscriptionKey == s.parameters.get("SPEECH-SubscriptionKey", null));
    
    s.close();
});

////@Ignore("TODO why is a string an int?")
//@Test
test("testGetParametersString", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const name: string = "stringName";
    const value: string = "stringValue";
    const p: sdk.ISpeechProperties =  s.parameters;
    expect(p).not.toBeUndefined();

    p.set(name, value);
    
    expect(isString(p.get(name, null)));
    
    expect(value == p.get(name, null));
    expect(value == p.get(name, "some-other-default-" + value)).not;

    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

////@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateSpeechRecognizer", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    
    const r: sdk.SpeechRecognizer = s.createSpeechRecognizer();
    
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    
    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

////@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateSpeechRecognizerLanguage1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=> s.createSpeechRecognizerWithLanguage(null)).toThrowError();
        
    s.close();
});

////@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test.skip("testCreateSpeechRecognizerLanguage2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(s.createSpeechRecognizerWithLanguage("illegal-language")).toThrowError();
        
    s.close();
});

////@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateSpeechRecognizerLanguageSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    
    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithLanguage("en-US");
    
    expect(r).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    
    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testCreateSpeechRecognizerString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=> s.createSpeechRecognizerWithFileInput(null)).toThrowError();
        
    s.close();
});

//@Test
test.skip("testCreateSpeechRecognizerString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=> s.createSpeechRecognizerWithFileInput("illegal-" + Settings.WaveFile)).toThrowError();
        
    s.close();
});

//@Test
test("testCreateSpeechRecognizerStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    
    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithFileInput(Settings.WaveFile);
    
    expect(r).not.toBeUndefined();
 //??   //expect(r.getRecoImpl()).not.toBeUndefined();
    expect(r instanceof sdk.Recognizer);
    
    r.close();
    s.close();
});

//@Test
test("testCreateSpeechRecognizerStringString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=> s.createSpeechRecognizerWithFileInputAndLanguage(null, null)).toThrowError();
        
    s.close();
});

//@Test
test("testCreateSpeechRecognizerStringString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    
    expect(()=> s.createSpeechRecognizerWithFileInputAndLanguage(null, "en-EN")).toThrowError();
        
    s.close();
});

////@Ignore("TODO why can create illegal language")
//@Test
test.skip("testCreateSpeechRecognizerStringString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=> s.createSpeechRecognizerWithFileInputAndLanguage(Settings.WaveFile, "illegal-language")).toThrowError();
    
    s.close();
});

//@Test
test("testCreateSpeechRecognizerStringStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithFileInputAndLanguage(Settings.WaveFile, "en-EN");
    
    expect(r).not.toBeUndefined();
 //??   assertNotNull(r.getRecoImpl());
    
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Ignore("TODO can create with null stream?")
//@Test
test("testCreateSpeechRecognizerAudioInputStream1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=>s.createSpeechRecognizerWithStream(null)).toThrowError();
        
    s.close();
});

//@Test
test("testCreateSpeechRecognizerAudioInputStreamSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStream(ais);
    
    expect(r).not.toBeUndefined();
//??//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    s.close();
});

//@Test
test("testCreateSpeechRecognizerAudioInputStreamString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=>s.createSpeechRecognizerWithStreamAndLanguage(null, null)).toThrowError();
        
    s.close();
});

//@Ignore("TODO why does null stream not fail")
//@Test
test("testCreateSpeechRecognizerAudioInputStreamString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    expect(()=>s.createSpeechRecognizerWithStreamAndLanguage(null, "en-US")).toThrowError();
    
    s.close();
});

//@Test
test("testCreateSpeechRecognizerAudioInputStreamString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    
    expect(()=>s.createSpeechRecognizerWithStreamAndLanguage(ais, null)).toThrowError();
    
    s.close();
});

//@Ignore("TODO why does illegal language not fail")
//@Test
test.skip("testCreateSpeechRecognizerAudioInputStreamString4", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    
    expect(()=>s.createSpeechRecognizerWithStreamAndLanguage(ais, "illegal-language")).toThrowError();
        
    s.close();
});

//@Test
test("testCreateSpeechRecognizerAudioInputStreamStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    
    const r: sdk.SpeechRecognizer = s.createSpeechRecognizerWithStreamAndLanguage(ais, "en-US");
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    ais.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateIntentRecognizer", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.IntentRecognizer = s.createIntentRecognizer();

    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateIntentRecognizerLanguage1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithLanguage(null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test.skip("testCreateIntentRecognizerLanguage2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithLanguage("illegal-language");
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});
    
//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateIntentRecognizerLanguageSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithLanguage("en-US");
    
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

//@Test
test("testCreateIntentRecognizerString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithFileInput(null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test.skip("testCreateIntentRecognizerString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithFileInput("illegal-" + Settings.WaveFile);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});
    
//@Test
test("testCreateIntentRecognizerStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithFileInput(Settings.WaveFile);
    
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});


//@Ignore("TODO why can create with null stream?")
//@Test
test("testCreateIntentRecognizerAudioInputStream1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStream(null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateIntentRecognizerAudioInputStreamSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStream(ais);
    
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    s.close();
});

//@Test
test("testCreateIntentRecognizerAudioInputStreamString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO why does null stream not fail")
//@Test
test("testCreateIntentRecognizerAudioInputStreamString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(null, "en-US");
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO why does illegal language not fail")
//@Test
test.skip("testCreateIntentRecognizerAudioInputStreamString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(ais, "illegal-language");
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateIntentRecognizerAudioInputStreamStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    const r: sdk.IntentRecognizer = s.createIntentRecognizerWithStreamAndLanguage(ais, "en-US");
    
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);
    
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer(null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("illegal", null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfString4", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];;

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", targets);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test.skip("testCreateTranslationRecognizerStringArrayListOfString5", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", targets);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});


//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-US"];
    
    const r: sdk.TranslationRecognizer = s.createTranslationRecognizer("en-EN", targets);
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice(null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("illegal", null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringString4", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];

        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringString5", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringString6", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test.skip("testCreateTranslationRecognizerStringArrayListOfStringString7", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, "illegal");
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO: enable. however, this will crash the java vm at shutdown due to COM issues.")
//@Test
test("testCreateTranslationRecognizerStringArrayListOfStringStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-us"];
    
    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithVoice("en-US", targets, "en-US");
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput("illegal-" + Settings.WaveFile, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString4", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "illegal", null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString5", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString6", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO why can create with illegal source language")
//@Test
test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfString7", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-US"];
    
    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInput(Settings.WaveFile, "en-US", targets);
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});


// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(null, null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice("illegal-" + Settings.WaveFile, null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString4", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "illegal", null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString5", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString6", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString7", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString8", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-US"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO why can create with illegal voice?")
//@Test
test.skip("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringString9", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-US"];
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, "illegal");
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithFileInputStringStringArrayListOfStringStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-US"];

    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithFileInputAndVoice(Settings.WaveFile, "en-US", targets, "en-US");
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "illegal", null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString4", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString5", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];
        
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Ignore("TODO why does illegal target language not fail")
//@Test
test.skip("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfString6", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];
        
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }
    
    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-us"];
    
    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    
    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStream(ais, "en-US", targets);
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString1", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(null, null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString2", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, null, null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString3", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "illegal", null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString4", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", null, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString5", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = [];
        
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test.skip("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString6", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["illegal"];
        
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString7", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];
        
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, null);
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Ignore("TODO create with stream on illegal voice does not fail?")
//@Test
test.skip("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringString8", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    try {
        const targets: string[] = ["en-us"];
        
        const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
        
        const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, "illegal");
        fail("not expected");
    }
    catch(ex) {
        // expected
    }

    s.close();
});

//@Test
test("testCreateTranslationRecognizerWithStreamAudioInputStreamStringArrayListOfStringStringSuccess", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);

    const targets: string[] = ["en-us"];
    
    const ais: WaveFileAudioInputStream = new WaveFileAudioInputStream(Settings.WaveFile);
    
    const r: sdk.TranslationRecognizer = s.createTranslationRecognizerWithStreamAndVoice(ais, "en-US", targets, "en-US");
    expect(r).not.toBeUndefined();
//??    assertNotNull(r.getRecoImpl());
    expect(r instanceof sdk.Recognizer);

    r.close();
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testClose", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    
    s.close();
});

// -----------------------------------------------------------------------
// --- 
// -----------------------------------------------------------------------

//@Test
test("testGetFactoryImpl", ()=> {
    const s: sdk.SpeechFactory = sdk.SpeechFactory.fromSubscription(Settings.SpeechSubscriptionKey, Settings.SpeechRegion);
    
    expect(s).not.toBeUndefined();
//??    assertNotNull(s.getFactoryImpl());
});
