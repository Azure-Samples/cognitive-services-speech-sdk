//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;
using System;
using System.Collections.Generic;

[Verb("createOrUpdateEventHubConfig", HelpText = "Create or update EventHub configuration.")]
public class CreateOrUpdateEventHubConfigOptions : BaseOptions
{
    [Option("isEnabled", Required = true, HelpText = "Indicate whether enable the event hub notification.")]
    public bool IsEnabled { get; set; }

    [Option("eventHubNamespaceHostName", Required = true, HelpText = "EventHub namespace host name.")]
    public string EventHubNamespaceHostName { get; set; }

    [Option("eventHubName", Required = true, HelpText = "EventHub name.")]
    public string EventHubName { get; set; }

    [Option("enabledEvents",  Separator = ',', Required = true, HelpText = "Enabled events, for example: TranslationCompletion,IterationCompletion")]
    public IEnumerable<EventHubVideoTranslationEventKind> EnabledEvents { get; set; }
}

