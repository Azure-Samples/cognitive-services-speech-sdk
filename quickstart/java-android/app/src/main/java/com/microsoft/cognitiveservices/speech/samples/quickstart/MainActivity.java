//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// <code>
package com.microsoft.cognitiveservices.speech.samples.quickstart;

import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.RecognitionStatus;
import com.microsoft.cognitiveservices.speech.SpeechFactory;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import java.util.concurrent.Future;

import static android.Manifest.permission.*;

public class MainActivity extends AppCompatActivity {

    // Replace below with your own subscription key
    private static String speechSubscriptionKey = "YourSubscriptionKey";
    // Replace below with your own service region (e.g., "westus").
    private static String serviceRegion = "YourServiceRegion";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Note: we need to request the permissions
        int requestCode = 5; // unique code for the permission request
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{RECORD_AUDIO, INTERNET}, requestCode);

        try {
            // Note: Configure native platform binding. This currently configures the directory
            //       in which to store certificates required to access the speech service.
            //       It is required to call this once after app start.
            SpeechFactory.configureNativePlatformBindingWithDefaultCertificate();
        } catch (Exception ex) {
            Log.e("SpeechSDKDemo", "unexpected " + ex.getMessage());
            assert(false);
        }
    }

    public void onSpeechButtonClicked(View v) {
        TextView txt = (TextView) this.findViewById(R.id.hello); // 'hello' is the ID of your text view

        try {
            SpeechFactory factory = SpeechFactory.fromSubscription(speechSubscriptionKey, serviceRegion);
            assert(factory!= null);

            SpeechRecognizer reco = factory.createSpeechRecognizer();
            assert(reco != null);

            Future<SpeechRecognitionResult> task = reco.recognizeAsync();
            assert(task != null);

            // Note: this will block the UI thread, so eventually, you want to
            //        register for the event (see full samples)
            SpeechRecognitionResult result = task.get();
            assert(result != null);

            if (result.getReason() == RecognitionStatus.Recognized) {
                txt.setText(result.toString());
            }
            else {
                txt.setText("Error recognizing. Did you update the subscription info?" + System.lineSeparator() + result.toString());
            }

            reco.close();
            factory.close();
        } catch (Exception ex) {
            Log.e("SpeechSDKDemo", "unexpected " + ex.getMessage());
            assert(false);
        }
    }
}
// </code>
