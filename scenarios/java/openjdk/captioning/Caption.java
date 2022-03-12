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
import com.microsoft.cognitiveservices.speech.audio.AudioConfig;

/*
Types
*/

class TooFewArgumentsException extends Exception {}

final class UserConfig
{
    final private boolean profanityFilterRemoveEnabled;
    final private boolean profanityFilterMaskEnabled;
    final private Optional<String> inputFile;
    final private Optional<String> outputFile;
    final private Optional<String> phraseList;
    final private boolean suppressOutputEnabled;
    final private boolean partialResultsEnabled;
    final private Optional<String> stablePartialResultThreshold;
    final private boolean srtEnabled;
    final private boolean trueTextEnabled;
    final private String subscriptionKey;
    final private String region;

    public boolean getProfanityFilterRemoveEnabled() {
        return profanityFilterRemoveEnabled;
    }
    public boolean getProfanityFilterMaskEnabled() {
        return profanityFilterMaskEnabled;
    }
    public Optional<String> getInputFile() {
        return inputFile;
    }
    public Optional<String> getOutputFile() {
        return outputFile;
    }
    public Optional<String> getPhraseList() {
        return phraseList;
    }
    public boolean getSuppressOutputEnabled() {
        return suppressOutputEnabled;
    }
    public boolean getPartialResultsEnabled() {
        return partialResultsEnabled;
    }
    public Optional<String> getStablePartialResultThreshold() {
        return stablePartialResultThreshold;
    }
    public boolean getSrtEnabled() {
        return srtEnabled;
    }
    public boolean getTrueTextEnabled() {
        return trueTextEnabled;
    }
    public String getSubscriptionKey() {
        return subscriptionKey;
    }
    public String getRegion() {
        return region;
    }

    public UserConfig(
        boolean profanityFilterRemoveEnabled,
        boolean profanityFilterMaskEnabled,
        Optional<String> inputFile,
        Optional<String> outputFile,
        Optional<String> phraseList,
        boolean suppressOutputEnabled,
        boolean partialResultsEnabled,
        Optional<String> stablePartialResultThreshold,
        boolean srtEnabled,
        boolean trueTextEnabled,
        String subscriptionKey,
        String region
        )
    {
        this.profanityFilterRemoveEnabled = profanityFilterRemoveEnabled;
        this.profanityFilterMaskEnabled = profanityFilterMaskEnabled;
        this.inputFile = inputFile;
        this.outputFile = outputFile;
        this.phraseList = phraseList;
        this.suppressOutputEnabled = suppressOutputEnabled;
        this.partialResultsEnabled = partialResultsEnabled;
        this.stablePartialResultThreshold = stablePartialResultThreshold;
        this.srtEnabled = srtEnabled;
        this.trueTextEnabled = trueTextEnabled;
        this.subscriptionKey = subscriptionKey;
        this.region = region;
    }
}

public class Caption
{
/*
Helper functions
*/

    static Optional<String> GetCmdOption(List<String> args, String option)
    {
        final int index = args.indexOf(option);
        if(index > -1 && index < args.size() - 2)
        {
// We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
            return Optional.of(args.get(index + 1));
        }
        else {
            return Optional.empty();
        }
    }

    static boolean CmdOptionExists(List<String> args, String option)
    {
        return(args.contains(option));
    }

    static String TimestampFromSpeechRecognitionResult(SpeechRecognitionResult result, UserConfig userConfig)
    {
        final BigInteger ticksPerMillisecond = BigInteger.valueOf(10000);
        final Date startTime = new Date(result.getOffset().divide(ticksPerMillisecond).longValue());
        final Date endTime = new Date((result.getOffset().add(result.getDuration())).divide(ticksPerMillisecond).longValue());
        
        String format = "";
        if(userConfig.getSrtEnabled())
        {
// SRT format requires ',' as decimal separator rather than '.'.
            format = "HH:mm:ss,S";
        }
        else
        {
            format = "HH:mm:ss.S";
        }
        DateFormat formatter = new SimpleDateFormat(format);
// If we don't do this, the time is adjusted for our local time zone, which we don't want.
        formatter.setTimeZone(TimeZone.getTimeZone("GMT"));
        return String.format("%s --> %s", formatter.format(startTime), formatter.format(endTime));
    }

