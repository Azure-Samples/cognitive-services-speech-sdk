package tests.endtoend;

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import tests.Settings;

public class SampleSimpleRecognize implements Runnable {

    private String _result;
    public String getResult()
    {
        return _result;
    }
    
    ///////////////////////////////////////////////////
    // recognize
    ///////////////////////////////////////////////////
    @Override
    public void run() {
        // create factory
        SpeechFactory factory = Settings.getFactory();

        try {
            // TODO: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            SpeechRecognizer reco = factory.createSpeechRecognizer(Settings.WaveFile);

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            SpeechRecognitionResult result = task.get();
            _result = result.getRecognizedText();

            System.out.println("Recognizer returned: " + _result);
            
            reco.close();
            factory.close();
        } catch (Exception ex) {
            _result = ex.toString();
            
            Settings.displayException(ex);
            throw new IllegalAccessError(ex.toString());
        }
    }
}
