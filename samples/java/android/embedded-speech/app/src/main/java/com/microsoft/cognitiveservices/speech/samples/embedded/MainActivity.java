//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See https://aka.ms/csspeech/license for the full license information.
//

package com.microsoft.cognitiveservices.speech.samples.embedded;

import androidx.core.app.ActivityCompat;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.concurrent.Future;

import static android.Manifest.permission.*;

public class MainActivity extends AppCompatActivity
{
    /**********************************
     * START OF CONFIGURABLE SETTINGS *
     **********************************/

    // Enter the names and keys of your embedded speech recognition model and synthesis voice.
    // If either recognition or synthesis is not needed, leave the corresponding default values unchanged.
    private static final String EmbeddedSpeechRecognitionModelName = ""; // e.g. "en-US" or "Microsoft Speech Recognizer en-US FP Model V8.1"
    private static final String EmbeddedSpeechRecognitionModelKey  = ""; // model decryption key
    private static final String EmbeddedSpeechSynthesisVoiceName   = ""; // e.g. "en-US-AriaNeural" or "Microsoft Server Speech Text to Speech Voice (en-US, AriaNeural)"
    private static final String EmbeddedSpeechSynthesisVoiceKey    = ""; // voice decryption key

    // Embedded speech recognition models and synthesis voices must reside
    // as normal individual files in model/voice specific folders on the
    // target device filesystem, not in a package.
    // NOTE: Direct access to files on the external storage requires both
    // READ_EXTERNAL_STORAGE permission and requestLegacyExternalStorage
    // attribute (max target API level 29) in app/src/main/AndroidManifest.xml.
    // Ref. Android 11 storage permission updates:
    // https://developer.android.com/about/versions/11/privacy/storage
    private static final String EmbeddedSpeechModelsRootPath = Environment.getExternalStorageDirectory().toString() + "/Models"; // example path

    /********************************
     * END OF CONFIGURABLE SETTINGS *
     ********************************/

