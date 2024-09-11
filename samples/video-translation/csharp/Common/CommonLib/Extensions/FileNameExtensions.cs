//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.Common;

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

public static class FileNameExtensions
{
    public const char FileExtensionDelimiter = '.';

    public const string Mp3 = "mp3";

    public const string Mp4 = "mp4";

    public const string CloudAudioMetadataFile = "metadata";

    public const string VideoTranslationDubbingMetricsReferenceYaml = "info.yaml";

    public const string CloudAudioTsvFile = "tsv";

    public const string Waveform = "wav";

    public const string RawWave = "raw";

    public const string Ogg = "ogg";

    public const string Text = "txt";

    public const string Tsv = "tsv";

    public const string Xml = "xml";

    public const string Yaml = "yaml";

    public const string Configuration = "config";

    public const string CsvFile = "csv";

    public const string ExcelFile = "xlsx";

    public const string ZipFile = "zip";

    public const string HtmlFile = "html";

    public const string PngFile = "png";

    public const string JpegFile = "jpeg";

    public const string JsonFile = "json";

    public const string Zip7Z = "7z";

    public const string IniFile = "ini";

    public const string LgMarkdownFile = "lg";

    public const string PdfFile = "pdf";

    public const string PptxFile = "pptx";

    public const string WaveformRaw = "raw";

    public const string SubRipFile = "srt";

    public const string WebVttFile = "vtt";

    public const string WebmVideoFile = "webm";

    public const string M4aAudioFile = "m4a";

    public const string PitchF0File = "if0";

    public static string EnsureExtensionWithoutDelimiter(this string extension)
    {
        string extensionWithoutDelimeter = string.Empty;
        if (!string.IsNullOrEmpty(extension))
        {
            if (extension[0] == FileExtensionDelimiter)
            {
                extensionWithoutDelimeter = extension.Substring(1);
            }
            else
            {
                extensionWithoutDelimeter = extension;
            }
        }

        return extensionWithoutDelimeter;
    }

    public static string EnsureExtensionWithDelimiter(this string extension)
    {
        string extensionWithDelimiter = extension;
        if (!string.IsNullOrEmpty(extension))
        {
            if (extension[0] != FileExtensionDelimiter)
            {
                extensionWithDelimiter = FileExtensionDelimiter + extension;
            }
            else
            {
                extensionWithDelimiter = extension;
            }
        }

        return extensionWithDelimiter;
    }

    public static string AppendExtensionName(this string file, string extensionName)
    {
        extensionName = extensionName ?? string.Empty;
        return (string.IsNullOrEmpty(extensionName) || extensionName[0] == FileExtensionDelimiter) ? file + extensionName : file + FileExtensionDelimiter + extensionName;
    }

    public static bool IsWithFileExtension(this string file, string extensionName)
    {
        if (string.IsNullOrEmpty(file))
        {
            throw new ArgumentNullException(nameof(file));
        }

        if (string.IsNullOrEmpty(extensionName))
        {
            throw new ArgumentNullException(nameof(extensionName));
        }

        if (extensionName[0] != FileExtensionDelimiter)
        {
            extensionName = FileExtensionDelimiter + extensionName;
        }

        return file.EndsWith(extensionName, StringComparison.OrdinalIgnoreCase);
    }

    public static bool IsSameFileExtension(this string actualExtension, string expectedExtension)
    {
        if (string.IsNullOrEmpty(actualExtension))
        {
            throw new ArgumentNullException(nameof(actualExtension));
        }

        if (string.IsNullOrEmpty(expectedExtension))
        {
            throw new ArgumentNullException(nameof(expectedExtension));
        }

        string actualExtensionWithoutDelimeter = actualExtension;
        if (actualExtension[0] == FileExtensionDelimiter)
        {
            actualExtensionWithoutDelimeter = actualExtension.Substring(1);
        }

        string expectedExtensionWithoutDelimeter = expectedExtension;
        if (expectedExtension[0] == FileExtensionDelimiter)
        {
            expectedExtensionWithoutDelimeter = expectedExtension.Substring(1);
        }

        bool isSame = true;
        if (string.CompareOrdinal(actualExtensionWithoutDelimeter, expectedExtensionWithoutDelimeter) != 0)
        {
            isSame = false;
        }

        return isSame;
    }

