//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.samples.sdkdemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.os.Bundle;
import android.text.TextUtils;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.github.difflib.DiffUtils;

import com.github.difflib.patch.AbstractDelta;
import com.github.difflib.patch.DeltaType;
import com.github.difflib.patch.Patch;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentConfig;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentGradingSystem;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentGranularity;
import com.microsoft.cognitiveservices.speech.PronunciationAssessmentResult;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.WordLevelTimingResult;
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import static android.Manifest.permission.INTERNET;
import static android.Manifest.permission.READ_EXTERNAL_STORAGE;
import static android.Manifest.permission.RECORD_AUDIO;

public class MainActivity extends AppCompatActivity {

    //
    // Configuration for speech recognition
    //

    // Replace below with your own subscription key
    private static final String SpeechSubscriptionKey = "YourSubscriptionKey";
    // Replace below with your own service region (e.g., "westus").
    private static final String SpeechRegion = "YourServiceRegion";

    private TextView recognizedTextView;

    private Button pronunciationAssessmentButton;
    private Button pronunciationAssessmentFromStreamButton;

    private MicrophoneStream microphoneStream;
    private MicrophoneStream createMicrophoneStream() {
        this.releaseMicrophoneStream();

        microphoneStream = new MicrophoneStream();
        return microphoneStream;
    }
    private void releaseMicrophoneStream() {
        if (microphoneStream != null) {
            microphoneStream.close();
            microphoneStream = null;
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        recognizedTextView = findViewById(R.id.recognizedText);
        recognizedTextView.setMovementMethod(new ScrollingMovementMethod());

        pronunciationAssessmentButton = findViewById(R.id.buttonPronunciationAssessment);
        pronunciationAssessmentFromStreamButton = findViewById(R.id.buttonPronunciationAssessmentStream);

        // Initialize SpeechSDK and request required permissions.
        try {
            // a unique number within the application to allow
            // correlating permission request responses with the request.
            int permissionRequestId = 5;

            // Request permissions needed for speech recognition
            ActivityCompat.requestPermissions(MainActivity.this, new String[]{RECORD_AUDIO, INTERNET, READ_EXTERNAL_STORAGE}, permissionRequestId);
        }
        catch(Exception ex) {
            Log.e("SpeechSDK", "could not init sdk, " + ex.toString());
            recognizedTextView.setText("Could not initialize: " + ex.toString());
        }

        // create config
        final SpeechConfig speechConfig;
        try {
            speechConfig = SpeechConfig.fromSubscription(SpeechSubscriptionKey, SpeechRegion);
        } catch (Exception ex) {
            System.out.println(ex.getMessage());
            displayException(ex);
            return;
        }

        ///////////////////////////////////////////////////
        // pronunciation assessment from file
        ///////////////////////////////////////////////////
        pronunciationAssessmentButton.setOnClickListener(view ->  {
            final String logTag = "pron";

            disableButtons();
            clearTextBox();

            try {
                final AudioConfig audioConfig = AudioConfig.fromWavFileInput(copyAssetToCacheAndGetFilePath("pronunciation_assessment.wav"));
                // Switch to other languages for example Spanish, change language "en-US" to "es-ES". Language name is not case sensitive.
                final SpeechRecognizer reco = new SpeechRecognizer(speechConfig, "en-US", audioConfig);

                String referenceText =  "Hello world hello. Today is a nice day.";
                PronunciationAssessmentConfig pronConfig =
                    new PronunciationAssessmentConfig(referenceText,
                        PronunciationAssessmentGradingSystem.HundredMark,
                        PronunciationAssessmentGranularity.Phoneme);

                pronConfig.applyTo(reco);

                ArrayList<String> recognizedWords = new ArrayList<>();
                ArrayList<Word> pronWords = new ArrayList<>();
                long[] totalDurations = new long[2]; // duration, valid duration
                long[] offsets = new long[2]; // start offset, end offset
                double[] totalAccuracyScore = new double[1];
                reco.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
                    final String s = speechRecognitionResultEventArgs.getResult().getText();
                    Log.i(logTag, "Final result received: " + s);
                    PronunciationAssessmentResult pronResult = PronunciationAssessmentResult.fromResult(speechRecognitionResultEventArgs.getResult());
                    Log.i(logTag, "Accuracy score: " + pronResult.getAccuracyScore() +
                        ";  pronunciation score: " +  pronResult.getPronunciationScore() +
                        ", completeness score: " + pronResult.getCompletenessScore() +
                        ", fluency score: " + pronResult.getFluencyScore());
                    String jString = speechRecognitionResultEventArgs.getResult().getProperties().getProperty(PropertyId.SpeechServiceResponse_JsonResult);

                    for (WordLevelTimingResult w: pronResult.getWords()) {
                        Word word = new Word(w.getWord(),
                            w.getErrorType(),
                            w.getAccuracyScore(),
                            w.getDuration() / 10000, // The time unit is ticks (100ns); convert it to milliseconds
                            w.getOffset() / 10000); // Same as above
                        pronWords.add(word);
                        recognizedWords.add(word.word);
                        totalAccuracyScore[0] += word.duration * word.accuracyScore;
                        totalDurations[0] += word.duration;
                        if (word.errorType != null && word.errorType.equals("None")) {
                            totalDurations[1] += word.duration + 10;
                        }
                        offsets[1] = word.offset + word.duration + 10;
                    }
                });

                reco.sessionStopped.addEventListener((o, s) -> {
                    Log.i(logTag, "Session stopped.");
                    reco.stopContinuousRecognitionAsync();

                    // We can calculate whole accuracy score by duration weighted averaging
                    double accuracyScore = totalAccuracyScore[0] / totalDurations[0];
                    double fluencyScore = 0;
                    if (!recognizedWords.isEmpty()) {
                        offsets[0] = pronWords.get(0).offset;
                        fluencyScore = (double)totalDurations[1] / (offsets[1] - offsets[0]) * 100;
                    }

                    // For continuous pronunciation assessment mode, the service won't return the words with `Insertion` or `Omission`
                    // even if miscue is enabled.
                    // We need to compare with the reference text after received all recognized words to get these error words.
                    String[] referenceWords = referenceText.toLowerCase().split(" ");
                    for (int j = 0; j < referenceWords.length; j++) {
                        referenceWords[j] = referenceWords[j].replaceAll("^\\p{Punct}+|\\p{Punct}+$","");
                    }
                    Patch<String> diff = DiffUtils.diff(Arrays.asList(referenceWords), recognizedWords, true);

                    int currentIdx = 0;
                    ArrayList<Word> finalWords = new ArrayList<>();
                    int[] validWord = new int[1];
                    for (AbstractDelta<String> d : diff.getDeltas()) {
                        if (d.getType() == DeltaType.EQUAL) {
                            for (int i = currentIdx; i < currentIdx + d.getSource().size(); i++) {
                                finalWords.add(pronWords.get(i));
                            }
                            currentIdx += d.getTarget().size();
                            validWord[0] += d.getTarget().size();
                        }
                        if (d.getType() == DeltaType.DELETE || d.getType() == DeltaType.CHANGE) {
                            for (String w : d.getSource().getLines()) {
                                finalWords.add(new Word(w, "Omission"));
                            }
                        }
                        if (d.getType() == DeltaType.INSERT || d.getType() == DeltaType.CHANGE) {
                            for (int i = currentIdx; i < currentIdx + d.getTarget().size(); i++) {
                                Word w = pronWords.get(i);
                                w.errorType = "Insertion";
                                finalWords.add(w);
                            }
                            currentIdx += d.getTarget().size();
                        }
                    }

                    // Calculate whole completeness score
                    double completenessScore = (double)validWord[0] / referenceWords.length * 100;

                    AppendTextLine("Paragraph accuracy score: " + accuracyScore +
                        ", completeness score: " +completenessScore +
                        " , fluency score: " + fluencyScore + "\n", true);
                    for (Word w : finalWords) {
                        AppendTextLine(" word: " + w.word + "\taccuracy score: " +
                            w.accuracyScore + "\terror type: " + w.errorType, false);
                    }

                    enableButtons();
                });

                reco.startContinuousRecognitionAsync();
            } catch (Exception ex) {
                System.out.println(ex.getMessage());
                displayException(ex);
            }
        });

