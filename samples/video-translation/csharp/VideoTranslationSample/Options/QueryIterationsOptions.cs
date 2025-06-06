//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.CommonLib;

[Verb("queryIterations", HelpText = "Query iterations.")]
public class QueryIterationsOptions : BaseOptions
{
    [Option("translationId", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.TranslationId)]
    public string TranslationId { get; set; }
}

