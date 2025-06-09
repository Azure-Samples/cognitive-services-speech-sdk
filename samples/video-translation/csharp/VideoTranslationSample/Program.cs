//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.ApiSampleCode.PublicPreview;

using CommandLine;
using Microsoft.SpeechServices.Common.Client.Enums.VideoTranslation;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.Public.Interface;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20250520;
using Microsoft.SpeechServices.VideoTranslationSample.PublicPreview;
using Newtonsoft.Json;
using System;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using VideoTranslationPublicPreviewLib.HttpClient;

internal class Program
{
    static async Task<int> Main(string[] args)
    {
        var types = LoadVerbs();

        var exitCode = await Parser.Default.ParseArguments(args, types)
            .MapResult(
                options => RunAndReturnExitCodeAsync(options),
                _ => Task.FromResult(1));

        if (exitCode == 0)
        {
            Console.WriteLine("Process completed successfully.");
        }
        else
        {
            Console.WriteLine($"Failure with exit code: {exitCode}");
        }

        return exitCode;
    }

    static async Task<int> RunAndReturnExitCodeAsync(object options)
    {
        var optionsBase = options as BaseOptions;
        ArgumentNullException.ThrowIfNull(optionsBase);
        try
        {
            return await DoRunAndReturnExitCodeAsync(optionsBase).ConfigureAwait(false);
        }
        catch (Exception e)
        {
            Console.WriteLine($"Failed to run with exception: {e.Message}");
            return CommonPublicConst.ExistCodes.GenericError;
        }
    }

