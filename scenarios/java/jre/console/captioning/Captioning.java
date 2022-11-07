//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// NOTE: Compile this sample with:
// javac Captioning.java -cp .;target\dependency\* -encoding UTF-8
// Without -encoding UTF-8, you might receive the error:
// unmappable character (0x81) for encoding windows-1252

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.AutoCloseable;
import java.lang.StringBuilder;
import java.net.URI;
import java.net.URISyntaxException;
import java.time.*;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.List;
import java.util.Optional;
import java.util.stream.*;
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;

public class Captioning
{
    UserConfig _userConfig;
    int _srtSequenceNumber = 1;
    Optional<Caption> _previousCaption = Optional.empty();
    Optional<Instant> _previousEndTime = Optional.empty();
    boolean _previousResultIsRecognized = false;
    List<String> _recognizedLines = new ArrayList<String>();
    List<SpeechRecognitionResult> _offlineResults = new ArrayList<SpeechRecognitionResult>();
    AudioStreamFormat _format;
    PullAudioInputStreamCallback _callback;
    PullAudioInputStream _stream;

    private void WriteToConsole(String text)
    {
        if (!_userConfig.getSuppressConsoleOutput())
        {
            System.out.print(text);
        }
    }

    private void WriteToConsoleOrFile(String text) throws IOException
    {
        WriteToConsole(text);
        if (_userConfig.getOutputFile().isPresent())
        {
            try
            {
                var outputFile = new FileWriter(_userConfig.getOutputFile().get(), true);
                outputFile.write(text);
                outputFile.close();
            }
            catch(IOException e)
            {
                throw e;
            }
        }
    }

    private String GetTimestamp(Instant startTime, Instant endTime)
    {
        var format = "";
        if (_userConfig.getUseSubRipTextCaptionFormat())
        {
            // SRT format requires ',' as decimal separator rather than '.'.
            format = "HH:mm:ss,SSS";
        }
        else
        {
            format = "HH:mm:ss.SSS";
        }
        // Set the timezone to UTC so the time is not adjusted for our local time zone.
        var formatter = DateTimeFormatter.ofPattern(format).withZone(ZoneId.from(ZoneOffset.UTC));
        return String.format("%s --> %s", formatter.format(startTime), formatter.format(endTime));
    }

    private String StringFromCaption(Caption caption)
    {
        var retval = new StringBuilder();
        if (_userConfig.getUseSubRipTextCaptionFormat())
        {
            retval.append(String.format("%d%s", caption.sequence, System.lineSeparator()));
        }
        retval.append(String.format("%s%s", GetTimestamp(caption.begin, caption.end), System.lineSeparator()));
        retval.append(String.format("%s%s%s", caption.text, System.lineSeparator(), System.lineSeparator()));
        return retval.toString();
    }

    private String AdjustRealTimeCaptionText(String text, boolean isRecognizedResult)
    {
        // Split the caption text into multiple lines based on maxLineLength and lines.
        var captionHelper = new CaptionHelper(Optional.of(_userConfig.getLanguage()), _userConfig.getMaxLineLength(), _userConfig.getLines(), new ArrayList<RecognitionResult>());
        List<String> lines = captionHelper.LinesFromText(text);

        // Recognizing results can change with each new result, so we do not save previous Recognizing results.
        // Recognized results are final, so we save them in a member value.
        List<String> recognizingLines = new ArrayList<String>();
        if (isRecognizedResult)
        {
            _recognizedLines.addAll(lines);
        }
        else
        {
            recognizingLines = lines;
        }

        List<String> combinedLines = Stream.concat(_recognizedLines.stream(), recognizingLines.stream()).toList();
        int combinedLinesSize = combinedLines.size();
        int takeLast = Math.min(combinedLinesSize, _userConfig.getLines());
        
        List<String> retval = combinedLines.subList(combinedLinesSize - takeLast, combinedLinesSize);

        return String.join("\n", retval);
    }

