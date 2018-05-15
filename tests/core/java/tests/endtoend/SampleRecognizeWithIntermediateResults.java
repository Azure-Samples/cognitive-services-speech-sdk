package tests.endtoend;

import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResultEventArgs;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import tests.Settings;

public class SampleRecognizeWithIntermediateResults implements Runnable {
    
    private SpeechRecognitionResult _result;
    public SpeechRecognitionResult getResult()
    {
        return _result;
    }
    
    private SpeechRecognitionResultEventArgs _speechRecognitionResultEventArgs;
    public SpeechRecognitionResultEventArgs getSpeechRecognitionResultEventArgs() {
        return _speechRecognitionResultEventArgs;
    }
    
    
    ///////////////////////////////////////////////////
    // recognize with intermediate results
    ///////////////////////////////////////////////////
    public void run() {
        // create factory
        SpeechFactory factory = Settings.getFactory();

        try {
            // TODO: to use the microphone, replace the parameter with "new MicrophoneAudioInputStream()"
            SpeechRecognizer reco = factory.createSpeechRecognizer(Settings.WaveFile);

            reco.IntermediateResultReceived.addEventListener((o, speechRecognitionResultEventArgs) -> {
                _speechRecognitionResultEventArgs = speechRecognitionResultEventArgs;
                String s = _speechRecognitionResultEventArgs.getResult().getRecognizedText();

                System.out.println("Intermediate result received: " + s);
            });

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();

            _result = task.get();
            String s = _result.getRecognizedText();
            System.out.println("Recognizer returned: " + s);
            
            reco.close();
            factory.close();
        } catch (Exception ex) {
            Settings.displayException(ex);
            
            throw new IllegalAccessError(ex.toString());
        }
    }
}
