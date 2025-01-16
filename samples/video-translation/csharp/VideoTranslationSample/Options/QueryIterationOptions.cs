//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;

[Verb("queryIteration", HelpText = "Query iteration by translation ID and iteration ID.")]
public class QueryIterationOptions : BaseOptions
{
    [Option('t', "translationId", Required = true, HelpText = "Specify translation ID.")]
    public string TranslationId { get; set; }

    [Option('i', "iterationId", Required = true, HelpText = "Specify iteration ID.")]
    public string IterationId { get; set; }
}

