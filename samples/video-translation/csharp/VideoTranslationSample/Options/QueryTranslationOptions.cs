//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;

[Verb("queryTranslation", HelpText = "Query translation by ID.")]
public class QueryTranslationOptions : BaseOptions
{
    [Option('t', "translationId", Required = true, HelpText = "Specify translation ID.")]
    public string TranslationId { get; set; }
}

