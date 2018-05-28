package tests.endtoend;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

@RunWith(Suite.class)
@SuiteClasses({
    MicrophoneAudioInputStreamTest.class,
    SampleRecognizeIntentTest.class,
    SampleRecognizeWithIntermediateResultsTest.class,
    SampleSimpleRecognizeTest.class,
    WaveFileAudioInputStreamTest.class
    })
public class AllEnd2EndTests {

}
