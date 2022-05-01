//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

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
}