    static String CaptionFromSpeechRecognitionResult(int sequenceNumber, SpeechRecognitionResult result, UserConfig userConfig)
    {
        StringBuilder caption = new StringBuilder();
        if(!userConfig.getPartialResultsEnabled() && userConfig.getSrtEnabled())
        {
            caption.append(String.format("%d%s", sequenceNumber, System.lineSeparator()));
        }
        caption.append(String.format("%s%s", TimestampFromSpeechRecognitionResult(result, userConfig), System.lineSeparator()));
        caption.append(String.format("%s%s%s", result.getText(), System.lineSeparator(), System.lineSeparator()));
        return caption.toString();
    }

    static void WriteToConsole(String text, UserConfig userConfig)
    {
        if(!userConfig.getSuppressOutputEnabled())
        {
            System.out.print(text);
        }
    }

    static void WriteToConsoleOrFile(String text, UserConfig userConfig) throws IOException
    {
        WriteToConsole(text, userConfig);
        if(userConfig.getOutputFile().isPresent())
        {
            try
            {
                FileWriter outputFile = new FileWriter(userConfig.getOutputFile().get(), true);
                outputFile.write(text);
                outputFile.close();
            }
            catch(IOException e)
            {
                throw e;
            }
        }
    }

    static void Initialize(UserConfig userConfig) throws IOException
    {
        if(userConfig.getOutputFile().isPresent())
        {
            File outputFile = new File(userConfig.getOutputFile().get());
            if(outputFile.exists())
            {
                outputFile.delete();
            }
        }

        if(!userConfig.getSrtEnabled())
        {
            WriteToConsoleOrFile(String.format("WEBVTT%s%s", System.lineSeparator(), System.lineSeparator()), userConfig);
        }
    }

/*
Main functions
*/

    static UserConfig UserConfigFromArgs(List<String> args) throws TooFewArgumentsException
    {
// Verify argc >= 3(caption, subscriptionKey, region)
        if(args.size() < 3)
        {
            throw new TooFewArgumentsException();
        }
       
        return new UserConfig(
            CmdOptionExists(args, "-f"),
            CmdOptionExists(args, "-m"),
            GetCmdOption(args, "-i"),
            GetCmdOption(args, "-o"),
            GetCmdOption(args, "-p"),
            CmdOptionExists(args, "-q"),
            CmdOptionExists(args, "-u"),
            GetCmdOption(args, "-r"),
            CmdOptionExists(args, "-s"),
            CmdOptionExists(args, "-t"),
            args.get(args.size() - 2),
            args.get(args.size() - 1)
        );
    }

    static AudioConfig AudioConfigFromUserConfig(UserConfig userConfig)
    {
        if(userConfig.getInputFile().isPresent())
        {
            return AudioConfig.fromWavFileInput(userConfig.getInputFile().get());
        }
        else
        {
            return AudioConfig.fromDefaultMicrophoneInput();
        }
    }

    static SpeechConfig SpeechConfigFromUserConfig(UserConfig userConfig)
    {
        final SpeechConfig speechConfig = SpeechConfig.fromSubscription(userConfig.getSubscriptionKey(), userConfig.getRegion());

        if(userConfig.getProfanityFilterRemoveEnabled())
        {
            speechConfig.setProfanity(ProfanityOption.Removed);
        }
        else if(userConfig.getProfanityFilterMaskEnabled())
        {
            speechConfig.setProfanity(ProfanityOption.Masked);
        }
        
        if(userConfig.getStablePartialResultThreshold().isPresent())
        {
            speechConfig.setProperty(PropertyId.SpeechServiceResponse_StablePartialResultThreshold, userConfig.getStablePartialResultThreshold().get());
        }
        
        if(userConfig.getTrueTextEnabled())
        {
            speechConfig.setProperty(PropertyId.SpeechServiceResponse_PostProcessingOption, "TrueText");
        }
        
        return speechConfig;
    }

    static SpeechRecognizer SpeechRecognizerFromSpeechConfig(SpeechConfig speechConfig, AudioConfig audio_config, UserConfig userConfig)
    {
        final SpeechRecognizer speechRecognizer = new SpeechRecognizer(speechConfig, audio_config);
        
        if(userConfig.getPhraseList().isPresent())
        {
            PhraseListGrammar grammar = PhraseListGrammar.fromRecognizer(speechRecognizer);
            grammar.addPhrase(userConfig.getPhraseList().get());
        }
        
        return speechRecognizer;
    }

