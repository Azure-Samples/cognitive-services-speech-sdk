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
using System.Text.RegularExpressions;
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
        private const string getImageDescriptionQuery = "visualFeatures=description";

        private const string imgTagPattern = "<img.*?\\/>";
        // \ssrc = ignore data-src attributes.
        // (?!\/ = ignore src attributes with relative paths.
        // (?!data: = ignore src attributes that contain base64-encoded data.
        private const string imgSrcPattern = "\\ssrc=\"(?!\\/|data:)(?<src>.*?)\"";
        // \salt = ignore data-alt attributes.
        private const string imgAltPattern = "\\salt=\"(?<alt>.*?)\"";

        private async Task<string> AnalyzeImage(string url)
        {
            var uri = new UriBuilder(Uri.UriSchemeHttps, m_userConfig.endpoint);
            uri.Path = imageAnalysisPath;
            uri.Query = getImageDescriptionQuery;
            var content = new { url = url };
            var response = await RestHelper.SendPost(uri.Uri.ToString(), JsonSerializer.Serialize(content), m_userConfig.subscriptionKey, new HttpStatusCode[]{ HttpStatusCode.OK });
            return response.content;
        }

        // TODO1 Break this up.
        private async Task GetPage()
        {
            using (HttpClient client = new HttpClient())
            using (HttpResponseMessage response = await client.GetAsync(this.m_userConfig.imageAnalysisURL))
            using (HttpContent content = response.Content)
            {
                // TODO1 See if we can submit a batch request, or send all requests asynchronously.
                string result = await content.ReadAsStringAsync();
                foreach (Match imgMatch in Regex.Matches(result, imgTagPattern, RegexOptions.IgnoreCase))
                {
                    Console.WriteLine($"Image tag: {imgMatch.ToString()}");
                    Match srcMatch = Regex.Match(imgMatch.Value, imgSrcPattern, RegexOptions.IgnoreCase);
                    if (srcMatch.Success)
                    {
                        string analyzeResult = await AnalyzeImage(srcMatch.Groups[1].Value);
                        using (JsonDocument document = JsonDocument.Parse(analyzeResult))
                        {
                            Match altMatch = Regex.Match(imgMatch.Value, imgAltPattern, RegexOptions.IgnoreCase);
//                            if (match.Groups.TryGetValue("alt", out Group? alt) && alt is Group altValue)
                            if (altMatch.Success)
                            {
                                Console.WriteLine($"Existing alt value: {altMatch.Groups[1].Value}");
                            }
                            string description = document.RootElement.Clone().GetProperty("describeResult").GetProperty("description").GetProperty("captions").EnumerateArray().First().GetProperty("text").ToString();
                            Console.WriteLine($"New alt value: {description}");
                            Console.WriteLine();
                        }
                        break;
                    }
                }
            }
        }

        public Program(string[] args, string usage)
        {
            this.m_userConfig = new UserConfig(args, usage);
        }

        // Note: To pass command-line arguments, run:
        // dotnet run -- [args]
        // For example:
        // dotnet run -- --help
        public async Task Run()
        {
            await GetPage();
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
    --input URL                     The URL of the Web page that contains the images to analyze. Required.
";
            
            try
            {
                if (args.Contains("--help"))
                {
                    Console.WriteLine(usage);
                }
                else
                {
                    await (new Program(args, usage)).Run();
                }
            }
            catch(Exception e)
            {
                Console.WriteLine(e.Message);
            }            
        }
    }
}
