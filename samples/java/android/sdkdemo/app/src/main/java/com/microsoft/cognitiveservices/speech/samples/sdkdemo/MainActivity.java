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
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;
import com.microsoft.cognitiveservices.speech.ResultReason;
import com.microsoft.cognitiveservices.speech.intent.LanguageUnderstandingModel;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognitionResult;
import com.microsoft.cognitiveservices.speech.intent.IntentRecognizer;
import com.microsoft.cognitiveservices.speech.SpeechRecognitionResult;
import com.microsoft.cognitiveservices.speech.SpeechRecognizer;
import com.microsoft.cognitiveservices.speech.CancellationDetails;
import com.microsoft.cognitiveservices.speech.KeywordRecognitionModel;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

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

    //
    // Configuration for intent recognition
    //

    // Replace below with your own Language Understanding subscription key
    // The intent recognition service calls the required key 'endpoint key'.
    private static final String LanguageUnderstandingSubscriptionKey = "YourLanguageUnderstandingSubscriptionKey";
    // Replace below with the deployment region of your Language Understanding application
    private static final String LanguageUnderstandingServiceRegion = "YourLanguageUnderstandingServiceRegion";
    // Replace below with the application ID of your Language Understanding application
    private static final String LanguageUnderstandingAppId = "YourLanguageUnderstandingAppId";
    // Replace below with your own Keyword model file, kws.table model file is configured for "Computer" keyword
    private static final String KwsModelFile = "kws.table";

    private TextView recognizedTextView;

    private Button recognizeButton;
    private Button recognizeIntermediateButton;
    private Button recognizeContinuousButton;
    private Button recognizeIntentButton;
    private Button recognizeWithKeywordButton;
    private Button pronunciationAssessmentButton;
    private Button pronunciationAssessmentFromStreamButton;

    private MicrophoneStream microphoneStream;
    private MicrophoneStream createMicrophoneStream() {
        if (microphoneStream != null) {
            microphoneStream.close();
            microphoneStream = null;
        }

        microphoneStream = new MicrophoneStream();
        return microphoneStream;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        recognizedTextView = findViewById(R.id.recognizedText);
        recognizedTextView.setMovementMethod(new ScrollingMovementMethod());

        recognizeButton = findViewById(R.id.buttonRecognize);
        recognizeIntermediateButton = findViewById(R.id.buttonRecognizeIntermediate);
        recognizeContinuousButton = findViewById(R.id.buttonRecognizeContinuous);
        recognizeIntentButton = findViewById(R.id.buttonRecognizeIntent);
        recognizeWithKeywordButton = findViewById(R.id.buttonRecognizeWithKeyword);
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
        final KeywordRecognitionModel kwsModel;
        try {
            speechConfig = SpeechConfig.fromSubscription(SpeechSubscriptionKey, SpeechRegion);
            kwsModel = KeywordRecognitionModel.fromFile(copyAssetToCacheAndGetFilePath(KwsModelFile));
        } catch (Exception ex) {
            System.out.println(ex.getMessage());
            displayException(ex);
            return;
        }

        ///////////////////////////////////////////////////
        // recognize
        ///////////////////////////////////////////////////
        recognizeButton.setOnClickListener(view -> {
            final String logTag = "reco 1";

            disableButtons();
            clearTextBox();

            try {
                // final AudioConfig audioInput = AudioConfig.fromDefaultMicrophoneInput();
                final AudioConfig audioInput = AudioConfig.fromStreamInput(createMicrophoneStream());
                final SpeechRecognizer reco = new SpeechRecognizer(speechConfig, audioInput);

                final Future<SpeechRecognitionResult> task = reco.recognizeOnceAsync();
                setOnTaskCompletedListener(task, result -> {
                    String s = result.getText();
                    if (result.getReason() != ResultReason.RecognizedSpeech) {
                        String errorDetails = (result.getReason() == ResultReason.Canceled) ? CancellationDetails.fromResult(result).getErrorDetails() : "";
                        s = "Recognition failed with " + result.getReason() + ". Did you enter your subscription?" + System.lineSeparator() + errorDetails;
                    }

                    reco.close();
                    Log.i(logTag, "Recognizer returned: " + s);
                    setRecognizedText(s);
                    enableButtons();
                });
            } catch (Exception ex) {
                System.out.println(ex.getMessage());
                displayException(ex);
            }
        });

        ///////////////////////////////////////////////////
        // recognize with intermediate results
        ///////////////////////////////////////////////////
        recognizeIntermediateButton.setOnClickListener(view -> {
            final String logTag = "reco 2";

            disableButtons();
            clearTextBox();

            try {
                // final AudioConfig audioInput = AudioConfig.fromDefaultMicrophoneInput();
                final AudioConfig audioInput = AudioConfig.fromStreamInput(createMicrophoneStream());
                final SpeechRecognizer reco = new SpeechRecognizer(speechConfig, audioInput);

                reco.recognizing.addEventListener((o, speechRecognitionResultEventArgs) -> {
                    final String s = speechRecognitionResultEventArgs.getResult().getText();
                    Log.i(logTag, "Intermediate result received: " + s);
                    setRecognizedText(s);
                });

                final Future<SpeechRecognitionResult> task = reco.recognizeOnceAsync();
                setOnTaskCompletedListener(task, result -> {
                    final String s = result.getText();
                    reco.close();
                    Log.i(logTag, "Recognizer returned: " + s);
                    setRecognizedText(s);
                    enableButtons();
                });
            } catch (Exception ex) {
                System.out.println(ex.getMessage());
                displayException(ex);
            }
        });

        ///////////////////////////////////////////////////
        // recognize continuously
        ///////////////////////////////////////////////////
        recognizeContinuousButton.setOnClickListener(new View.OnClickListener() {
            private static final String logTag = "reco 3";
            private boolean continuousListeningStarted = false;
            private SpeechRecognizer reco = null;
            private AudioConfig audioInput = null;
            private String buttonText = "";
            private ArrayList<String> content = new ArrayList<>();

            @Override
            public void onClick(final View view) {
                final Button clickedButton = (Button) view;
                disableButtons();
                if (continuousListeningStarted) {
                    if (reco != null) {
                        final Future<Void> task = reco.stopContinuousRecognitionAsync();
                        setOnTaskCompletedListener(task, result -> {
                            Log.i(logTag, "Continuous recognition stopped.");
                            MainActivity.this.runOnUiThread(() -> {
                                clickedButton.setText(buttonText);
                            });
                            enableButtons();
                            continuousListeningStarted = false;
                        });
                    } else {
                        continuousListeningStarted = false;
                    }

                    return;
                }

                clearTextBox();

                try {
                    content.clear();

                    // audioInput = AudioConfig.fromDefaultMicrophoneInput();
                    audioInput = AudioConfig.fromStreamInput(createMicrophoneStream());
                    reco = new SpeechRecognizer(speechConfig, audioInput);

                    reco.recognizing.addEventListener((o, speechRecognitionResultEventArgs) -> {
                        final String s = speechRecognitionResultEventArgs.getResult().getText();
                        Log.i(logTag, "Intermediate result received: " + s);
                        content.add(s);
                        setRecognizedText(TextUtils.join(" ", content));
                        content.remove(content.size() - 1);
                    });

                    reco.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
                        final String s = speechRecognitionResultEventArgs.getResult().getText();
                        Log.i(logTag, "Final result received: " + s);
                        content.add(s);
                        setRecognizedText(TextUtils.join(" ", content));
                    });

                    final Future<Void> task = reco.startContinuousRecognitionAsync();
                    setOnTaskCompletedListener(task, result -> {
                        continuousListeningStarted = true;
                        MainActivity.this.runOnUiThread(() -> {
                            buttonText = clickedButton.getText().toString();
                            clickedButton.setText("Stop");
                            clickedButton.setEnabled(true);
                        });
                    });
                } catch (Exception ex) {
                    System.out.println(ex.getMessage());
                    displayException(ex);
                }
            }
        });

        ///////////////////////////////////////////////////
        // recognize intent
        ///////////////////////////////////////////////////
        recognizeIntentButton.setOnClickListener(view -> {
            final String logTag = "intent";
            final ArrayList<String> content = new ArrayList<>();

            disableButtons();
            clearTextBox();

            content.add("");
            content.add("");
            try {
                final SpeechConfig intentConfig = SpeechConfig.fromSubscription(LanguageUnderstandingSubscriptionKey, LanguageUnderstandingServiceRegion);

                // final AudioConfig audioInput = AudioConfig.fromDefaultMicrophoneInput();
                final AudioConfig audioInput = AudioConfig.fromStreamInput(createMicrophoneStream());
                final IntentRecognizer reco = new IntentRecognizer(intentConfig, audioInput);

                LanguageUnderstandingModel intentModel = LanguageUnderstandingModel.fromAppId(LanguageUnderstandingAppId);
                reco.addAllIntents(intentModel);

                reco.recognizing.addEventListener((o, intentRecognitionResultEventArgs) -> {
                    final String s = intentRecognitionResultEventArgs.getResult().getText();
                    Log.i(logTag, "Intermediate result received: " + s);
                    content.set(0, s);
                    setRecognizedText(TextUtils.join(System.lineSeparator(), content));
                });

                final Future<IntentRecognitionResult> task = reco.recognizeOnceAsync();
                setOnTaskCompletedListener(task, result -> {
                    Log.i(logTag, "Continuous recognition stopped.");
                    String s = result.getText();

                    if (result.getReason() != ResultReason.RecognizedIntent) {
                        String errorDetails = (result.getReason() == ResultReason.Canceled) ? CancellationDetails.fromResult(result).getErrorDetails() : "";
                        s = "Intent failed with " + result.getReason() + ". Did you enter your Language Understanding subscription?" + System.lineSeparator() + errorDetails;
                    }

                    String intentId = result.getIntentId();
                    Log.i(logTag, "Final result received: " + s + ", intent: " + intentId);
                    content.set(0, s);
                    content.set(1, " [intent: " + intentId + "]");
                    setRecognizedText(TextUtils.join(System.lineSeparator(), content));
                    enableButtons();
                });
            } catch (Exception ex) {
                System.out.println(ex.getMessage());
                displayException(ex);
            }
        });

        ///////////////////////////////////////////////////
        // recognize with keyword
        ///////////////////////////////////////////////////
        recognizeWithKeywordButton.setOnClickListener(new View.OnClickListener() {
            private static final String logTag = "keyword";
            private boolean continuousListeningStarted = false;
            private SpeechRecognizer reco = null;
            private AudioConfig audioInput = null;
            private String buttonText = "";
            private ArrayList<String> content = new ArrayList<>();

            @Override
            public void onClick(final View view) {
                final Button clickedButton = (Button) view;
                disableButtons();
                if (continuousListeningStarted) {
                    if (reco != null) {
                        final Future<Void> task = reco.stopKeywordRecognitionAsync();
                        setOnTaskCompletedListener(task, result -> {
                            Log.i(logTag, "Continuous recognition stopped.");
                            MainActivity.this.runOnUiThread(() -> {
                                clickedButton.setText(buttonText);
                            });
                            enableButtons();
                            continuousListeningStarted = false;
                        });
                    } else {
                        continuousListeningStarted = false;
                    }

                    return;
                }

                clearTextBox();

                try {
                    content.clear();

                    // audioInput = AudioConfig.fromDefaultMicrophoneInput();
                    audioInput = AudioConfig.fromStreamInput(createMicrophoneStream());
                    reco = new SpeechRecognizer(speechConfig, audioInput);

                    reco.recognizing.addEventListener((o, speechRecognitionResultEventArgs) -> {
                        final String s = speechRecognitionResultEventArgs.getResult().getText();
                        Log.i(logTag, "Intermediate result received: " + s);
                        content.add(s);
                        setRecognizedText(TextUtils.join(" ", content));
                        content.remove(content.size() - 1);
                    });

                    reco.recognized.addEventListener((o, speechRecognitionResultEventArgs) -> {
                        final String s;
                        if (speechRecognitionResultEventArgs.getResult().getReason() == ResultReason.RecognizedKeyword)
                        {
                            s = "Keyword: " + speechRecognitionResultEventArgs.getResult().getText();
                            Log.i(logTag, "Keyword recognized result received: " + s);
                        }
                        else
                        {
                            s = "Recognized: " + speechRecognitionResultEventArgs.getResult().getText();
                            Log.i(logTag, "Final result received: " + s);
                        }
                        content.add(s);
                        setRecognizedText(TextUtils.join(" ", content));
                    });

                    final Future<Void> task = reco.startKeywordRecognitionAsync(kwsModel);
                    setOnTaskCompletedListener(task, result -> {
                        continuousListeningStarted = true;
                        MainActivity.this.runOnUiThread(() -> {
                            buttonText = clickedButton.getText().toString();
                            clickedButton.setText("Stop");
                            clickedButton.setEnabled(true);
                        });
                    });
                } catch (Exception ex) {
                    System.out.println(ex.getMessage());
                    displayException(ex);
                }
            }
        });


        ///////////////////////////////////////////////////
        // pronunciation assessment from file
        ///////////////////////////////////////////////////
        pronunciationAssessmentButton.setOnClickListener(view ->  {
            final String logTag = "pron";

            disableButtons();
            clearTextBox();

            try {
                final AudioConfig audioConfig = AudioConfig.fromWavFileInput(copyAssetToCacheAndGetFilePath("pronunciation_assessment.wav"));
                final SpeechRecognizer reco = new SpeechRecognizer(speechConfig, audioConfig);

                String referenceText =  "Hello world hello. Today is a nice day.";
                PronunciationAssessmentConfig pronConfig =
                    new PronunciationAssessmentConfig(referenceText,
                        PronunciationAssessmentGradingSystem.HundredMark,
                        PronunciationAssessmentGranularity.Phoneme);

                pronConfig.applyTo(reco);

                ArrayList<String> recognizedWords = new ArrayList<>();
                ArrayList<Word> pronWords = new ArrayList<>();
                int[] totalDurations = new int[2]; // duration, valid duration
                int[] offsets = new int[2]; // start offset, end offset
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
                    try {
                        JSONObject json = new JSONObject(jString);
                        JSONArray currentWords = json.getJSONArray("NBest").getJSONObject(0).getJSONArray("Words");
                        for (int j = 0; j < currentWords.length(); j++) {
                            JSONObject w = currentWords.getJSONObject(j);
                            JSONObject pron = w.getJSONObject("PronunciationAssessment");
                            Word word = new Word(w.getString("Word"),
                                pron.getString("ErrorType"),
                                pron.getDouble("AccuracyScore"),
                                w.getInt("Duration"),
                                w.getInt("Offset"));
                            pronWords.add(word);
                            recognizedWords.add(word.word);
                            totalAccuracyScore[0] += word.duration * word.accuracyScore;
                            totalDurations[0] += word.duration;
                            if (word.errorType != null && word.errorType.equals("None")) {
                                totalDurations[1] += word.duration + 100000;
                            }
                            offsets[1] = word.offset + word.duration + 100000;
                        }
                    } catch (JSONException e) {
                        System.out.println(e.getMessage());
                        displayException(e);
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

            disableButtons();
            clearTextBox();
            try {
                final AudioConfig audioConfig = AudioConfig.fromStreamInput(createMicrophoneStream());
                final SpeechRecognizer reco = new SpeechRecognizer(speechConfig, audioConfig);

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

                    enableButtons();
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

    private void setRecognizedText(final String s) {
        AppendTextLine(s, true);
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
            recognizeButton.setEnabled(false);
            recognizeIntermediateButton.setEnabled(false);
            recognizeContinuousButton.setEnabled(false);
            recognizeIntentButton.setEnabled(false);
            recognizeWithKeywordButton.setEnabled(false);
            pronunciationAssessmentButton.setEnabled(false);
            pronunciationAssessmentFromStreamButton.setEnabled(false);
        });
    }

    private void enableButtons() {
        MainActivity.this.runOnUiThread(() -> {
            recognizeButton.setEnabled(true);
            recognizeIntermediateButton.setEnabled(true);
            recognizeContinuousButton.setEnabled(true);
            recognizeIntentButton.setEnabled(true);
            recognizeWithKeywordButton.setEnabled(true);
            pronunciationAssessmentButton.setEnabled(true);
            pronunciationAssessmentFromStreamButton.setEnabled(true);
        });
    }

    private <T> void setOnTaskCompletedListener(Future<T> task, OnTaskCompletedListener<T> listener) {
        s_executorService.submit(() -> {
            T result = task.get();
            listener.onCompleted(result);
            return null;
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
        public int duration;
        public int offset;
        public Word(String word, String errorType) {
            this.word = word;
            this.errorType = errorType;
        }

        public Word(String word, String errorType, double accuracyScore, int duration, int offset) {
            this(word, errorType);
            this.accuracyScore = accuracyScore;
            this.duration = duration;
            this.offset = offset;
        }
    }
}
