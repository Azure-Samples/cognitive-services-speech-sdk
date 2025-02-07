using System;
using System.Collections.Generic;
using System.Linq;

namespace BatchClient
{
    public class UserConfig
    {
        /// A SAS URI pointing to an audio file stored in Azure Blob Storage
        readonly public List<Uri> recordingsBlobUri;
        /// The locale of the recordings
        readonly public string locale;
        /// The resource key for your Speech service subscription.
        readonly public string subscriptionKey;
        /// The region for your Speech service subscription.
        readonly public string region;

        public UserConfig(
            string recordingsBlobUri,
            string locale,
            string subscriptionKey,
            string region
            )
        {
            this.recordingsBlobUri = recordingsBlobUri
                .Split(',', StringSplitOptions.RemoveEmptyEntries)
                .Select(uriStr => new Uri(uriStr.Trim()))
                .ToList();
            this.locale = locale;
            this.subscriptionKey = subscriptionKey;
            this.region = region;
        }

        private static string GetCmdOption(string[] args, string option)
        {
            int index = Array.FindIndex(args, x => x.Equals(option, StringComparison.OrdinalIgnoreCase));
            if (index > -1 && index < args.Length - 1)
            {
                // We found the option (for example, "--output"), so advance from that to the value (for example, "filename").
                return args[index + 1];
            }
            else
            {
                return null;
            }
        }

        public static UserConfig UserConfigFromArgs(string[] args, string usage)
        {
            string key;
            if (GetCmdOption(args, "--key") is string keyOptionValue)
            {
                key = keyOptionValue;
            }
            else if (Environment.GetEnvironmentVariable("SPEECH_KEY") is string keyValue)
            {
                key = keyValue;
            }
            else
            {
                throw new ArgumentException($"Please set the SPEECH_KEY environment variable or provide a Speech resource key with the --subscriptionKey option.{Environment.NewLine}Usage: {usage}");
            }

            string region;
            if (GetCmdOption(args, "--region") is string regionOptionValue)
            {
                region = regionOptionValue;
            }
            else if (Environment.GetEnvironmentVariable("SPEECH_REGION") is string regionValue)
            {
                region = regionValue;
            }
            else
            {
                throw new ArgumentException($"Please set the SPEECH_REGION environment variable or provide a Speech region with the --region option.{Environment.NewLine}Usage: {usage}");
            }

            string recordingsBlobUri;
            if (GetCmdOption(args, "--recordingsBlobUri") is string recordingsBlobUriOptionValue)
            {
                recordingsBlobUri = recordingsBlobUriOptionValue;
            }
            else
            {
                throw new ArgumentException($"Please provide SAS URIs (if input multiple audios, please separate them with commas.) pointing to audio files stored in Azure Blob Storage with the --recordingsBlobSasUri option.{Environment.NewLine}Usage: {usage}");
            }

            string locale;
            if (GetCmdOption(args, "--locale") is string localeOptionValue)
            {
                locale = localeOptionValue;
            }
            else
            {
                throw new ArgumentException($"Please provide a locale of audios with the --locale option.{Environment.NewLine}Usage: {usage}");
            }

            return new UserConfig(
                recordingsBlobUri,
                locale,
                key,
                region
            );
        }
    }
}
