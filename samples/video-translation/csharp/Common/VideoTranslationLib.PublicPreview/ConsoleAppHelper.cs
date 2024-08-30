//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslationLib.PublicPreview.Base;

using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;
using Microsoft.SpeechServices.CustomVoice.TtsLib.Util;
using Newtonsoft.Json;
using System;
using System.Globalization;
using System.Threading.Tasks;
using VideoTranslationPublicPreviewLib.HttpClient;

public static class ConsoleAppHelper
{
    public static async Task CreateTranslationAsync<TDeploymentEnvironment, TIteration>(
        TranslationClient<TDeploymentEnvironment, TIteration> translationClient,
        string translationId,
        CultureInfo sourceLocale,
        CultureInfo targetLocale,
        VoiceKind voiceKind,
        Uri videoFileUrl,
        int? speakerCount)
        where TDeploymentEnvironment : Enum
        where TIteration : Iteration
    {
        ArgumentNullException.ThrowIfNull(translationClient);
        if (string.IsNullOrWhiteSpace(videoFileUrl?.OriginalString))
        {
            throw new ArgumentNullException(nameof(videoFileUrl));
        }

        var fileName = UriHelper.GetFileName(videoFileUrl);
        var translation = new Translation<Iteration>()
        {
            Id = translationId,
            DisplayName = fileName,
            Description = $"Translation {fileName} from {sourceLocale} to {targetLocale} with {voiceKind.AsString()}",
            Input = new TranslationInput()
            {
                SourceLocale = sourceLocale,
                TargetLocale = targetLocale,
                SpeakerCount = speakerCount,
                VoiceKind = voiceKind,
                VideoFileUrl = videoFileUrl,
            },
        };

        var operationId = Guid.NewGuid().ToString();
        (translation, var headers) = await translationClient.CreateTranslationAsync(
            translation: translation,
            operationId: operationId).ConfigureAwait(false);

        Console.WriteLine();
        Console.WriteLine("Created translation:");
        Console.WriteLine(JsonConvert.SerializeObject(
            translation,
            Formatting.Indented,
            CustomContractResolver.WriterSettings));
    }

    public static async Task CreateTranslationAndIterationAndWaitUntilTerminatedAsync<TDeploymentEnvironment, TIteration>(
        TranslationClient<TDeploymentEnvironment, TIteration> translationClient,
        string translationId,
        CultureInfo sourceLocale,
        CultureInfo targetLocale,
        VoiceKind voiceKind,
        TIteration iteration,
        Uri videoFileUrl)
        where TDeploymentEnvironment : Enum
        where TIteration : Iteration
    {
        ArgumentNullException.ThrowIfNull(translationClient);
        if (string.IsNullOrWhiteSpace(videoFileUrl?.OriginalString))
        {
            throw new ArgumentNullException(nameof(videoFileUrl));
        }

        var fileName = UriHelper.GetFileName(videoFileUrl);
        var translation = new Translation<TIteration>()
        {
            Id = translationId,
            DisplayName = fileName,
            Description = $"Translation {fileName} from {sourceLocale} to {targetLocale} with {voiceKind.AsString()}",
            Input = new TranslationInput()
            {
                SpeakerCount = iteration.Input?.SpeakerCount,
                EnableLipSync = iteration.Input?.EnableLipSync,
                SubtitleMaxCharCountPerSegment = iteration.Input?.SubtitleMaxCharCountPerSegment,
                ExportSubtitleInVideo = iteration.Input?.ExportSubtitleInVideo,
                SourceLocale = sourceLocale,
                TargetLocale = targetLocale,
                VoiceKind = voiceKind,
                VideoFileUrl = videoFileUrl,
            }
        };

        (translation, iteration) = await translationClient.CreateTranslationAndIterationAndWaitUntilTerminatedAsync(
            translation: translation,
            iteration: iteration).ConfigureAwait(false);

    }
}
