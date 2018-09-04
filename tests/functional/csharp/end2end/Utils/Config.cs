//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    static class Config
    {
        public static T GetSettingByKey<T>(TestContext context, string key)
        {
            var value = context.Properties[key];
            if (string.IsNullOrWhiteSpace(value.ToString()) || value.Equals(string.Empty))
            {
                throw new System.Exception("Field " + key + " is missing in the runsettings");
            }
            return (T)(object)(value);
        }

        public static List<byte[]> GetByteArraysForFilesWithSamePrefix(string dir, string prefix)
        {
            List<byte[]> byteArrList = new List<byte[]>();
            var fileInfos = new DirectoryInfo(dir).GetFiles(string.Concat(prefix,"*.*"));
            for (int i = 1; i <= fileInfos.Length; i++)
            {
                byteArrList.Add(File.ReadAllBytes(Path.Combine(dir, string.Concat(prefix, i, ".wav"))));
            }
            return byteArrList;
        }

        public static async Task<string> GetToken(string key)
        {
            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.Add("Ocp-Apim-Subscription-Key", key);
                UriBuilder uriBuilder = new UriBuilder("https://westus.api.cognitive.microsoft.com/sts/v1.0");
                uriBuilder.Path += "/issueToken";

                using (var result = await client.PostAsync(uriBuilder.Uri.AbsoluteUri, null))
                {
                    Console.WriteLine("Token Uri: {0}", uriBuilder.Uri.AbsoluteUri);
                    if (result.IsSuccessStatusCode)
                    {
                        return await result.Content.ReadAsStringAsync();
                    }
                    else
                    {
                        throw new HttpRequestException($"Cannot get token from {uriBuilder.ToString()}. Error: {result.StatusCode}");
                    }
                }
            }
        }
    }
}
