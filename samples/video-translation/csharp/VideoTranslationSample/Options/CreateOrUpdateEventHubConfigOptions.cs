//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;
using Microsoft.SpeechServices.CommonLib;
using System;
using System.Collections.Generic;

[Verb("createOrUpdateEventHubConfig", HelpText = "Create or update EventHub configuration.")]
public class CreateOrUpdateEventHubConfigOptions : BaseOptions
{
    [Option("isEnabled", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.EventHub.IsEnabled)]
    public bool IsEnabled { get; set; }

    [Option("eventHubNamespaceHostName", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.EventHub.EventHubNamespaceHostName)]
    public string EventHubNamespaceHostName { get; set; }

    [Option("eventHubName", Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.EventHub.EventHubName)]
    public string EventHubName { get; set; }

    [Option("enabledEvents",  Separator = ',', Required = true, HelpText = VideoTranslationPublicConst.ArgumentDescription.EventHub.EnabledEvents)]
    public IEnumerable<EventHubVideoTranslationEventKind> EnabledEvents { get; set; }
}

