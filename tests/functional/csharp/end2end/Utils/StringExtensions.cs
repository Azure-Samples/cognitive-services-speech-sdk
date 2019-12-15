//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.IO;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils
{
    using static Config;

    public static class StringExtensions
    {
        public static string GetRootRelativePath(this string filePath)
        {
            return Path.Combine(DefaultSettingsMap[DefaultSettingKeys.INPUT_DIR], filePath);
        }
    }
}
