//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.List;
import java.util.Optional;
import com.microsoft.cognitiveservices.speech.*;
import com.microsoft.cognitiveservices.speech.audio.*;

final class UserConfig
{
    final private boolean useCompressedAudio;
    final private AudioStreamContainerFormat compressedAudioFormat;
    final private ProfanityOption profanityOption;
    final private Optional<String> inputFile;
    final private Optional<String> outputFile;
    final private Optional<String> phraseList;
    final private boolean suppressConsoleOutput;
    final private boolean showRecognizingResults;
    final private Optional<String> stablePartialResultThreshold;
    final private boolean useSubRipTextCaptionFormat;
    final private String subscriptionKey;
    final private String region;

    public boolean getUseCompressedAudio() {
        return useCompressedAudio;
    }
    public AudioStreamContainerFormat getCompressedAudioFormat() {
        return compressedAudioFormat;
    }
    public ProfanityOption getProfanityOption() {
        return profanityOption;
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
    public boolean getSuppressConsoleOutput() {
        return suppressConsoleOutput;
    }
    public boolean getShowRecognizingResults() {
        return showRecognizingResults;
    }
    public Optional<String> getStablePartialResultThreshold() {
        return stablePartialResultThreshold;
    }
    public boolean getUseSubRipTextCaptionFormat() {
        return useSubRipTextCaptionFormat;
    }
    public String getSubscriptionKey() {
        return subscriptionKey;
    }
    public String getRegion() {
        return region;
    }

    public UserConfig(
        boolean useCompressedAudio,
        AudioStreamContainerFormat compressedAudioFormat,
        ProfanityOption profanityOption,
        Optional<String> inputFile,
        Optional<String> outputFile,
        Optional<String> phraseList,
        boolean suppressConsoleOutput,
        boolean showRecognizingResults,
        Optional<String> stablePartialResultThreshold,
        boolean useSubRipTextCaptionFormat,
        String subscriptionKey,
        String region
        )
    {
        this.useCompressedAudio = useCompressedAudio;
        this.compressedAudioFormat = compressedAudioFormat;
        this.profanityOption = profanityOption;
        this.inputFile = inputFile;
        this.outputFile = outputFile;
        this.phraseList = phraseList;
        this.suppressConsoleOutput = suppressConsoleOutput;
        this.showRecognizingResults = showRecognizingResults;
        this.stablePartialResultThreshold = stablePartialResultThreshold;
        this.useSubRipTextCaptionFormat = useSubRipTextCaptionFormat;
        this.subscriptionKey = subscriptionKey;
        this.region = region;
    }

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

    static public AudioStreamContainerFormat GetCompressedAudioFormat(List<String> args)
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

    static public ProfanityOption GetProfanityOption(List<String> args)
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
    
    static public UserConfig UserConfigFromArgs(List<String> args, String usage) throws IllegalArgumentException
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
}
