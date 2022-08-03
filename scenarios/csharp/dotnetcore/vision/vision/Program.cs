//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

namespace Vision
{
    public class Program
    {
        private UserConfig m_userConfig;
        
        // How long to wait while polling TODO1.
        private const int waitSeconds = 10;

        private const string imageAnalysisPath = "/vision/v4.0-preview.1/operations/imageanalysis:analyze";

        private async Task<string> AnalyzeImage()
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, m_userConfig.endpoint);
            uri.Path = imageAnalysisPath;
            var content = new
            {
                url = m_userConfig.imageAnalysisURL
            };
            var response = await RestHelper.SendPost(uri.Uri.ToString(), JsonSerializer.Serialize(content), m_userConfig.subscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            return response.content;
        }

        // Note: To pass command-line arguments, run:
        // dotnet run -- [args]
        // For example:
        // dotnet run -- --help
        public async Task Run()
        {
            var result = await AnalyzeImage();
            Console.WriteLine(result);
            return;
        }

        public async static Task Main(string[] args)
        {
            string usage = @"USAGE: dotnet run -- [...]

  HELP
    --help                          Show this help and stop.

  CONNECTION
    --key KEY                       Your Azure Computer Vision subscription key. Required.
    --endpoint ENDPOINT             Your Azure Computer Vision endpoint. Required.

  INPUT
    --input URL                     The URL of the image you want to analyze. Required.
";
            
            try
            {
                if (args.Contains("--help"))
                {
                    Console.WriteLine(usage);
                }
                else
                {
                    await (new Program { m_userConfig = new UserConfig(args, usage) }).Run();
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }            
        }
    }
}
