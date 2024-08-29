//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.CommandParser;

using System;
using System.Globalization;

[AttributeUsage(AttributeTargets.Field, Inherited = false)]
public sealed class ArgumentAttribute : Attribute
{
    private readonly string flag;
    private string description = string.Empty;
    private string usagePlaceholder;
    private bool optional;
    private bool hidden;
    private InOutType inoutType;
    private string requiredModes;
    private string optionalModes;

    /// <summary>
    /// Initializes a new instance of the <see cref="ArgumentAttribute"/> class.
    /// </summary>
    /// <param name="optionName">Flag string for this attribute.</param>
    public ArgumentAttribute(string optionName)
    {
        if (optionName == null)
        {
            throw new ArgumentNullException(nameof(optionName));
        }

        this.flag = optionName;
    }

    /// <summary>
    /// Gets The parse recognising flag.
    /// </summary>
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1308:NormalizeStringsToUppercase", Justification = "Ignore.")]
    public string OptionName
    {
        get { return this.flag.ToLower(CultureInfo.InvariantCulture); }
    }

    /// <summary>
    /// Gets or sets Description will display in the PrintUsage method.
    /// </summary>
    public string Description
    {
        get
        {
            return this.description;
        }

        set
        {
            if (value == null)
            {
                throw new ArgumentNullException(nameof(value));
            }

            this.description = value;
        }
    }

    /// <summary>
    /// Gets or sets In the PrintUsage method this will display a place hold for a parameter.
    /// </summary>
    public string UsagePlaceholder
    {
        get
        {
            return this.usagePlaceholder;
        }

        set
        {
            if (value == null)
            {
                throw new ArgumentNullException(nameof(value));
            }

            this.usagePlaceholder = value;
        }
    }

    /// <summary>
    /// Gets or sets a value indicating whether (optional = true) means not necessarily in the command-line.
    /// </summary>
    public bool Optional
    {
        get { return this.optional; }
        set { this.optional = value; }
    }

    /// <summary>
    /// Gets or sets a value indicating whether (Hidden = true) means this option will not be printed in the command-line.
    /// While one option is set with Hidden, the Optional must be true.
    /// </summary>
    public bool Hidden
    {
        get { return this.hidden; }
        set { this.hidden = value; }
    }

    /// <summary>
    /// Gets or sets The in/out type of argument.
    /// </summary>
    public InOutType InOutType
    {
        get { return this.inoutType; }
        set { this.inoutType = value; }
    }

    /// <summary>
    /// Gets or sets The modes require this argument.
    /// </summary>
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1308:NormalizeStringsToUppercase", Justification = "Ignore.")]
    public string RequiredModes
    {
        get
        {
            return this.requiredModes;
        }

        set
        {
            this.requiredModes = value?.ToLower(CultureInfo.InvariantCulture);
        }
    }

    /// <summary>
    /// Gets or sets The modes optionally require this argument.
    /// </summary>
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Globalization", "CA1308:NormalizeStringsToUppercase", Justification = "Ignore.")]
    public string OptionalModes
    {
        get
        {
            return this.optionalModes;
        }

        set
        {
            this.optionalModes = value?.ToLower(CultureInfo.InvariantCulture);
        }
    }

    /// <summary>
    /// Get required modes in an array.
    /// </summary>
    /// <returns>Mode array.</returns>
    public string[] GetRequiredModeArray()
    {
        string[] modes = null;
        if (!string.IsNullOrEmpty(this.requiredModes))
        {
            modes = this.requiredModes.Split(new char[] { ',', ' ' }, StringSplitOptions.RemoveEmptyEntries);
        }

        return modes;
    }

    /// <summary>
    /// Get optional modes in an array.
    /// </summary>
    /// <returns>Mode array.</returns>
    public string[] GetOptionalModeArray()
    {
        string[] modes = null;
        if (!string.IsNullOrEmpty(this.optionalModes))
        {
            modes = this.optionalModes.Split(new char[] { ',', ' ' }, StringSplitOptions.RemoveEmptyEntries);
        }

        return modes;
    }
}