    public static bool IsSupportedFileExtension(this string actualExtension, IEnumerable<string> supportedExtensions, StringComparison stringComparison = StringComparison.CurrentCulture)
    {
        ArgumentNullException.ThrowIfNull(actualExtension);
        ArgumentNullException.ThrowIfNull(supportedExtensions);

        string actualExtensionWithoutDelimeter = actualExtension.EnsureExtensionWithoutDelimiter();
        var supportedExtensionsWithoutDelimeter = supportedExtensions.Select(extension => extension.EnsureExtensionWithoutDelimiter());

        return supportedExtensionsWithoutDelimeter.Any(extenstion => actualExtensionWithoutDelimeter.Equals(extenstion, stringComparison));
    }

    public static string CreateSearchPatternWithFileExtension(this string fileExtension)
    {
        if (string.IsNullOrEmpty(fileExtension))
        {
            throw new ArgumentNullException(nameof(fileExtension));
        }

        return "*".AppendExtensionName(fileExtension);
    }

    public static string RemoveFilePathExtension(this string filePath)
    {
        if (string.IsNullOrEmpty(filePath))
        {
            throw new ArgumentNullException(nameof(filePath));
        }

        return Path.Combine(Path.GetDirectoryName(filePath), Path.GetFileNameWithoutExtension(filePath));
    }

    public static string ChangeFilePathExtension(this string filePath, string newFileNameExtension)
    {
        if (string.IsNullOrEmpty(filePath))
        {
            throw new ArgumentNullException(nameof(filePath));
        }

        return Path.GetFileNameWithoutExtension(filePath).AppendExtensionName(newFileNameExtension);
    }

    public static bool HasFileExtension(this string fileName)
    {
        try
        {
            return !string.IsNullOrWhiteSpace(fileName) && !string.IsNullOrWhiteSpace(fileName.GetFileExtension());
        }
        catch (ArgumentException)
        {
            return false;
        }
    }

    public static string GetFileExtension(this string fileName, bool withDelimiter = true)
    {
        if (string.IsNullOrWhiteSpace(fileName))
        {
            throw new ArgumentException("The file name is either an empty string, null or whitespace.", nameof(fileName));
        }

        try
        {
            var fileInfo = new FileInfo(fileName);

            if (!withDelimiter)
            {
                if (fileInfo.Extension.StartsWith(FileExtensionDelimiter.ToString(), StringComparison.InvariantCultureIgnoreCase))
                {
                    return fileInfo.Extension.Substring(1);
                }
            }

            return fileInfo.Extension;
        }
        catch (ArgumentException)
        {
            return string.Empty;
        }
    }

    public static string GetLowerCaseFileNameExtensionWithoutDot(string fileName)
    {
        if (string.IsNullOrEmpty(fileName))
        {
            throw new ArgumentNullException(nameof(fileName));
        }

        var extension = Path.GetExtension(fileName);
        if (string.IsNullOrEmpty(extension))
        {
            return extension;
        }

        extension = extension.TrimStart('.');
#pragma warning disable CA1308 // Normalize strings to uppercase
        return extension.ToLowerInvariant();
#pragma warning restore CA1308 // Normalize strings to uppercase
    }

    public static string GetFileNameExtensionFromCodec(string codec)
    {
        var fileExtension = FileNameExtensions.Waveform;
        if (codec.IndexOf("riff", StringComparison.OrdinalIgnoreCase) >= 0)
        {
            fileExtension = FileNameExtensions.Waveform;
        }
        else if (codec.IndexOf("mp3", StringComparison.OrdinalIgnoreCase) >= 0)
        {
            fileExtension = FileNameExtensions.Mp3;
        }
        else if (codec.IndexOf("raw", StringComparison.OrdinalIgnoreCase) >= 0)
        {
            fileExtension = FileNameExtensions.RawWave;
        }
        else if (codec.IndexOf("json", StringComparison.OrdinalIgnoreCase) >= 0)
        {
            fileExtension = FileNameExtensions.JsonFile;
        }

        return fileExtension;
    }
}
