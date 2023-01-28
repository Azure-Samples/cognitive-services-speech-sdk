//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.List;
import java.util.Optional;

final class UserConfig
{
    // This should not change unless the Speech REST API changes.
    static final String partialSpeechEndpoint = ".api.cognitive.microsoft.com";    
    
    final private boolean useStereoAudio;
    final private Optional<String> inputAudioURL;
    final private Optional<String> inputFilePath;
    final private Optional<String> outputFilePath;
    final private String language;
    final private String locale;
    final private Optional<String> speechSubscriptionKey;
    final private Optional<String> speechEndpoint;
    final private String languageSubscriptionKey;
    final private String languageEndpoint;

    public boolean getUseStereoAudio()
    {
        return useStereoAudio;
    }
    public Optional<String> getInputAudioURL()
    {
        return inputAudioURL;
    }
    public Optional<String> getInputFilePath()
    {
        return inputFilePath;
    }
    public Optional<String> getOutputFilePath()
    {
        return outputFilePath;
    }
    public String getLanguage()
    {
        return language;
    }
    public String getLocale()
    {
        return locale;
    }
    public Optional<String> getSpeechSubscriptionKey()
    {
        return speechSubscriptionKey;
    }
    public Optional<String> getSpeechEndpoint()
    {
        return speechEndpoint;
    }
    public String getLanguageSubscriptionKey()
    {
        return languageSubscriptionKey;
    }
    public String getLanguageEndpoint()
    {
        return languageEndpoint;
    }

    public UserConfig(
        boolean useStereoAudio,
        Optional<String> inputAudioURL,
        Optional<String> inputFilePath,
        Optional<String> outputFilePath,
        String language,
        String locale,
        Optional<String> speechSubscriptionKey,
        Optional<String> speechEndpoint,
        String languageSubscriptionKey,
        String languageEndpoint
        )
    {
        this.useStereoAudio = useStereoAudio;
        this.inputAudioURL = inputAudioURL;
        this.inputFilePath = inputFilePath;
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
    
    static public UserConfig UserConfigFromArgs(List<String> args, String usage) throws IllegalArgumentException
    {
        Optional<String> inputAudioURL = GetCmdOption(args, "--input");
        Optional<String> inputFilePath = GetCmdOption(args, "--jsonInput");
        if (!inputAudioURL.isPresent() && !inputFilePath.isPresent())
        {
            throw new IllegalArgumentException(String.format("Please specify either --input or --jsonInput.%s%s", System.lineSeparator(), usage));
        }

        Optional<String> speechSubscriptionKey = GetCmdOption(args, "--speechKey");
        if (!speechSubscriptionKey.isPresent() && !inputFilePath.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing Speech subscription key. Speech subscription key is required unless --jsonInput is present.%s%s", System.lineSeparator(), usage));
        }
        Optional<String> speechEndpoint = Optional.empty();
        Optional<String> speechRegion = GetCmdOption(args, "--speechRegion");
        if (speechRegion.isPresent())
        {
            speechEndpoint = Optional.of(String.format("https://%s%s", speechRegion.get(), UserConfig.partialSpeechEndpoint));
        }
        else if (!inputFilePath.isPresent())
        {
            throw new IllegalArgumentException(String.format("Missing Speech region. Speech region is required unless --jsonInput is present.%s%s", System.lineSeparator(), usage));
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
        else if (!languageEndpoint.get().startsWith("https://"))
        {
            languageEndpoint = Optional.of(String.format("https://%s", languageEndpoint.get()));
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
            inputAudioURL,
            inputFilePath,
            GetCmdOption(args, "--output"),
            language.get(),
            locale.get(),
            speechSubscriptionKey,
            speechEndpoint,
            languageSubscriptionKey.get(),
            languageEndpoint.get()
        );
    }
}
