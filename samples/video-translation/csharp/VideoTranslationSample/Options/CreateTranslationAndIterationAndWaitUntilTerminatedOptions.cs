//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using System;

[Verb("createTranslationAndIterationAndWaitUntilTerminated", HelpText = "Create translation and create first iteration until terminated.")]
public partial class CreateTranslationAndIterationAndWaitUntilTerminatedOptions : CreateTranslationAndIterationAndWaitUntilTerminatedBaseOptions
{
    [Option('v', "videoFileAzureBlobUrl", Required = true, HelpText = "Specify video file Azure blob URL.")]
    public Uri VideoFileAzureBlobUrl { get; set; }
}