    private Optional<String> CaptionFromRealTimeResult(SpeechRecognitionResult result, boolean isRecognizedResult)
    {
        Optional<String> retval = Optional.empty();

        var startTime = CaptionHelper.InstantFromTicks(result.getOffset());
        var endTime = CaptionHelper.InstantFromTicks(result.getOffset().add(result.getDuration()));
        // If the end timestamp for the previous result is later
        // than the end timestamp for this result, drop the result.
        // This sometimes happens when we receive a lot of Recognizing results close together.
        if (_previousEndTime.isPresent() && _previousEndTime.get().compareTo(endTime) > 0)
        {
            // Do nothing.
        }
        else
        {
            // Record the end timestamp for this result.
            _previousEndTime = Optional.of(endTime);

            // Convert the SpeechRecognitionResult to a caption.
            // We are not ready to set the text for this caption.
            // First we need to determine whether to clear _recognizedLines.
            Caption caption = new Caption(Optional.of(_userConfig.getLanguage()), _srtSequenceNumber++, startTime.plusMillis(_userConfig.getDelay()), endTime.plusMillis(_userConfig.getDelay()), "");

            // If we have a previous caption...
            if (_previousCaption.isPresent())
            {
                // If the previous result was type Recognized...
                if (_previousResultIsRecognized)
                {
                    // Set the end timestamp for the previous caption to the earliest of:
                    // - The end timestamp for the previous caption plus the remain time.
                    // - The start timestamp for the current caption.
                    Instant previousEnd = _previousCaption.get().end.plusMillis(_userConfig.getRemainTime());
                    _previousCaption.get().end = previousEnd.compareTo(caption.begin) < 0 ? previousEnd : caption.begin;
                    // If the gap between the original end timestamp for the previous caption
                    // and the start timestamp for the current caption is larger than remainTime,
                    // clear the cached recognized lines.
                    // Note this needs to be done before we call AdjustRealTimeCaptionText
                    // for the current caption, because it uses _recognizedLines.
                    if (previousEnd.compareTo(caption.begin) < 0)
                    {
                        _recognizedLines.clear();
                    }
                }
                // If the previous result was type Recognizing, simply set the start timestamp
                // for the current caption to the end timestamp for the previous caption.
                // Note this presumes there will not be a large gap between Recognizing results,
                // because such a gap would cause the previous Recognizing result to be succeeded
                // by a Recognized result.
                else
                {
                    caption.begin = _previousCaption.get().end;
                }

                retval = Optional.of(StringFromCaption(_previousCaption.get()));
            }

            // Break the caption text into lines if needed.
            caption.text = AdjustRealTimeCaptionText(result.getText(), isRecognizedResult);
            // Save the current caption as the previous caption.
            _previousCaption = Optional.of(caption);
            // Save the result type as the previous result type.
            _previousResultIsRecognized = isRecognizedResult;
        }

        return retval;
    }

    private List<Caption> CaptionsFromOfflineResults()
    {
        // Although SpeechRecognitionResult extends RecognitionResult, we cannot pass
        // List<SpeechRecognitionResult> as a List<RecognitionResult>.
        // https://docs.oracle.com/javase/tutorial/java/generics/inheritance.html
        List<RecognitionResult> offlineResults = _offlineResults.stream().map(result -> (RecognitionResult)result).toList();
        List<Caption> captions = CaptionHelper.GetCaptions(Optional.of(_userConfig.getLanguage()), _userConfig.getMaxLineLength(), _userConfig.getLines(), offlineResults);

        // In offline mode, all captions come from RecognitionResults of type Recognized.
        // Set the end timestamp for each caption to the earliest of:
        // - The end timestamp for this caption plus the remain time.
        // - The start timestamp for the next caption.
        for (int index = 0; index < captions.size() - 1; index++)
        {
            Caption caption_1 = captions.get(index);
            Caption caption_2 = captions.get(index + 1);
            Instant end = caption_1.end.plusMillis(_userConfig.getRemainTime());
            caption_1.end = end.compareTo(caption_2.begin) < 0 ? end : caption_2.begin;
        }
        Caption lastCaption = captions.get(captions.size() - 1);
        lastCaption.end = lastCaption.end.plusMillis(_userConfig.getRemainTime());
        return captions;
    }

