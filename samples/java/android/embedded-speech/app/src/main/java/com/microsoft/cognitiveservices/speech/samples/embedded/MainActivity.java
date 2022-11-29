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
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.concurrent.Future;

import static android.Manifest.permission.*;

public class MainActivity extends AppCompatActivity
{
    // Replace below with your embedded speech recognition model name, key
    // and path (on the Android device).
    private static final String EmbeddedSpeechRecognitionModelName = "YourEmbeddedSpeechRecognitionModelName";
    private static final String EmbeddedSpeechRecognitionModelKey  = "YourEmbeddedSpeechRecognitionModelKey";
    private static final String EmbeddedSpeechRecognitionModelPath = Environment.getExternalStorageDirectory().toString() + "/model"; // example path
    // NOTE: Direct access to files on the external storage requires both
    // READ_EXTERNAL_STORAGE permission and requestLegacyExternalStorage
    // attribute (max target API level 29) in app/src/main/AndroidManifest.xml.
    // Ref. Android 11 storage permission updates:
    // https://developer.android.com/about/versions/11/privacy/storage

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // We need to request the permissions here as well.
        int requestCode = 5; // unique code for the permission request
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{RECORD_AUDIO, INTERNET, READ_EXTERNAL_STORAGE}, requestCode);
    }

    public void onSpeechButtonClicked(View v)
    {
        TextView txt = (TextView) this.findViewById(R.id.hello); // 'hello' is the ID of your text view

        try
        {
            // For debugging whether files under EmbeddedSpeechRecognitionModelPath
            // can be read by the app process as required. They must be located as
            // normal individual files on the device filesystem. Note that this is
            // not a recursive listing.
            Log.d("EmbeddedSpeechSamples", "Path: " + EmbeddedSpeechRecognitionModelPath);
            File path = new File(EmbeddedSpeechRecognitionModelPath);
            if (path.exists() && path.canRead() && path.isDirectory())
            {
                File[] files = path.listFiles();
                Log.d("EmbeddedSpeechSamples", "Size: " + files.length);
                for (File file : files)
                {
                    Log.d("EmbeddedSpeechSamples", "File: " + file.getName());
                }
            }
            else
            {
                throw new FileNotFoundException("Invalid path " + EmbeddedSpeechRecognitionModelPath);
            }

            // Creates an instance of an embedded speech config.
            EmbeddedSpeechConfig speechConfig = EmbeddedSpeechConfig.fromPath(EmbeddedSpeechRecognitionModelPath);
            // Mandatory configuration for embedded speech recognition.
            speechConfig.setSpeechRecognitionModel(EmbeddedSpeechRecognitionModelName, EmbeddedSpeechRecognitionModelKey);

            // Creates an instance of a speech recognizer.
            SpeechRecognizer recognizer = new SpeechRecognizer(speechConfig);
            // Runs speech recognition once.
            Future<SpeechRecognitionResult> task = recognizer.recognizeOnceAsync();

            // Note that this will block the UI thread, so eventually you
            // should register for and use the result events instead (see
            // Java JRE samples).
            SpeechRecognitionResult result = task.get();

            // Checks the result.
            if (result.getReason() == ResultReason.RecognizedSpeech)
            {
                txt.setText(result.getText());
            }
            else if (result.getReason() == ResultReason.NoMatch)
            {
                txt.setText("NO MATCH: Speech could not be recognized.");
            }
            else if (result.getReason() == ResultReason.Canceled)
            {
                CancellationDetails cancellation = CancellationDetails.fromResult(result);
                txt.setText("CANCELED: Reason=" + cancellation.getReason() + " ErrorDetails=" + cancellation.getErrorDetails());
            }

            result.close();
            recognizer.close();
            speechConfig.close();
        }
        catch (Exception ex)
        {
            Log.e("EmbeddedSpeechSamples", "Exception caught: " + ex.getMessage());
            assert(false);
        }
    }
}