    private static SpeechRecognizer recognizer = null;
    private static SpeechSynthesizer synthesizer = null;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // We need to request the permissions here as well.
        int requestCode = 5; // unique code for the permission request
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{RECORD_AUDIO, INTERNET, READ_EXTERNAL_STORAGE}, requestCode);
    }

    @Override
    protected void onDestroy()
    {
        super.onDestroy();

        if (recognizer != null)
        {
            recognizer.close();
        }
        if (synthesizer != null)
        {
            synthesizer.close();
        }
    }

    public void onInitObjectsButtonClicked(View v)
    {
        try
        {
            Log.d("EmbeddedSpeechSamples", "Path " + EmbeddedSpeechModelsRootPath);
            File path = new File(EmbeddedSpeechModelsRootPath);
            if (path.exists() && path.canRead() && path.isDirectory())
            {
                // For debugging whether files under EmbeddedSpeechModelsRootPath
                // can be read by the application process as required.
                listFiles(path.getAbsolutePath(), 0);
            }
            else
            {
                throw new FileNotFoundException("Invalid path " + EmbeddedSpeechModelsRootPath);
            }

            // Creates an instance of embedded speech config.
            EmbeddedSpeechConfig speechConfig = EmbeddedSpeechConfig.fromPath(EmbeddedSpeechModelsRootPath);

            StringBuilder sb = new StringBuilder("Initialized");

            if (!EmbeddedSpeechRecognitionModelName.isEmpty() && !EmbeddedSpeechRecognitionModelKey.isEmpty())
            {
                // Selects the embedded speech recognition model to use.
                speechConfig.setSpeechRecognitionModel(EmbeddedSpeechRecognitionModelName, EmbeddedSpeechRecognitionModelKey);

                // Creates a speech recognizer instance using the device default
                // microphone for audio input.
                // With embedded speech, this can take a moment due to loading
                // of the model. To avoid unnecessary delays when recognition is
                // started, create the recognizer well in advance.
                recognizer = new SpeechRecognizer(speechConfig);
                sb.append(" recognizer");
            }

            if (!EmbeddedSpeechSynthesisVoiceName.isEmpty() && !EmbeddedSpeechSynthesisVoiceKey.isEmpty())
            {
                // Selects the embedded speech synthesis voice to use.
                speechConfig.setSpeechSynthesisVoice(EmbeddedSpeechSynthesisVoiceName, EmbeddedSpeechSynthesisVoiceKey);

                if (EmbeddedSpeechSynthesisVoiceName.contains("Neural"))
                {
                    // Embedded neural voices only support 24 kHz sample rate.
                    speechConfig.setSpeechSynthesisOutputFormat(SpeechSynthesisOutputFormat.Riff24Khz16BitMonoPcm);
                }

                // Creates a speech synthesizer instance using the device default
                // speaker for audio output.
                synthesizer = new SpeechSynthesizer(speechConfig);
                if (recognizer != null)
                {
                    sb.append(" and");
                }
                sb.append(" synthesizer");
            }

            if (recognizer == null && synthesizer == null)
            {
                UpdateStatus("Cannot initialize recognizer or synthesizer!");
            }
            else
            {
                sb.append(".");
                UpdateStatus(sb.toString());
            }

            speechConfig.close();
        }
        catch (Exception ex)
        {
            Log.e("EmbeddedSpeechSamples", "Exception caught: " + ex.getMessage());
            assert(false);
        }
    }

    private void listFiles(String dirPath, int indent)
    {
        // Recursively list files under the given directory path, output to the system log.
        File dir = new File(dirPath);
        File[] files = dir.listFiles();

        if (files != null && files.length > 0)
        {
            for (File file : files)
            {
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < indent; i++) sb.append("  ");

                if (file.isDirectory())
                {
                    sb.append(file.getName()).append("/");
                    Log.d("EmbeddedSpeechSamples", "Dir  " + sb);
                    listFiles(file.getAbsolutePath(), indent + 1);
                }
                else
                {
                    sb.append(file.getName());
                    Log.d("EmbeddedSpeechSamples", "File " + sb);
                }
            }
        }
    }

    public void onRecognizeSpeechButtonClicked(View v)
    {
        if (recognizer == null)
        {
            UpdateStatus("Recognizer is not initialized!");
            return;
        }

        try
        {
            // Starts speech recognition and returns after a single utterance is
            // recognized. The task returns the recognition text as result.
            // Note: Since RecognizeOnceAsync() returns only a single utterance,
            // it is best suited for single-shot recognition, like a command or
            // short query. For long-running multi-utterance recognition, use
            // StartContinuousRecognitionAsync() instead (see general C# samples).
            Future<SpeechRecognitionResult> task = recognizer.recognizeOnceAsync();

            // Note that this will block the UI thread, so eventually you
            // should register for and use the result events instead (see
            // general Java JRE samples).
            SpeechRecognitionResult result = task.get();

            // Checks the result.
            StringBuilder sb = new StringBuilder();
            if (result.getReason() == ResultReason.RecognizedSpeech)
            {
                sb.append("RECOGNIZED: Text=").append(result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch)
            {
                NoMatchDetails nomatch = NoMatchDetails.fromResult(result);
                sb.append("NO MATCH: Reason=").append(nomatch.getReason());
            }
            else if (result.getReason() == ResultReason.Canceled)
            {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                sb.append("CANCELED: Reason=").append(cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error)
                {
                    sb.append(" ErrorCode=").append(cancellation.getErrorCode());
                    sb.append(" ErrorDetails=").append(cancellation.getErrorDetails());
                }
            }
            UpdateStatus(sb.toString());

            result.close();
        }
        catch (Exception ex)
        {
            Log.e("EmbeddedSpeechSamples", "Exception caught: " + ex.getMessage());
            assert(false);
        }
    }

    public void onSynthesizeSpeechButtonClicked(View v)
    {
        if (synthesizer == null)
        {
            UpdateStatus("Synthesizer is not initialized!");
            return;
        }

        try
        {
            EditText input = this.findViewById(R.id.synthesisInputText);
            String inputText = input.getText().toString();

            if (inputText.isEmpty())
            {
                UpdateStatus("Missing text input for synthesis!");
                return;
            }

            // Synthesizes speech.
            Future<SpeechSynthesisResult> task = synthesizer.SpeakTextAsync(inputText);
            SpeechSynthesisResult result = task.get();

            // Checks the result.
            StringBuilder sb = new StringBuilder();
            if (result.getReason() == ResultReason.SynthesizingAudioCompleted)
            {
                sb.append("Synthesis completed.");
            }
            else if (result.getReason() == ResultReason.Canceled)
            {
                SpeechSynthesisCancellationDetails cancellation = SpeechSynthesisCancellationDetails.fromResult(result);
                sb.append("CANCELED: Reason=").append(cancellation.getReason());

                if (cancellation.getReason() == CancellationReason.Error)
                {
                    sb.append(" ErrorCode=").append(cancellation.getErrorCode());
                    sb.append(" ErrorDetails=").append(cancellation.getErrorDetails());
                }
            }
            UpdateStatus(sb.toString());

            result.close();
        }
        catch (Exception ex)
        {
            Log.e("EmbeddedSpeechSamples", "Exception caught: " + ex.getMessage());
            assert(false);
        }
    }

    private void UpdateStatus(String message)
    {
        TextView status = (TextView) this.findViewById(R.id.statusText);
        status.setText(message);
    }
}