        ///////////////////////////////////////////////////
        // pronunciation assessment from stream
        ///////////////////////////////////////////////////
        pronunciationAssessmentFromStreamButton.setOnClickListener(view -> {
            final String logTag = "pron";

            if (this.microphoneStream != null) {
                /*
                 The pronunciation assessment service has a longer default end silence timeout (5 seconds) than normal STT
                 as the pronunciation assessment is widely used in education scenario where kids have longer break in reading.
                 You can explicitly stop the recording stream to stop assessment and the service will return the results immediately
                 */
                this.releaseMicrophoneStream();
                return;
            }

            disableButtons();
            this.pronunciationAssessmentFromStreamButton.setEnabled(true);
            this.pronunciationAssessmentFromStreamButton.setText("Stop recording");
            clearTextBox();
            try {
                 createMicrophoneStream();
                final AudioConfig audioConfig = AudioConfig.fromStreamInput(createMicrophoneStream());
                // Switch to other languages for example Spanish, change language "en-US" to "es-ES". Language name is not case sensitive.
                final SpeechRecognizer reco = new SpeechRecognizer(speechConfig, "en-US", audioConfig);

                // Replace this referenceText to match your input.
                String referenceText =  "Today is a nice day.";
                PronunciationAssessmentConfig pronConfig =
                    new PronunciationAssessmentConfig(referenceText,
                        PronunciationAssessmentGradingSystem.HundredMark,
                        PronunciationAssessmentGranularity.Phoneme);

                pronConfig.applyTo(reco);

                reco.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
                    final String s = speechRecognitionResultEventArgs.getResult().getText();
                    AppendTextLine("Final result received: " + s, true);
                    PronunciationAssessmentResult pronResult = PronunciationAssessmentResult.fromResult(speechRecognitionResultEventArgs.getResult());
                    AppendTextLine("Accuracy score: " + pronResult.getAccuracyScore() +
                        ";  pronunciation score: " +  pronResult.getPronunciationScore() +
                        ", completeness score: " + pronResult.getCompletenessScore() +
                        ", fluency score: " + pronResult.getFluencyScore(), false);
                });

                reco.sessionStopped.addEventListener((o, s) -> {
                    Log.i(logTag, "Session stopped.");
                    reco.stopContinuousRecognitionAsync();

                    this.releaseMicrophoneStream();
                    enableButtons();
                    this.pronunciationAssessmentFromStreamButton.setText("Pronunciation Assessment From Stream");
                });

                reco.recognizeOnceAsync();
            } catch (Exception ex) {
                System.out.println(ex.getMessage());
                displayException(ex);
            }
        });
    }


    private void displayException(Exception ex) {
        recognizedTextView.setText(ex.getMessage() + System.lineSeparator() + TextUtils.join(System.lineSeparator(), ex.getStackTrace()));
    }

    private void clearTextBox() {
        AppendTextLine("", true);
    }

    private void AppendTextLine(final String s, final Boolean erase) {
        MainActivity.this.runOnUiThread(() -> {
            if (erase) {
                recognizedTextView.setText(s);
            } else {
                String txt = recognizedTextView.getText().toString();
                recognizedTextView.setText(txt + System.lineSeparator() + s);
            }
        });
    }

    private void disableButtons() {
        MainActivity.this.runOnUiThread(() -> {
            pronunciationAssessmentButton.setEnabled(false);
            pronunciationAssessmentFromStreamButton.setEnabled(false);
        });
    }

    private void enableButtons() {
        MainActivity.this.runOnUiThread(() -> {
            pronunciationAssessmentButton.setEnabled(true);
            pronunciationAssessmentFromStreamButton.setEnabled(true);
        });
    }

    private interface OnTaskCompletedListener<T> {
        void onCompleted(T taskResult);
    }

    private String copyAssetToCacheAndGetFilePath(String filename) {
        File cacheFile = new File(getCacheDir() + "/" + filename);
        if (!cacheFile.exists()) {
            try {
                InputStream is = getAssets().open(filename);
                int size = is.available();
                byte[] buffer = new byte[size];
                is.read(buffer);
                is.close();
                FileOutputStream fos = new FileOutputStream(cacheFile);
                fos.write(buffer);
                fos.close();
            }
            catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
        return cacheFile.getPath();
    }

    private static ExecutorService s_executorService;
    static {
        s_executorService = Executors.newCachedThreadPool();
    }

    public class Word {
        public String word;
        public String errorType;
        public double accuracyScore;
        public long duration;
        public long offset;
        public Word(String word, String errorType) {
            this.word = word;
            this.errorType = errorType;
        }

        public Word(String word, String errorType, double accuracyScore, long duration, long offset) {
            this(word, errorType);
            this.accuracyScore = accuracyScore;
            this.duration = duration;
            this.offset = offset;
        }
    }
}
