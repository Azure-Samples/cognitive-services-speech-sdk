//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.CommonLib;

[Verb("queryIteration", HelpText = "Query iteration by translation ID and iteration ID.")]
public class QueryIterationOptions : BaseOptions
{
    [Option("translationId", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.TranslationId)]
    public string TranslationId { get; set; }

    [Option("iterationId", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.IterationId)]
    public string IterationId { get; set; }
}

