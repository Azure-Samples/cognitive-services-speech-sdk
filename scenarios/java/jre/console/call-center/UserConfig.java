//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.List;
import java.util.Optional;

final class UserConfig
{
    final private boolean useStereoAudio;
    final private String inputAudioURL;
    final private Optional<String> outputFilePath;
    final private String language;
    final private String locale;
    final private String speechSubscriptionKey;
    final private String speechEndpoint;
    final private String languageSubscriptionKey;
    final private String languageEndpoint;

    public boolean getUseStereoAudio() {
        return useStereoAudio;
    }
    public String getInputAudioURL() {
        return inputAudioURL;
    }
    public Optional<String> getOutputFilePath() {
        return outputFilePath;
    }
    public String getLanguage() {
        return language;
    }
    public String getLocale() {
        return locale;
    }
    public String getSpeechSubscriptionKey() {
        return speechSubscriptionKey;
    }
    public String getSpeechEndpoint() {
        return speechEndpoint;
    }
    public String getLanguageSubscriptionKey() {
        return languageSubscriptionKey;
    }
    public String getLanguageEndpoint() {
        return languageEndpoint;
    }

    public UserConfig(
        boolean useStereoAudio,
        String inputAudioURL,
        Optional<String> outputFilePath,
        String language,
        String locale,
        String speechSubscriptionKey,
        String speechEndpoint,
        String languageSubscriptionKey,
        String languageEndpoint
        )
    {
        this.useStereoAudio = useStereoAudio;
        this.inputAudioURL = inputAudioURL;
        this.outputFilePath = outputFilePath;
        this.language = language;
        this.locale = locale;
        this.speechSubscriptionKey = speechSubscriptionKey;
        this.speechEndpoint = speechEndpoint;
        this.languageSubscriptionKey = languageSubscriptionKey;
        this.languageEndpoint = languageEndpoint;

    }

    static private Optional<String> GetCmdOption(List<String> args, String option)
    {
        final int index = args.indexOf(option);
        if (index > -1 && index < args.size() - 1)
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
    
    static public UserConfig UserConfigFromArgs(List<String> args, String usage) throws IllegalArgumentException
    {
        // This should not change unless the Speech REST API changes.
        final String partialSpeechEndpoint = ".api.cognitive.microsoft.com";
        
        Optional<String> speechSubscriptionKey = GetCmdOption(args, "--speechKey");
        if (!speechSubscriptionKey.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing Speech subscription key.%s%s", System.lineSeparator(), usage));
        }
        Optional<String> speechRegion = GetCmdOption(args, "--speechRegion");
        if (!speechRegion.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing Speech region.%s%s", System.lineSeparator(), usage));
        }

        Optional<String> languageSubscriptionKey = GetCmdOption(args, "--languageKey");
        if (!languageSubscriptionKey.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing Language subscription key.%s%s", System.lineSeparator(), usage));
        }
        Optional<String> languageEndpoint = GetCmdOption(args, "--languageEndpoint");
        if (!languageEndpoint.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing Language endpoint.%s%s", System.lineSeparator(), usage));
        }
        
        Optional<String> inputAudioURL = GetCmdOption(args, "--input");
        if (!inputAudioURL.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing input audio URL.%s%s", System.lineSeparator(), usage));
        }
        
        Optional<String> language = GetCmdOption(args, "--language");
        if (!language.isPresent())
        {
            language = Optional.of("en");
        }
        Optional<String> locale = GetCmdOption(args, "--locale");
        if (!locale.isPresent())
        {
            locale = Optional.of("en-US");
        }

        return new UserConfig(
            CmdOptionExists(args, "--stereo"),
            inputAudioURL.get(),
            GetCmdOption(args, "--output"),
            language.get(),
            locale.get(),
            speechSubscriptionKey.get(),
            String.format("https://%s%s", speechRegion.get(), partialSpeechEndpoint),
            languageSubscriptionKey.get(),
            String.format("https://%s", languageEndpoint.get())
        );
    }
}
