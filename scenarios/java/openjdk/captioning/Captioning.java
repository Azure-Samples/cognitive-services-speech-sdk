//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.AutoCloseable;
import java.lang.StringBuilder;
import java.math.BigInteger;
import java.net.URI;
import java.net.URISyntaxException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.Date;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;
import java.util.TimeZone;
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;

public class Captioning
{
    UserConfig m_userConfig;
    AudioStreamFormat m_format;
    PullAudioInputStreamCallback m_callback;
    PullAudioInputStream m_stream;

    static private Optional<String> GetCmdOption(List<String> args, String option)
    {
        final var index = args.indexOf(option);
        if(index > -1 && index < args.size() - 1)
        {
            // We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
            return Optional.of(args.get(index + 1));
        }
        else {
            return Optional.empty();
        }
    }

    static private boolean CmdOptionExists(List<String> args, String option)
    {
        return(args.contains(option));
    }

    static private AudioStreamContainerFormat GetCompressedAudioFormat(List<String> args)
    {
        final var value = GetCmdOption(args, "--format");
        if (!value.isPresent())
        {
            return AudioStreamContainerFormat.ANY;
        }
        else
        {
            switch (value.get().toLowerCase())
            {
                case "alaw" : return AudioStreamContainerFormat.ALAW;
                case "flac" : return AudioStreamContainerFormat.FLAC;
                case "mp3" : return AudioStreamContainerFormat.MP3;
                case "mulaw" : return AudioStreamContainerFormat.MULAW;
                case "ogg_opus" : return AudioStreamContainerFormat.OGG_OPUS;
                default : return AudioStreamContainerFormat.ANY;
            }
        }
    }

    static private ProfanityOption GetProfanityOption(List<String> args)
    {
        final var value = GetCmdOption(args, "--profanity");
        if (!value.isPresent())
        {
            return ProfanityOption.Masked;
        }
        else
        {
            switch (value.get().toLowerCase())
            {
                case "raw" : return ProfanityOption.Raw;
                case "removed" : return ProfanityOption.Removed;
                default : return ProfanityOption.Masked;
            }
        }
    }

    private String TimestampFromSpeechRecognitionResult(SpeechRecognitionResult result)
    {
        final var ticksPerMillisecond = BigInteger.valueOf(10000);
        final var startTime = new Date(result.getOffset().divide(ticksPerMillisecond).longValue());
        final var endTime = new Date((result.getOffset().add(result.getDuration())).divide(ticksPerMillisecond).longValue());
        
        var format = "";
        if(m_userConfig.getUseSubRipTextCaptionFormat())
        {
            // SRT format requires ',' as decimal separator rather than '.'.
            format = "HH:mm:ss,S";
        }
        else
        {
            format = "HH:mm:ss.S";
        }
        var formatter = new SimpleDateFormat(format);
        // If we don't do this, the time is adjusted for our local time zone, which we don't want.
        formatter.setTimeZone(TimeZone.getTimeZone("GMT"));
        return String.format("%s --> %s", formatter.format(startTime), formatter.format(endTime));
    }

    private String CaptionFromSpeechRecognitionResult(int sequenceNumber, SpeechRecognitionResult result)
    {
        var caption = new StringBuilder();
        if(!m_userConfig.getShowRecognizingResults() && m_userConfig.getUseSubRipTextCaptionFormat())
        {
            caption.append(String.format("%d%s", sequenceNumber, System.lineSeparator()));
        }
        caption.append(String.format("%s%s", TimestampFromSpeechRecognitionResult(result), System.lineSeparator()));
        caption.append(String.format("%s%s%s", result.getText(), System.lineSeparator(), System.lineSeparator()));
        return caption.toString();
    }

    private void WriteToConsole(String text)
    {
        if(!m_userConfig.getSuppressConsoleOutput())
        {
            System.out.print(text);
        }
    }

    private void WriteToConsoleOrFile(String text) throws IOException
    {
        WriteToConsole(text);
        if(m_userConfig.getOutputFile().isPresent())
        {
            try
            {
                var outputFile = new FileWriter(m_userConfig.getOutputFile().get(), true);
                outputFile.write(text);
                outputFile.close();
            }
            catch(IOException e)
            {
                throw e;
            }
        }
    }

    private void Initialize(UserConfig userConfig) throws IOException
    {
        m_userConfig = userConfig;
        
        if(m_userConfig.getOutputFile().isPresent())
        {
            var outputFile = new File(m_userConfig.getOutputFile().get());
            if(outputFile.exists())
            {
                outputFile.delete();
            }
        }

        if(!m_userConfig.getUseSubRipTextCaptionFormat())
        {
            WriteToConsoleOrFile(String.format("WEBVTT%s%s", System.lineSeparator(), System.lineSeparator()));
        }
    }

    static private UserConfig UserConfigFromArgs(List<String> args, String usage) throws IllegalArgumentException
    {
        var key = GetCmdOption(args, "--key");
        if(!key.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing subscription key.%s%s", System.lineSeparator(), usage));
        }
        var region = GetCmdOption(args, "--region");
        if(!region.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing region.%s%s", System.lineSeparator(), usage));
        }
        
        return new UserConfig(
            CmdOptionExists(args, "--format"),
            GetCompressedAudioFormat(args),
            GetProfanityOption(args),
            GetCmdOption(args, "--input"),
            GetCmdOption(args, "--output"),
            GetCmdOption(args, "--phrases"),
            CmdOptionExists(args, "--quiet"),
            CmdOptionExists(args, "--recognizing"),
            GetCmdOption(args, "--threshold"),
            CmdOptionExists(args, "--srt"),
            key.get(),
            region.get()
        );
    }

