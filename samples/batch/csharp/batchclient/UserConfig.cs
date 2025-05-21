using System;
using System.Collections.Generic;
using System.Linq;

namespace BatchClient
{
    public class UserConfig
    {
        /// SAS URI pointing to an audio files stored in Azure Blob Storage
        readonly public List<Uri> recordingsBlobUris;
        /// SAS URI pointing to an container in Azure Blob Storage
        readonly public Uri contentAzureBlobContainer;
        readonly public string locale;
        /// The resource key for your Speech service subscription.
        readonly public string subscriptionKey;
        /// The region for your Speech service subscription.
        readonly public string region;

        public UserConfig(
            string subscriptionKey,
            string region,
            string locale,
            string recordingsBlobUris = null,
            string contentAzureBlobContainer = null

            )
        {
            this.recordingsBlobUris = recordingsBlobUris?
                .Split(',', StringSplitOptions.RemoveEmptyEntries)
                .Select(uri => new Uri(uri.Trim()))
                .ToList();
            this.locale = locale;
            this.subscriptionKey = subscriptionKey;
            this.region = region;
            this.contentAzureBlobContainer = string.IsNullOrEmpty(contentAzureBlobContainer) ? null : new Uri(contentAzureBlobContainer.Trim());
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
            else
            {
                throw new ArgumentException($"Please provide a Speech resource key with the --key option.{Environment.NewLine}Usage: {usage}");
            }

            string region;
            if (GetCmdOption(args, "--region") is string regionOptionValue)
            {
                region = regionOptionValue;
            }
            else
            {
                throw new ArgumentException($"Please provide a Speech region with the --region option.{Environment.NewLine}Usage: {usage}");
            }

            string recordingsBlobUris;
            if (GetCmdOption(args, "--recordingsBlobUris") is string recordingsBlobUrisOptionValue)
            {
                recordingsBlobUris = recordingsBlobUrisOptionValue;
            }
            else
            {
                recordingsBlobUris = null;
            }

            string recordingsContainerUri;
            if (GetCmdOption(args, "--recordingsContainerUri") is string recordingsContainerUriOptionValue)
            {
                recordingsContainerUri = recordingsContainerUriOptionValue;
            }
            else
            {
                recordingsContainerUri = null;
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
                key,
                region,
                locale,
                recordingsBlobUris,
                recordingsContainerUri
            );
        }
    }
}
