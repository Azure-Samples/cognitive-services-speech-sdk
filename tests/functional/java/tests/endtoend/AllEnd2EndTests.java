package tests.endtoend;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

@RunWith(Suite.class)
@SuiteClasses({
    MicrophoneAudioInputStreamTest.class,
    SampleRecognizeIntentTest.class,
    SampleRecognizeWithIntermediateResultsTest.class,
    SampleSimpleRecognizeTest.class,
    WavFileAudioInputStreamTest.class,
    RecognizerTests.class,
    VirtualAssistantTests.class
    })
public class AllEnd2EndTests {

}
