import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.AfterAll;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import com.microsoft.cognitiveservices.speech.SessionEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SessionEventListener;

class SpeechRecognizer {

    @BeforeAll
    static void setUpBeforeClass() throws Exception {
        System.loadLibrary("carbon_java_bindings");
    }

    @Test
    void test()
    {
        com.microsoft.cognitiveservices.speech.SpeechRecognizer reco = com.microsoft.cognitiveservices.speech.RecognizerFactory.createSpeechRecognizerWithFileInput("whatstheweatherlike.wav");

        MySessionEventListener listener = new MySessionEventListener();
        assertFalse(listener.listenerFired, "callback?");

        reco.getSessionStarted().addEventListener(listener);

        SpeechRecognitionResult result = reco.recognize();
        assertNotNull(result);
        assertTrue(result.getResultId().length() > 0, "no result?");
        assertTrue(listener.listenerFired, "no callback?");

        String s = result.getText();
        assertNotNull(s);
        assertTrue(s.length() > 0, "no result?");

        System.out.println("*********  reco: " + s);        
    }

    public static class MySessionEventListener extends SessionEventListener
    {
        public boolean listenerFired;

        public MySessionEventListener()
        {
            listenerFired = false;
        }

        public void execute(SessionEventArgs eventArgs)
        {
            listenerFired = true;
            System.out.println(eventArgs.toString());
        }

    };

}