    private AudioConfig AudioConfigFromUserConfig()
    {
        if(m_userConfig.getInputFile().isPresent())
        {
            if (!m_userConfig.getUseCompressedAudio())
            {
                var reader = new WavFileReader(m_userConfig.getInputFile().get());
                m_format = reader.getFormat();
                reader.Close();
            }
            else
            {
                m_format = AudioStreamFormat.getCompressedFormat(m_userConfig.getCompressedAudioFormat());
            }
            m_callback = new BinaryFileReader(m_userConfig.getInputFile().get());
            m_stream = AudioInputStream.createPullStream(m_callback, m_format);
            return AudioConfig.fromStreamInput(m_stream);
        }
        else
        {
            return AudioConfig.fromDefaultMicrophoneInput();
        }
    }

    private SpeechConfig SpeechConfigFromUserConfig()
    {
        final var speechConfig = SpeechConfig.fromSubscription(m_userConfig.getSubscriptionKey(), m_userConfig.getRegion());

        speechConfig.setProfanity(m_userConfig.getProfanityOption());

        if(m_userConfig.getStablePartialResultThreshold().isPresent())
        {
            speechConfig.setProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, m_userConfig.getStablePartialResultThreshold().get());
        }
        
        speechConfig.setProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");
        
        return speechConfig;
    }

    private SpeechRecognizer SpeechRecognizerFromUserConfig()
    {
        final var audioConfig = AudioConfigFromUserConfig();
        final var speechConfig = SpeechConfigFromUserConfig();
        final var speechRecognizer = new SpeechRecognizer(speechConfig, audioConfig);
        
        if(m_userConfig.getPhraseList().isPresent())
        {
            var grammar = PhraseListGrammar.fromRecognizer(speechRecognizer);
            grammar.addPhrase(m_userConfig.getPhraseList().get());
        }
        
        return speechRecognizer;
    }

    private void RecognizeContinuous(SpeechRecognizer speechRecognizer) throws ExecutionException, InterruptedException
    {
        // This lets us modify local variables from inside a lambda.
        final var done = new boolean[] { false };
        final var sequenceNumber = new int[] { 0 };

        if(m_userConfig.getShowRecognizingResults())
        {
            speechRecognizer.recognizing.addEventListener((s, e) -> {
                if(ResultReason.RecognizingSpeech == e.getResult().getReason() && e.getResult().getText().length() > 0)
                {
                    // We don't show sequence numbers for partial results.
                    WriteToConsole(CaptionFromSpeechRecognitionResult(0, e.getResult()));
                }
                else if(ResultReason.NoMatch == e.getResult().getReason())
                {
                    WriteToConsole(String.format("NOMATCH: Speech could not be recognized.%s", System.lineSeparator()));
                }
            });
        }

        speechRecognizer.recognized.addEventListener((s, e) -> {
            if(ResultReason.RecognizedSpeech == e.getResult().getReason() && e.getResult().getText().length() > 0)
            {
                sequenceNumber[0]++;
                try
                {
                    WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber[0], e.getResult()));
                }
                catch(IOException ex)
                {
                    // Error output should not be suppressed, even if suppress output flag is set.
                    System.out.println(ex.getMessage());
                    done[0] = true;
                }
            }
            else if(ResultReason.NoMatch == e.getResult().getReason())
            {
                WriteToConsole(String.format("NOMATCH: Speech could not be recognized.%s", System.lineSeparator()));
            }
        });
        
        speechRecognizer.canceled.addEventListener((s, e) -> {
            if(CancellationReason.EndOfStream == e.getReason())
            {
                WriteToConsole(String.format("End of stream reached.%s", System.lineSeparator()));
                done[0] = true; // Notify to stop recognition.
            }
            else if(CancellationReason.CancelledByUser == e.getReason())
            {
                WriteToConsole(String.format("User canceled request.%s", System.lineSeparator()));
                done[0] = true; // Notify to stop recognition.
            }
            else if(CancellationReason.Error == e.getReason())
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
        // Text block content must begin on a new line.
        final var usage = """
Usage: java -cp .;target\\dependency\\* Captioning [...]

  CONNECTION
    --key KEY                     Your Azure Speech service subscription key.
    --region REGION               Your Azure Speech service region.
                                  Examples: westus, eastus

  INPUT
    --input FILE                  Input audio from file (default input is the microphone.)
    --url URL                     Input audio from URL (default input is the microphone.)
    --format FORMAT               Use compressed audio format.
                                  Valid only with --file or --url.
                                  If this is not specified, uncompressed format (wav) is assumed.
                                  Valid values: alaw, any, flac, mp3, mulaw, ogg_opus
                                  Default value: any

  RECOGNITION
    --recognizing                 Output Recognizing results (default output is Recognized results only.)
                                  These are always written to the console, never to an output file.
                                  --quiet overrides this.

  ACCURACY
    --phrases PHRASE1;PHRASE2     Example: Constoso;Jessie;Rehaan

  OUTPUT
    --help                        Show this help and stop.
    --output FILE                 Output captions to file.
    --srt                         Output captions in SubRip Text format (default format is WebVTT.)
    --quiet                       Suppress console output, except errors.
    --profanity OPTION            Valid values: raw, remove, mask
    --threshold NUMBER            Set stable partial result threshold.
                                  Default value: 3
""";

        try
        {
            List<String> argsList = Arrays.stream(args).collect(Collectors.toList());
            if(argsList.contains("--help"))
            {
                System.out.println(usage);
            }
            else
            {
                final var captioning = new Captioning();
                final var userConfig = UserConfigFromArgs(argsList, usage);
                captioning.Initialize(userConfig);
                final var speechRecognizer = captioning.SpeechRecognizerFromUserConfig();
                captioning.RecognizeContinuous(speechRecognizer);
            }
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }        
    }
}
