// <copyright file="VideoTranslationConsent.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation;

using System;
using System.Globalization;

public class VideoTranslationConsent : StatefulResourceBase
{
    public CultureInfo Locale { get; set; }

    // VoiceTalentName and CompanyName are required for LocaleDefaultTemplate kind.
    public string VoiceTalentName { get; set; }

    public string CompanyName { get; set; }

    public Uri AudioFileUrl { get; set; }

    public string FailureReason { get; set; }
}
