// <copyright file="Consent.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;

using System;
using System.ComponentModel.DataAnnotations;
using System.Globalization;

public class Consent : StatefulResourceBase
{
    [Required]
    public CultureInfo Locale { get; set; }

    [Required]
    public string VoiceTalentName { get; set; }

    [Required]
    public string CompanyName { get; set; }

    public Uri AudioUrl { get; set; }

    public string FailureReason { get; set; }
}
