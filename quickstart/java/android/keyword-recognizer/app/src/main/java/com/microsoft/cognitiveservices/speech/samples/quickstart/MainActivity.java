//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package com.microsoft.cognitiveservices.speech.samples.quickstart;

import android.content.res.AssetManager;
import androidx.core.app.ActivityCompat;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.KeywordRecognizer;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionResult;
import com.microsoft.cognitiveservices.speech.ResultReason;

import java.io.InputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import static android.Manifest.permission.*;

public class MainActivity extends AppCompatActivity {

    private ExecutorService es = Executors.newFixedThreadPool(2);
    private AssetManager assetManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        assetManager = getAssets();

        // Note: we need to request the permissions
        int requestCode = 5; // unique code for the permission request
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{RECORD_AUDIO, INTERNET}, requestCode);
    }

    public void onSpeechButtonClicked(View v) {
        TextView txt = (TextView) this.findViewById(R.id.hello); // 'hello' is the ID of your text view
        txt.setText("Say \"Computer\"");

        Runnable asyncTask = () -> {
            try (AudioConfig config = AudioConfig.fromDefaultMicrophoneInput();
                 KeywordRecognizer reco = new KeywordRecognizer(config);
                 InputStream is = assetManager.open("kws.table");
                 KeywordRecognitionModel model = KeywordRecognitionModel.fromStream(is, "computer", false);) {

                Future<KeywordRecognitionResult> task = reco.recognizeOnceAsync(model);
                // Note: this will block the UI thread, so eventually, you want to
                //       register for the event (see full samples)
                KeywordRecognitionResult result = task.get();

                if (result.getReason() == ResultReason.RecognizedKeyword) {
                    txt.setText("Recognized " + result.getText());
                }
                else {
                    txt.setText("Error: got the wrong sort of recognition.");
                }

            } catch (Exception ex) {
                Log.e("SpeechSDKDemo", "unexpected " + ex.getMessage());
                assert(false);
            }
        };
        es.execute(asyncTask);
    }
}
// </code>
