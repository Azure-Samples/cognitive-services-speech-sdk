using Microsoft.CognitiveServices.Speech;

namespace samples;

public static class ConfigurationExtension
{
    public static SpeechConfig GetSpeechConfig(this IConfiguration configuration)
    {
        var section = configuration.GetSection("SpeechService");
        var key = section.GetValue<string>("Key");
        var region = section.GetValue<string>("Region");

        if (string.IsNullOrWhiteSpace(key) || string.IsNullOrWhiteSpace(region))
        {
            return null;
        }

        return SpeechConfig.FromSubscription(key, region);
    }
}
