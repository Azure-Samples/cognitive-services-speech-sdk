//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Linq;

namespace Vision
{
    public class UserConfig
    {
        /// The URL of the image to analyze.
        readonly public string imageAnalysisURL;
        /// The subscription key for your Computer Vision subscription.
        readonly public string subscriptionKey;
        /// The endpoint for your Computer Vision subscription.
        readonly public string endpoint;

        public static string? GetCmdOption(string[] args, string option)
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

        public static bool CmdOptionExists(string[] args, string option)
        {
            return args.Contains(option);
        }

        public UserConfig(string[] args, string usage)
        {
            string? subscriptionKey = GetCmdOption(args, "--key");
            if (subscriptionKey is null)
            {
                throw new ArgumentException($"Missing Computer Vision subscription key.{Environment.NewLine}Usage: {usage}");
            }
            string? endpoint = GetCmdOption(args, "--endpoint");
            if (endpoint is null)
            {
                throw new ArgumentException($"Missing Computer Vision endpoint.{Environment.NewLine}Usage: {usage}");
            }

            string? imageAnalysisURL = GetCmdOption(args, "--input");
            if (imageAnalysisURL is null)
            {
                throw new ArgumentException($"Missing URL of image to analyze.{Environment.NewLine}Usage: {usage}");
            }
            
            this.imageAnalysisURL = imageAnalysisURL;
            this.subscriptionKey = subscriptionKey;
            this.endpoint = endpoint;
        }
    }
}