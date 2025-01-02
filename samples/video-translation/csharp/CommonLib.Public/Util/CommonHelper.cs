//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.TtsUtil;

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;

public static class CommonHelper
{
    public static void CopyDirectory(string sourceDir, string destinationDir, bool recursive)
    {
        // Get information about the source directory
        var dir = new DirectoryInfo(sourceDir);

        // Check if the source directory exists
        if (!dir.Exists)
            throw new DirectoryNotFoundException($"Source directory not found: {dir.FullName}");

        // Cache directories before we start copying
        DirectoryInfo[] dirs = dir.GetDirectories();

        // Create the destination directory
        Directory.CreateDirectory(destinationDir);

        // Get the files in the source directory and copy to the destination directory
        foreach (FileInfo file in dir.GetFiles())
        {
            string targetFilePath = Path.Combine(destinationDir, file.Name);
            file.CopyTo(targetFilePath);
        }

        // If recursive and copying subdirectories, recursively call this method
        if (recursive)
        {
            foreach (DirectoryInfo subDir in dirs)
            {
                string newDestinationDir = Path.Combine(destinationDir, subDir.Name);
                CopyDirectory(subDir.FullName, newDestinationDir, true);
            }
        }
    }

    /// <summary>
    /// Format the string in language independent way.
    /// </summary>
    /// <param name="format">Format string.</param>
    /// <param name="arg">Arguments to format.</param>
    /// <returns>Formated string.</returns>
    public static string NeutralFormat(string format, params object[] arg)
    {
        if (string.IsNullOrEmpty(format))
        {
            throw new ArgumentNullException(nameof(format));
        }

        return string.Format(CultureInfo.InvariantCulture, format, arg);
    }

    /// <summary>
    /// Ensure directory exist for certain file path, this is,
    /// If the directory does not exist, create it.
    /// </summary>
    /// <param name="filePath">File path to process.</param>
    public static void EnsureFolderExistForFile(string filePath)
    {
        string dir = Path.GetDirectoryName(filePath);
        EnsureFolderExist(dir);
    }

    /// <summary>
    /// Ensure directory exist for certain file path, this is,
    /// If the directory does not exist, create it.
    /// </summary>
    /// <param name="dirPath">Directory path to process.</param>
    public static void EnsureFolderExist(string dirPath)
    {
        if (!string.IsNullOrEmpty(dirPath) &&
            !Directory.Exists(dirPath))
        {
            Directory.CreateDirectory(dirPath);
        }
    }

    /// <summary>
    /// Checks file exists.
    /// </summary>
    /// <param name="filePath">FilePath.</param>
    public static void ThrowIfFileNotExist(string filePath)
    {
        ThrowIfNull(filePath);
        if (!File.Exists(filePath))
        {
            throw CreateException(typeof(FileNotFoundException), filePath);
        }
    }

    /// <summary>
    /// Create new exception instance with given exception type and parameter.
    /// </summary>
    /// <param name="type">Exception type.</param>
    /// <param name="parameter">
    /// 1. if type is FileNotFoundException/DirectoryNotFoundException,
    /// Parameter should be file/directory name.
    /// </param>
    /// <returns>Exception created.</returns>
    public static Exception CreateException(Type type, string parameter)
    {
        if (type == null)
        {
            throw new ArgumentNullException(nameof(type));
        }

        // check public parameter
        if (string.IsNullOrEmpty(parameter))
        {
            parameter = string.Empty;
        }

        if (type.Equals(typeof(FileNotFoundException)))
        {
            string message = $"Could not find file [{parameter}].";
            return new FileNotFoundException(message, parameter);
        }
        else if (type.Equals(typeof(DirectoryNotFoundException)))
        {
            string message = $"Could not find a part of the path [{parameter}].";
            return new DirectoryNotFoundException(message);
        }
        else
        {
            string message = $"Unsupported exception message with parameter [{parameter}]";
            return new NotSupportedException(message);
        }
    }

    /// <summary>
    /// Checks dir exists.
    /// </summary>
    /// <param name="dirPath">DirPath.</param>
    public static void ThrowIfDirectoryNotExist(string dirPath)
    {
        ThrowIfNull(dirPath);
        if (!Directory.Exists(dirPath))
        {
            throw CreateException(typeof(DirectoryNotFoundException), dirPath);
        }
    }

    public static void ThrowIfNullOrEmpty(string instance)
    {
        if (string.IsNullOrEmpty(instance))
        {
            throw new ArgumentNullException(nameof(instance));
        }
    }

    /// <summary>
    /// Checks object argument not as null.
    /// </summary>
    /// <param name="instance">Object instance to check.</param>
    public static void ThrowIfNull(object instance)
    {
        if (instance == null)
        {
            throw new ArgumentNullException(nameof(instance));
        }
    }

    public static IEnumerable<string> FileLines(Stream stream, Encoding encoding)
    {
        ArgumentNullException.ThrowIfNull(stream);
        ArgumentNullException.ThrowIfNull(encoding);

        using (StreamReader sr = new StreamReader(stream, encoding))
        {
            string line = null;
            while ((line = sr.ReadLine()) != null)
            {
                yield return line;
            }
        }
    }

    public static IEnumerable<string> FileLines(string filePath)
    {
        return FileLines(filePath, true);
    }

    public static IEnumerable<string> FileLines(string filePath, bool ignoreBlankLine)
    {
        if (string.IsNullOrEmpty(filePath))
        {
            throw new ArgumentNullException(nameof(filePath));
        }

        using (StreamReader sr = new StreamReader(filePath))
        {
            string line = null;
            while ((line = sr.ReadLine()) != null)
            {
                if (ignoreBlankLine && string.IsNullOrEmpty(line.Trim()))
                {
                    continue;
                }

                yield return line;
            }
        }
    }

    public static IEnumerable<string> FileLines(string filePath, Encoding encoding)
    {
        return FileLines(filePath, encoding, true);
    }

    public static IEnumerable<string> FileLines(string filePath, Encoding encoding, bool ignoreBlankLine)
    {
        if (string.IsNullOrEmpty(filePath))
        {
            throw new ArgumentNullException(nameof(filePath));
        }

        if (encoding == null)
        {
            throw new ArgumentNullException(nameof(encoding));
        }

        using (StreamReader sr = new StreamReader(filePath, encoding))
        {
            string line = null;
            while ((line = sr.ReadLine()) != null)
            {
                if (ignoreBlankLine && string.IsNullOrEmpty(line.Trim()))
                {
                    continue;
                }

                yield return line;
            }
        }
    }

}