    private void Finish() throws IOException
    {
        if (CaptioningMode.Offline == _userConfig.getCaptioningMode())
        {
            for (Caption caption : CaptionsFromOfflineResults())
            {
                WriteToConsoleOrFile(StringFromCaption(caption));
            }
        }
        else if (CaptioningMode.RealTime == _userConfig.getCaptioningMode())
        {
            // Show the last "previous" caption, which is actually the last caption.
            if (_previousCaption.isPresent())
            {
                _previousCaption.get().end = _previousCaption.get().end.plusMillis(_userConfig.getRemainTime());
                WriteToConsoleOrFile(StringFromCaption(_previousCaption.get()));
            }
        }
    }

    private void Initialize(UserConfig userConfig) throws IOException
    {
        _userConfig = userConfig;
        
        if (_userConfig.getOutputFile().isPresent())
        {
            var outputFile = new File(_userConfig.getOutputFile().get());
            if (outputFile.exists())
            {
                outputFile.delete();
            }
        }

        if (!_userConfig.getUseSubRipTextCaptionFormat())
        {
            WriteToConsoleOrFile(String.format("WEBVTT%s%s", System.lineSeparator(), System.lineSeparator()));
        }
    }

    private AudioConfig AudioConfigFromUserConfig()
    {
        if (_userConfig.getInputFile().isPresent())
        {
            if (!_userConfig.getUseCompressedAudio())
            {
                var reader = new WavFileReader(_userConfig.getInputFile().get());
                _format = reader.getFormat();
                reader.Close();
            }
            else
            {
                _format = AudioStreamFormat.getCompressedFormat(_userConfig.getCompressedAudioFormat());
            }
            _callback = new BinaryFileReader(_userConfig.getInputFile().get());
            _stream = AudioInputStream.createPullStream(_callback, _format);
            return AudioConfig.fromStreamInput(_stream);
        }
        else
        {
            return AudioConfig.fromDefaultMicrophoneInput();
        }
    }

    private SpeechConfig SpeechConfigFromUserConfig()
    {
        final var speechConfig = SpeechConfig.fromSubscription(_userConfig.getSubscriptionKey(), _userConfig.getRegion());

        speechConfig.setProfanity(_userConfig.getProfanityOption());

        if (_userConfig.getStablePartialResultThreshold().isPresent())
        {
            speechConfig.setProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, _userConfig.getStablePartialResultThreshold().get());
        }
        
        speechConfig.setProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");
        speechConfig.setSpeechRecognitionLanguage(_userConfig.getLanguage());
        