    static SpeechRecognizer SpeechRecognizerFromUserConfig(UserConfig userConfig)
    {
        final AudioConfig audioConfig = AudioConfigFromUserConfig(userConfig);
        final SpeechConfig speechConfig = SpeechConfigFromUserConfig(userConfig);
        return SpeechRecognizerFromSpeechConfig(speechConfig, audioConfig, userConfig);
    }

    static void RecognizeContinuous(SpeechRecognizer speechRecognizer, UserConfig userConfig) throws ExecutionException, InterruptedException, IOException
    {
// This lets us modify local variables from inside a lambda.
        final boolean[] done = new boolean[] { false };
        final int[] sequenceNumber = new int[] { 0 };

        if(userConfig.getPartialResultsEnabled())
        {
            speechRecognizer.recognizing.addEventListener((s, e) -> {
                if(ResultReason.RecognizingSpeech == e.getResult().getReason() && e.getResult().getText().length() > 0)
                {
                    try
                    {
// We don't show sequence numbers for partial results.
                        WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(0, e.getResult(), userConfig), userConfig);
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
                    WriteToConsole(String.format("NOMATCH: Speech could not be recognized.%s", System.lineSeparator()), userConfig);
                }
            });
        }
        else
        {
            speechRecognizer.recognized.addEventListener((s, e) -> {
                if(ResultReason.RecognizedSpeech == e.getResult().getReason() && e.getResult().getText().length() > 0)
                {
                    sequenceNumber[0]++;
                    try
                    {
                        WriteToConsoleOrFile(CaptionFromSpeechRecognitionResult(sequenceNumber[0], e.getResult(), userConfig), userConfig);
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
                    WriteToConsole(String.format("NOMATCH: Speech could not be recognized.%s", System.lineSeparator()), userConfig);
                }
            });
        }
        
        speechRecognizer.canceled.addEventListener((s, e) -> {
            if(CancellationReason.EndOfStream == e.getReason())
            {
                WriteToConsole(String.format("End of stream reached.%s", System.lineSeparator()), userConfig);
                done[0] = true; // Notify to stop recognition.
            }
            else if(CancellationReason.CancelledByUser == e.getReason())
            {
                WriteToConsole(String.format("User canceled request.%s", System.lineSeparator()), userConfig);
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
            WriteToConsole(String.format("Session stopped.%s", System.lineSeparator()), userConfig);
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
        final String usage = """
Usage: java -cp .;target\\dependency\\* Caption [-f] [-h] [-i file] [-m] [-o file] [-p phrases] [-q] [-r number] [-s] [-t] [-u] <subscriptionKey> <region>
            -f: Enable profanity filter (remove profanity). Overrides -m.
            -h: Show this help and stop.
            -i: Input audio file *file* (default input is from the microphone.)
            -m: Enable profanity filter (mask profanity). -f overrides this.
       -o file: Output to *file*.
    -p phrases: Add specified *phrases*.
                Example: Constoso;Jessie;Rehaan
            -q: Suppress console output (except errors).
     -r number: Set stable partial result threshold to *number*.
                Example: 3
            -s: Emit SRT (default is WebVTT.)
            -t: Enable TrueText.
            -u: Emit partial results instead of finalized results.
""";

        try
        {
            List<String> argsList = Arrays.stream(args).collect(Collectors.toList());
            if(argsList.contains("-h"))
            {
                System.out.println(usage);
            }
            else
            {
                final UserConfig userConfig = UserConfigFromArgs(argsList);
                Initialize(userConfig);
                final AudioConfig audio_config = AudioConfigFromUserConfig(userConfig);
                final SpeechRecognizer speechRecognizer = SpeechRecognizerFromUserConfig(userConfig);
                RecognizeContinuous(speechRecognizer, userConfig);
            }
        }
        catch(TooFewArgumentsException e)
        {
            System.out.println("Too few arguments.");
            System.out.println(usage);
        }
        catch(Exception e)
        {
            System.out.println(e.getMessage());
        }        
    }
}
