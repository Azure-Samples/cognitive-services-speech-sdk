//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.Util;

using Microsoft.SpeechServices.Common;
using System;
using System.Globalization;
using System.IO;
using System.Security.Cryptography;
using System.Text;

public static class Sha256Helper
{
    public static string GetSha256FromFile(string filename)
    {
        if (string.IsNullOrEmpty(filename))
        {
            throw new ArgumentNullException(nameof(filename));
        }

        if (!File.Exists(filename))
        {
            throw new FileNotFoundException(filename);
        }

        using (var md5Hash = SHA256.Create())
        using (FileStream stream = File.OpenRead(filename))
        {
            byte[] data = md5Hash.ComputeHash(stream);
            var sb = new StringBuilder();

            for (int i = 0; i < data.Length; i++)
            {
                sb.Append(data[i].ToString("x2", CultureInfo.InvariantCulture));
            }

            return sb.ToString();
        }
    }

    public static string GetSha256WithExtensionFromFile(string filename)
    {
        return $"{GetSha256FromFile(filename).AppendExtensionName(Path.GetExtension(filename))}";
    }

    public static string GetSha256FromString(string value)
    {
        value = value ?? string.Empty;
        using (var md5Hash = SHA256.Create())
        {
            byte[] data = md5Hash.ComputeHash(Encoding.UTF8.GetBytes(value));

            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < data.Length; i++)
            {
                sb.Append(data[i].ToString("x2", CultureInfo.InvariantCulture));
            }

            return sb.ToString();
        }
    }
}