        return speechConfig;
    }

    private SpeechRecognizer SpeechRecognizerFromUserConfig()
    {
        final AudioConfig audioConfig = AudioConfigFromUserConfig();
        final SpeechConfig speechConfig = SpeechConfigFromUserConfig();
        final var speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);
        
        if (_userConfig.getPhraseList().isPresent())
        {
            var grammar = PhraseListGrammar.fromRecognizer(speechRecognizer);
            for (String phrase : _userConfig.getPhraseList().get().split(";"))
            {
                grammar.addPhrase(phrase);
            }
        }
        
        return speechRecognizer;
    }

    private void RecognizeContinuous(SpeechRecognizer speechRecognizer) throws ExecutionException, InterruptedException
    {
        // This lets us modify local variables from inside a lambda.
        final var done = new boolean[] { false };

        // We only use Recognizing results in real-time mode.
        if (CaptioningMode.RealTime == _userConfig.getCaptioningMode())
        {
            speechRecognizer.recognizing.addEventListener((s, e) -> {
                try
                {
                    if (ResultReason.RecognizingSpeech == e.getResult().getReason() && e.getResult().getText().length() > 0)
                    {
                        Optional<String> caption = CaptionFromRealTimeResult(e.getResult(), false);
                        if (caption.isPresent())
                        {
                            WriteToConsoleOrFile(caption.get());
                        }
                    }
                    else if (ResultReason.NoMatch == e.getResult().getReason())
                    {
                        WriteToConsole(String.format("NOMATCH: Speech could not be recognized.%s", System.lineSeparator()));
                    }
                }
                catch(IOException ex)
                {
                    // Error output should not be suppressed, even if suppress output flag is set.
                    System.out.println(ex.getMessage());
                    done[0] = true;
                }
            });
        }

        speechRecognizer.recognized.addEventListener((s, e) -> {
            if (ResultReason.RecognizedSpeech == e.getResult().getReason() && e.getResult().getText().length() > 0)
            {
                try
                {
                    if (CaptioningMode.Offline == _userConfig.getCaptioningMode())
                    {
                        _offlineResults.add(e.getResult());
                    }
                    else
                    {
                        Optional<String> caption = CaptionFromRealTimeResult(e.getResult(), true);
                        if (caption.isPresent())
                        {
                            WriteToConsoleOrFile(caption.get());
                        }
                    }
                }
                catch(IOException ex)
                {
                    // Error output should not be suppressed, even if suppress output flag is set.
                    System.out.println(ex.getMessage());
                    done[0] = true;
                }
            }
            else if (ResultReason.NoMatch == e.getResult().getReason())
            {
                WriteToConsole(String.format("NOMATCH: Speech could not be recognized.%s", System.lineSeparator()));
            }
        });
        
        speechRecognizer.canceled.addEventListener((s, e) -> {
            if (CancellationReason.EndOfStream == e.getReason())
            {
                WriteToConsole(String.format("End of stream reached.%s", System.lineSeparator()));
                done[0] = true; // Notify to stop recognition.
            }
            else if (CancellationReason.CancelledByUser == e.getReason())
            {
                WriteToConsole(String.format("User canceled request.%s", System.lineSeparator()));
                done[0] = true; // Notify to stop recognition.
            }
            else if (CancellationReason.Error == e.getReason())
            {
                // Error output should not be suppressed, even if suppress output flag is set.
                System.out.println(String.format("Encountered error.%sError code: %d%sError details: %s%s", System.lineSeparator(), e.getErrorCode(), e.getErrorDetails()));
                done[0] = true; // Notify to stop recognition.
            }
            else
            {
                System.out.println(String.format("Request was cancelled for an unrecognized reason: %d.%s", e.getReason(), System.lineSeparator()));
                done[0] = true; // Notify to stop recognition.
            }
        });
        
        speechRecognizer.sessionStopped.addEventListener((s, e) -> {
            WriteToConsole(String.format("Session stopped.%s", System.lineSeparator()));
            done[0] = true; // Notify to stop recognition.
        });
        
        speechRecognizer.startContinuousRecognitionAsync().get();

        while(!done[0])
        {
            Thread.sleep(500);
        }

        speechRecognizer.stopContinuousRecognitionAsync().get();        
        
        return;
    }

    public static void main(String[] args) {
        final var usage = new StringBuilder()
.append("Usage: java -cp .;target\\dependency\\* Captioning [...]").append(System.lineSeparator())
.append(System.lineSeparator())
.append("  HELP").append(System.lineSeparator())
.append("    --help                           Show this help and stop.").append(System.lineSeparator())
.append(System.lineSeparator())
.append("  CONNECTION").append(System.lineSeparator())
.append("    --key KEY                        Your Azure Speech service resource key.").append(System.lineSeparator())
.append("                                     Overrides the SPEECH_KEY environment variable. You must set the environment variable (recommended) or use the `--key` option.").append(System.lineSeparator())
.append("    --region REGION                  Your Azure Speech service region.").append(System.lineSeparator())
.append("                                     Overrides the SPEECH_REGION environment variable. You must set the environment variable (recommended) or use the `--region` option.").append(System.lineSeparator())
.append("                                     Examples: westus, eastus").append(System.lineSeparator())
.append(System.lineSeparator())
.append("  LANGUAGE").append(System.lineSeparator())
.append("    --language LANG                  Specify language. This is used when breaking captions into lines.").append(System.lineSeparator())
.append("                                     Default value is en-US.").append(System.lineSeparator())
.append("                                     Examples: en-US, ja-JP").append(System.lineSeparator())
.append(System.lineSeparator())
.append("  INPUT").append(System.lineSeparator())
.append("    --input FILE                     Input audio from file (default input is the microphone.)").append(System.lineSeparator())
.append("    --format FORMAT                  Use compressed audio format.").append(System.lineSeparator())
.append("                                     If this is not present, uncompressed format (wav) is assumed.").append(System.lineSeparator())
.append("                                     Valid only with --file.").append(System.lineSeparator())
.append("                                     Valid values: alaw, any, flac, mp3, mulaw, ogg_opus").append(System.lineSeparator())
.append(System.lineSeparator())
.append("  MODE").append(System.lineSeparator())
.append("    --offline                        Output offline results.").append(System.lineSeparator())
.append("                                     Overrides --realTime.").append(System.lineSeparator())
.append("    --realTime                       Output real-time results.").append(System.lineSeparator())
.append("                                     Default output mode is offline.").append(System.lineSeparator())
.append(System.lineSeparator())
.append("  ACCURACY").append(System.lineSeparator())
.append("    --phrases \"PHRASE1;PHRASE2\"    Example: \"Constoso;Jessie;Rehaan\"").append(System.lineSeparator())
.append(System.lineSeparator())
.append("  OUTPUT").append(System.lineSeparator())
.append("    --output FILE                    Output captions to text file.").append(System.lineSeparator())
.append("    --srt                            Output captions in SubRip Text format (default format is WebVTT.)").append(System.lineSeparator())
.append("    --maxLineLength LENGTH           Set the maximum number of characters per line for a caption to LENGTH.").append(System.lineSeparator())
.append("                                     Minimum is 20. Default is 37 (30 for Chinese).").append(System.lineSeparator())
.append("    --lines LINES                    Set the number of lines for a caption to LINES.").append(System.lineSeparator())
.append("                                     Minimum is 1. Default is 2.").append(System.lineSeparator())
.append("    --delay MILLISECONDS             How many MILLISECONDS to delay the appearance of each caption.").append(System.lineSeparator())
.append("                                     Minimum is 0. Default is 1000.").append(System.lineSeparator())
.append("    --remainTime MILLISECONDS        How many MILLISECONDS a caption should remain on screen if it is not replaced by another.").append(System.lineSeparator())
.append("                                     Minimum is 0. Default is 1000.").append(System.lineSeparator())
.append("    --quiet                          Suppress console output, except errors.").append(System.lineSeparator())
.append("    --profanity OPTION               Valid values: raw, remove, mask").append(System.lineSeparator())
.append("                                     Default value: mask").append(System.lineSeparator())
.append("    --threshold NUMBER               Set stable partial result threshold.").append(System.lineSeparator())
.append("                                     Default value: 3").append(System.lineSeparator())
.toString();

        try
        {
            List<String> argsList = Arrays.stream(args).collect(Collectors.toList());
            if (argsList.contains("--help"))
            {
                System.out.println(usage);
            }
            else
            {
                final var captioning = new Captioning();
                final UserConfig userConfig = UserConfig.UserConfigFromArgs(argsList, usage);
                captioning.Initialize(userConfig);
                final SpeechRecognizer speechRecognizer = captioning.SpeechRecognizerFromUserConfig();
                captioning.RecognizeContinuous(speechRecognizer);
                captioning.Finish();
            }
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }        
    }
}