//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Collections.Generic;
using System.IO;

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
    }
}
