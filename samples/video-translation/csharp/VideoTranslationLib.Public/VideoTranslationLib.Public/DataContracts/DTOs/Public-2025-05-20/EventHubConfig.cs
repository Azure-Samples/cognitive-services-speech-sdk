// <copyright file="EventHubConfig.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20250520;

using Microsoft.SpeechServices.Backend.VideoTranslationCoreEngine.Attributes;
using Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;
using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

public class EventHubConfig
{
    public bool IsEnabled { get; set; }

    [Required(AllowEmptyStrings = false, ErrorMessage = "The field is required.")]
    [RegularExpression(@"\S+", ErrorMessage = "The field cannot be pure whitespace.")]
    public string EventHubNamespaceHostName { get; set; }

    [Required(AllowEmptyStrings = false, ErrorMessage = "The field is required.")]
    [RegularExpression(@"\S+", ErrorMessage = "The field cannot be pure whitespace.")]
    public string EventHubName { get; set; }

    public Guid? ManagedIdentityClientId { get; set; }

    [MustHaveOneElement(ErrorMessage = "The collection must contain at least one element, supported values: TranslationCompletion, IterationCompletion")]
    public IEnumerable<EventHubVideoTranslationEventKind> EnabledEvents { get; set; }
}