    static async Task<int> DoRunAndReturnExitCodeAsync(BaseOptions baseOptions)
    {
        ArgumentNullException.ThrowIfNull(baseOptions);
        var regionConfig = new ApimApiRegionConfig(baseOptions.Region);

        var httpConfig = new VideoTranslationPublicPreviewHttpClientConfig(
            regionConfig: regionConfig,
            subKey: baseOptions.SubscriptionKey,
            customDomainName: baseOptions.customDomainName,
            managedIdentityClientId: baseOptions.ManagedIdentityClientId == Guid.Empty ? null : baseOptions.ManagedIdentityClientId)
        {
            ApiVersion = string.IsNullOrEmpty(baseOptions.ApiVersion) ?
                CommonPublicConst.ApiVersions.ApiVersion20240520Preview : baseOptions.ApiVersion,
        };

        var translationClient = new TranslationClient(httpConfig);

        var iterationClient = new IterationClient(httpConfig);

        var operationClient = new OperationClient(httpConfig);

        switch (baseOptions)
        {
            case CreateTranslationOptions options:
                {
                    var translation = new Translation()
                    {
                        Id = options.TranslationId,
                        DisplayName = options.TranslationName,
                        Description = options.TranslationDescription,
                        Input = new TranslationInput()
                        {
                            SourceLocale = options.SourceLocale,
                            TargetLocale = options.TargetLocale,
                            VoiceKind = options.VoiceKind,
                            SpeakerCount = options.SpeakerCount,
                            SubtitleMaxCharCountPerSegment = options.SubtitleMaxCharCountPerSegment,
                            ExportSubtitleInVideo = options.ExportSubtitleInVideo,
                            VideoFileUrl = options.VideoFileAzureBlobUrl,
                        },
                    };

                    translation = await translationClient.CreateTranslationAndWaitUntilTerminatedAsync(
                        translation: translation).ConfigureAwait(false);

                    Console.WriteLine();
                    Console.WriteLine("Created translation:");
                    Console.WriteLine(JsonConvert.SerializeObject(
                        translation,
                        Formatting.Indented,
                        CommonPublicConst.Json.WriterSettings));
                    break;
                }

            case QueryTranslationsOptions options:
                {
                    var translations = await translationClient.GetTranslationsAsync().ConfigureAwait(false);
                    Console.WriteLine(JsonConvert.SerializeObject(
                        translations,
                        Formatting.Indented,
                        CommonPublicConst.Json.WriterSettings));
                    break;
                }

            case QueryTranslationOptions options:
                {
                    var translation = await translationClient.GetTranslationAsync(
                        options.TranslationId).ConfigureAwait(false);
                    Console.WriteLine(JsonConvert.SerializeObject(
                        translation,
                        Formatting.Indented,
                        CommonPublicConst.Json.WriterSettings));
                    break;
                }

            case DeleteTranslationOptions options:
                {
                    var response = await translationClient.DeleteTranslationAsync(
                        options.TranslationId).ConfigureAwait(false);
                    Console.WriteLine(response.StatusCode);
                    break;
                }

            case CreateIterationAndWaitUntilTerminatedOptions options:
                {
                    var iteration = new Iteration()
                    {
                        Id = options.IterationId,
                        DisplayName = string.IsNullOrEmpty(options.IterationName) ?
                            options.IterationId : options.IterationName,
                        Input = new IterationInput()
                        {
                            SpeakerCount = options.SpeakerCount,
                            SubtitleMaxCharCountPerSegment = options.SubtitleMaxCharCountPerSegment,
                            ExportSubtitleInVideo = options.ExportSubtitleInVideo,
                            WebvttFile = options.WebvttFile,
                            TtsCustomLexiconFileIdInAudioContentCreation = options.TtsCustomLexiconFileIdInAudioContentCreation == Guid.Empty ?
                                null : options.TtsCustomLexiconFileIdInAudioContentCreation,
                            TtsCustomLexiconFileUrl = options.TtsCustomLexiconFileUrl,
                            EnableVideoSpeedAdjustment = options.EnableVideoSpeedAdjustment ? true : null,
                            EnableOcrCorrectionFromSubtitle = options.EnableOcrCorrectionFromSubtitle ? true : null,
                            ExportTargetLocaleAdvancedSubtitleFile = options.ExportTargetLocaleAdvancedSubtitleFile ? true : null,
                            SubtitlePrimaryColor = options.SubtitlePrimaryRgbaColor,
                            SubtitleOutlineColor = options.SubtitleOutlineRgbaColor,
                            EnableEmotionalPlatformVoice = options.EnableEmotionalPlatformVoice ==
                                EnableEmotionalPlatformVoiceKind.Auto ? null : options.EnableEmotionalPlatformVoice,
                            SubtitleFontSize = options.SubtitleFontSize == 0 ? null : options.SubtitleFontSize,
                        }
                    };

                    var iterationResponse = await iterationClient.CreateIterationAndWaitUntilTerminatedAsync(
                        translationId: options.TranslationId,
                        iteration: iteration,
                        additionalHeaders: null).ConfigureAwait(false);
                    break;
                }

            case QueryIterationsOptions options:
                {
                    var translations = await translationClient.GetIterationsAsync(
                        options.TranslationId).ConfigureAwait(false);
                    Console.WriteLine(JsonConvert.SerializeObject(
                        translations,
                        Formatting.Indented,
                        CommonPublicConst.Json.WriterSettings));
                    break;
                }

            case QueryIterationOptions options:
                {
                    var translations = await translationClient.GetIterationAsync(
                        translationId: options.TranslationId,
                        iterationId: options.IterationId).ConfigureAwait(false);
                    Console.WriteLine(JsonConvert.SerializeObject(
                        translations,
                        Formatting.Indented,
                        CommonPublicConst.Json.WriterSettings));
                    break;
                }

            case CreateTranslationAndIterationAndWaitUntilTerminatedOptions options:
                {
                    var iteration = new Iteration()
                    {
                        Id = options.IterationId,
                        DisplayName = options.IterationId,
                        Input = new IterationInput()
                        {
                            SpeakerCount = options.SpeakerCount,
                            SubtitleMaxCharCountPerSegment = options.SubtitleMaxCharCountPerSegment,
                            ExportSubtitleInVideo = options.ExportSubtitleInVideo,
                            WebvttFile = options.WebvttFileAzureBlobUrl == null ? null : new WebvttFile()
                            {
                                Kind = options.WebvttFileKind == WebvttFileKind.None ?
                                    throw new ArgumentException($"Please specify {nameof(options.WebvttFileKind)}") :
                                    options.WebvttFileKind,
                                Url = options.WebvttFileAzureBlobUrl,
                            },
                            TtsCustomLexiconFileUrl = options.TtsCustomLexiconFileUrl,
                            TtsCustomLexiconFileIdInAudioContentCreation = options.TtsCustomLexiconFileIdInAudioContentCreation == Guid.Empty ?
                                null : options.TtsCustomLexiconFileIdInAudioContentCreation,
                            EnableVideoSpeedAdjustment = options.EnableVideoSpeedAdjustment ? true : null,
                            EnableOcrCorrectionFromSubtitle = options.EnableOcrCorrectionFromSubtitle ? true : null,
                            ExportTargetLocaleAdvancedSubtitleFile = options.ExportTargetLocaleAdvancedSubtitleFile ? true : null,
                            SubtitlePrimaryColor = options.SubtitlePrimaryRgbaColor,
                            SubtitleOutlineColor = options.SubtitleOutlineRgbaColor,
                            SubtitleFontSize = options.SubtitleFontSize == 0 ? null : options.SubtitleFontSize,
                            EnableEmotionalPlatformVoice = options.EnableEmotionalPlatformVoice ==
                                EnableEmotionalPlatformVoiceKind.Auto ? null : options.EnableEmotionalPlatformVoice,
                        }
                    };

                    var translation = new Translation()
                    {
                        Id = options.TranslationId,
                        DisplayName = options.TranslationId,
                        Description = options.TranslationId,
                        Input = new TranslationInput()
                        {
                            SourceLocale = options.SourceLocale,
                            TargetLocale = options.TargetLocale,
                            VoiceKind = options.VoiceKind,
                            SpeakerCount = iteration.Input?.SpeakerCount,
                            SubtitleMaxCharCountPerSegment = iteration.Input?.SubtitleMaxCharCountPerSegment,
                            ExportSubtitleInVideo = iteration.Input?.ExportSubtitleInVideo,
                            VideoFileUrl = options.VideoFileAzureBlobUrl,
                        }
                    };

                    (translation, iteration) = await translationClient.CreateTranslationAndIterationAndWaitUntilTerminatedAsync(
                        translation: translation,
                        iteration: iteration).ConfigureAwait(false);
                    break;
                }

            default:
                throw new NotSupportedException();
        }

        return CommonPublicConst.ExistCodes.NoError;
    }

    //load all types using Reflection
    private static Type[] LoadVerbs()
    {
        return Assembly.GetExecutingAssembly().GetTypes()
            .Where(t => t.GetCustomAttribute<VerbAttribute>() != null).ToArray();
    }
}
