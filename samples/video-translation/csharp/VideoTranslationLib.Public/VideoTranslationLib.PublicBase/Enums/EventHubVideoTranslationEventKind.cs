// <copyright file="EventHubVideoTranslationEventKind.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// </copyright>

namespace Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;

// ********** Do NOT change enum value name, due to it is DTO contract to customer *************
public enum EventHubVideoTranslationEventKind
{
    None = 0,

    Ping,

    TranslationCompletion,

    IterationCompletion,
}
