//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using System;
using System.Globalization;
using VoiceKind = Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview.VoiceKind;

[Verb("createTranslation", HelpText = "Create translation.")]
public partial class CreateTranslationOptions : CreateTranslationBaseOptions
{
    [Option('t', "translationId", Required = true, HelpText = "Specify translation ID.")]
    public string TranslationId { get; set; }
}

