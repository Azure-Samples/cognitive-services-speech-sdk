// <copyright file="VideoTranslationConsentCreate.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation;

using System.Globalization;

public class VideoTranslationConsentCreate
{
    public string UserProvidedId { get; set; }

    public string DisplayName { get; set; }

    public string Description { get; set; }

    public CultureInfo Locale { get; set; }

    // VoiceTalentName and CompanyName are required for LocaleDefaultTemplate kind.
    public string VoiceTalentName { get; set; }

    public string CompanyName { get; set; }
}
