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
import android.widget.Button;
import android.widget.TextView;

import com.microsoft.cognitiveservices.speech.AudioDataStream;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.KeywordRecognizer;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionResult;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.audio.PushAudioInputStream;

import java.io.InputStream;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicBoolean;

import static android.Manifest.permission.*;

public class MainActivity extends AppCompatActivity {

    private ExecutorService es = Executors.newFixedThreadPool(3);

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
    }

    void enableButton(final boolean enabled) {
        final Button btn = (Button) this.findViewById(R.id.button);
        runOnUiThread(() -> {
            btn.setEnabled(enabled);
        });
    }

    void updateText(final String text) {
        final TextView txt = (TextView) this.findViewById(R.id.hello); // 'hello' is the ID of your text view
        runOnUiThread(() -> {
            txt.setText(text);
        });

    }

    final AtomicBoolean stopPumping = new AtomicBoolean();
    private Runnable pump(final AudioDataStream src, final PushAudioInputStream dst)
    {
        return () -> {
            try {
                final byte[] buffer = new byte[300];
                while (!stopPumping.get()) {
                    if (!src.canReadData(300)) {
                        continue;
                    }
                    long count = src.readData(buffer);
                    if (count == 0) {
                        break;
                    }
                    if (count < 300) {
                        dst.write(Arrays.copyOf(buffer, (int) count));
                    } else {
                        dst.write(buffer);
                    }
                }
            } catch (Exception ex) {
                Log.e("SpeechSDKDemo", "Pump got an exception");
            }
        };
    }

    public void onSpeechButtonClicked(View v) {
        enableButton(false);
        updateText("Say \"Computer\"");
        Runnable asyncTask = () -> {
            try (AssetManager am = getAssets();
                 AudioConfig config = AudioConfig.fromDefaultMicrophoneInput();
                 KeywordRecognizer reco = new KeywordRecognizer(config);) {

                InputStream is = am.open("kws.table");
                KeywordRecognitionModel model = KeywordRecognitionModel.fromStream(is, "computer", false);
                Future<KeywordRecognitionResult> task = reco.recognizeOnceAsync(model);

                // Note: this will block the UI thread, so eventually, you want to
                //       register for the event (see full samples)
                KeywordRecognitionResult result = task.get();

                if (result.getReason() == ResultReason.RecognizedKeyword) {
                    updateText("Recognized " + result.getText());
                }
                else {
                    updateText("Error: got the wrong sort of recognition.");
                    enableButton(true);
                    return;
                }
                // Get audio data from the KeywordRecognitionResult and transcribe it from speech to text
                try (AudioDataStream audioDataStream = AudioDataStream.fromResult(result);
                     PushAudioInputStream srInputStream = PushAudioInputStream.create();
                     AudioConfig srAudioConfig = AudioConfig.fromStreamInput(srInputStream);
                     SpeechConfig srConfig = SpeechConfig.fromSubscription(speechSubscriptionKey, serviceRegion);
                     SpeechRecognizer speechRecognizer = new SpeechRecognizer(srConfig, srAudioConfig);) {

                    /* We pump the audio into the push stream */
                    stopPumping.set(false);
                    es.execute(pump(audioDataStream, srInputStream));
                    SpeechRecognitionResult srResult = speechRecognizer.recognizeOnceAsync().get();
                    stopPumping.set(true);
                    if (srResult.getReason() == ResultReason.RecognizedSpeech) {
                        updateText("Recognized Speech " + srResult.getText());
                    }
                    else {
                        updateText("Error: got the wrong sort of recognition.");
                    }
                }

            } catch (Exception ex) {
                Log.e("SpeechSDKDemo", "unexpected " + ex.getMessage());
                enableButton(true);
                assert(false);
            }
            enableButton(true);
        };
        es.execute(asyncTask);
    }
}
// </code>
