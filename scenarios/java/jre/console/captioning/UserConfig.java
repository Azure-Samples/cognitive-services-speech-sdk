//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.List;
import java.util.Optional;
import java.util.stream.*;
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;

enum CaptioningMode
{
    Offline,
    RealTime
}

final class UserConfig
{
    final public static int defaultMaxLineLengthSBCS = 37;
    final public static int defaultMaxLineLengthMBCS = 30;
    
    final private boolean useCompressedAudio;
    final private AudioStreamContainerFormat compressedAudioFormat;
    final private ProfanityOption profanityOption;
    final private String language;
    final private Optional<String> inputFile;
    final private Optional<String> outputFile;
    final private Optional<String> phraseList;
    final private boolean suppressConsoleOutput;
    final private CaptioningMode captioningMode;
    final private int remainTime;
    final private int delay;
    final private boolean useSubRipTextCaptionFormat;
    final private int maxLineLength;
    final private int lines;
    final private Optional<String> stablePartialResultThreshold;    
    final private String subscriptionKey;
    final private String region;

    public boolean getUseCompressedAudio()
    {
        return useCompressedAudio;
    }
    public AudioStreamContainerFormat getCompressedAudioFormat()
    {
        return compressedAudioFormat;
    }
    public ProfanityOption getProfanityOption()
    {
        return profanityOption;
    }
    public String getLanguage()
    {
        return language;
    }
    public Optional<String> getInputFile()
    {
        return inputFile;
    }
    public Optional<String> getOutputFile()
    {
        return outputFile;
    }
    public Optional<String> getPhraseList()
    {
        return phraseList;
    }
    public boolean getSuppressConsoleOutput()
    {
        return suppressConsoleOutput;
    }
    public CaptioningMode getCaptioningMode()
    {
        return captioningMode;
    }
    public int getRemainTime()
    {
        return remainTime;
    }
    public int getDelay()
    {
        return delay;
    }
    public boolean getUseSubRipTextCaptionFormat()
    {
        return useSubRipTextCaptionFormat;
    }
    public int getMaxLineLength()
    {
        return maxLineLength;
    }
    public int getLines()
    {
        return lines;
    }
    public Optional<String> getStablePartialResultThreshold()
    {
        return stablePartialResultThreshold;
    }
    public String getSubscriptionKey()
    {
        return subscriptionKey;
    }
    public String getRegion()
    {
        return region;
    }

    public UserConfig(
        boolean useCompressedAudio,
        AudioStreamContainerFormat compressedAudioFormat,
        ProfanityOption profanityOption,
        String language,
        Optional<String> inputFile,
        Optional<String> outputFile,
        Optional<String> phraseList,
        boolean suppressConsoleOutput,
        CaptioningMode captioningMode,
        int remainTime,
        int delay,
        boolean useSubRipTextCaptionFormat,
        int maxLineLength,
        int lines,
        Optional<String> stablePartialResultThreshold,
        String subscriptionKey,
        String region
        )
    {
        this.useCompressedAudio = useCompressedAudio;
        this.compressedAudioFormat = compressedAudioFormat;
        this.profanityOption = profanityOption;
        this.language = language;
        this.inputFile = inputFile;
        this.outputFile = outputFile;
        this.phraseList = phraseList;
        this.suppressConsoleOutput = suppressConsoleOutput;
        this.captioningMode = captioningMode;
        this.remainTime = remainTime;
        this.delay = delay;
        this.useSubRipTextCaptionFormat = useSubRipTextCaptionFormat;
        this.maxLineLength = maxLineLength;
        this.lines = lines;
        this.stablePartialResultThreshold = stablePartialResultThreshold;
        this.subscriptionKey = subscriptionKey;
        this.region = region;
    }

    static private Optional<String> GetCmdOption(List<String> args, String option)
    {
        final int index = args.stream().map(arg -> arg.toLowerCase()).toList().indexOf(option.toLowerCase());
        if (index > -1 && index < args.size() - 1)
        {
            // We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
            return Optional.of(args.get(index + 1));
        }
        else
        {
            return Optional.empty();
        }
    }

    static private boolean CmdOptionExists(List<String> args, String option)
    {
        return(args.stream().map(arg -> arg.toLowerCase()).toList().contains(option.toLowerCase()));
    }

    static public AudioStreamContainerFormat GetCompressedAudioFormat(List<String> args)
    {
        final Optional<String> value = GetCmdOption(args, "--format");
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

    static public ProfanityOption GetProfanityOption(List<String> args)
    {
        final Optional<String> value = GetCmdOption(args, "--profanity");
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
    
    static public UserConfig UserConfigFromArgs(List<String> args, String usage) throws IllegalArgumentException
    {

        Optional<String> keyOption = GetCmdOption(args, "--key");
        String key = keyOption.isPresent() ? keyOption.get() : System.getenv("SPEECH_KEY");
        if (0 == key.length())
        {
            throw new IllegalArgumentException(String.format("Please set the SPEECH_KEY environment variable or provide a Speech resource key with the --key option.%s%s", System.lineSeparator(), usage));
        }
        Optional<String> regionOption = GetCmdOption(args, "--region");
        String region = regionOption.isPresent() ? regionOption.get() : System.getenv("SPEECH_REGION");
        if (0 == region.length())
        {
            throw new IllegalArgumentException(String.format("Please set the SPEECH_REGION environment variable or provide a Speech resource region with the --region option%s%s", System.lineSeparator(), usage));
        }
        
        String language = "en-US";
        Optional<String> languageOption = GetCmdOption(args, "--language");
        if (languageOption.isPresent())
        {
            language = languageOption.get();
        }
        
        CaptioningMode captioningMode = CmdOptionExists(args, "--realTime") && !CmdOptionExists(args, "--offline") ? CaptioningMode.RealTime : CaptioningMode.Offline;
        
        Optional<String> strRemainTime = GetCmdOption(args, "--remainTime");
        int remainTime = 1000;
        if (strRemainTime.isPresent())
        {
            remainTime = Integer.parseInt(strRemainTime.get());
            if (remainTime < 0)
            {
                remainTime = 1000;
            }
        }
        
        Optional<String> strDelay = GetCmdOption(args, "--delay");
        int delay = 1000;
        if (strDelay.isPresent())
        {
            delay = Integer.parseInt(strDelay.get());
            if (delay < 0)
            {
                delay = 1000;
            }
        }
        
        Optional<String> strMaxLineLength = GetCmdOption(args, "--maxLineLength");
        int maxLineLength = defaultMaxLineLengthSBCS;
        if (strMaxLineLength.isPresent())
        {
            maxLineLength = Integer.parseInt(strMaxLineLength.get());
            if (maxLineLength < 20)
            {
                maxLineLength = 20;
            }
        }
        
        Optional<String> strLines = GetCmdOption(args, "--lines");
        int lines = 2;
        if (strLines.isPresent())
        {
            lines = Integer.parseInt(strLines.get());
            if (lines < 1)
            {
                lines = 2;
            }
        }
        
        return new UserConfig(
            CmdOptionExists(args, "--format"),
            GetCompressedAudioFormat(args),
            GetProfanityOption(args),
            language,
            GetCmdOption(args, "--input"),
            GetCmdOption(args, "--output"),
            GetCmdOption(args, "--phrases"),
            CmdOptionExists(args, "--quiet"),
            captioningMode,
            remainTime,
            delay,
            CmdOptionExists(args, "--srt"),
            maxLineLength,
            lines,
            GetCmdOption(args, "--threshold"),
            key,
            region
        );
    }
}
