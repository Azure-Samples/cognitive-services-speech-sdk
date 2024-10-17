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

        var config = SpeechConfig.FromSubscription(key, region);

        var enableLogging = configuration.GetValue<bool>("EnableSDKLogging");

        if (enableLogging)
        {
            var contentRoot = configuration.GetValue<string>(WebHostDefaults.ContentRootKey);

            var directory = Path.Combine(contentRoot, "logs");
            var fileFullname = Path.Combine(directory, $"log-{DateTime.Now:yyyy-MM-dd}.log");

            if (!Path.Exists(fileFullname))
            {
                if (!Directory.Exists(directory))
                {
                    Directory.CreateDirectory(directory);
                }

                using var logFile = new FileStream(fileFullname, FileMode.Create);
            }

            config.EnableAudioLogging();
            config.SetProperty(PropertyId.Speech_LogFilename, fileFullname);
        }

        return config;
    }
}
