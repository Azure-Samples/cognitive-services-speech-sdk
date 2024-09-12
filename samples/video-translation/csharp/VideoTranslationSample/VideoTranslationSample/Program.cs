//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.ApiSampleCode.PublicPreview;

using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.CommandParser;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.Cris.Http.DTOs.Public.VideoTranslation.Public20240520Preview;
using Microsoft.SpeechServices.CustomVoice.TtsLib.Util;
using Microsoft.SpeechServices.VideoTranslationLib.Enums;
using Microsoft.SpeechServices.VideoTranslationLib.PublicPreview.Base;
using Microsoft.VisualBasic;
using Newtonsoft.Json;
using System;
using System.Threading.Tasks;
using VideoTranslationPublicPreviewLib.HttpClient;

internal class Program
{
    static async Task<int> Main(string[] args)
    {
        ConsoleMaskSasHelper.ShowSas = true;
        return await ConsoleApp<Arguments<DeploymentEnvironment>>.RunAsync(
            args,
            ProcessAsync<DeploymentEnvironment>).ConfigureAwait(false);
    }

    public static async Task<int> ProcessAsync<TDeploymentEnvironment>(
        Arguments<TDeploymentEnvironment> args)
        where TDeploymentEnvironment : Enum
    {
        try
        {
            var httpConfig = new VideoTranslationPublicPreviewHttpClientConfig<TDeploymentEnvironment>(
                args.Environment,
                args.SpeechSubscriptionKey)
            {
                ApiVersion = args.ApiVersion,
            };

            var translationClient = new TranslationClient<TDeploymentEnvironment, Iteration<IterationInput>, IterationInput>(httpConfig);

            var iterationClient = new IterationClient<TDeploymentEnvironment>(httpConfig);

            var operationClient = new OperationClient<TDeploymentEnvironment>(httpConfig);

            switch (args.Mode)
            {
                case Mode.CreateTranslationAndIterationAndWaitUntilTerminated:
                    {
                        var iteration = new Iteration<IterationInput>()
                        {
                            Id = args.IterationId,
                            DisplayName = args.IterationId,
                            Input = new IterationInput()
                            {
                                SpeakerCount = args.SpeakerCount,
                                SubtitleMaxCharCountPerSegment = args.SubtitleMaxCharCountPerSegment,
                                ExportSubtitleInVideo = args.ExportSubtitleInVideo,
                                WebvttFile = args.TypedWebvttFileAzureBlobUrl == null ? null : new WebvttFile()
                                {
                                    Kind = args.TypedWebvttFileKind ?? WebvttFileKind.TargetLocaleSubtitle,
                                    Url = args.TypedWebvttFileAzureBlobUrl,
                                }
                            }
                        };

                        var voiceKind = args.TypedVoiceKind ?? VoiceKind.PlatformVoice;
                        var fileName = UriHelper.GetFileName(args.TypedVideoFileAzureBlobUrl);
                        var translation = new Translation<Iteration<IterationInput>, IterationInput>()
                        {
                            Id = args.TranslationId,
                            DisplayName = fileName,
                            Description = $"Translation {fileName} from {args.TypedSourceLocale} to {args.TypedTargetLocale} with {voiceKind.AsString()}",
                            Input = new TranslationInput()
                            {
                                SpeakerCount = iteration.Input?.SpeakerCount,
                                SubtitleMaxCharCountPerSegment = iteration.Input?.SubtitleMaxCharCountPerSegment,
                                ExportSubtitleInVideo = iteration.Input?.ExportSubtitleInVideo,
                                SourceLocale = args.TypedSourceLocale,
                                TargetLocale = args.TypedTargetLocale,
                                VoiceKind = voiceKind,
                                VideoFileUrl = args.TypedVideoFileAzureBlobUrl,
                            }
                        };

                        (translation, iteration) = await translationClient.CreateTranslationAndIterationAndWaitUntilTerminatedAsync(
                            translation: translation,
                            iteration: iteration).ConfigureAwait(false);
                        break;
                    }

                case Mode.CreateTranslation:
                    {
                        await ConsoleAppHelper.CreateTranslationAsync(
                            translationClient: translationClient,
                            translationId: args.TranslationId,
                            sourceLocale: args.TypedSourceLocale,
                            targetLocale: args.TypedTargetLocale,
                            voiceKind: args.TypedVoiceKind ?? VoiceKind.PlatformVoice,
                            videoFileUrl: args.TypedVideoFileAzureBlobUrl,
                            speakerCount: args.SpeakerCount).ConfigureAwait(false);
                        break;
                    }

                case Mode.QueryTranslations:
                    {
                        var translations = await translationClient.GetTranslationsAsync().ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            translations,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.QueryTranslation:
                    {
                        var translation = await translationClient.GetTranslationAsync(
                            args.TranslationId).ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            translation,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.DeleteTranslation:
                    {
                        var response = await translationClient.DeleteTranslationAsync(args.TranslationId).ConfigureAwait(false);
                        Console.WriteLine(response.StatusCode);
                        break;
                    }

                case Mode.CreateIteration:
                    {
                        var iteration = new Iteration<IterationInput>()
                        {
                            Id = args.IterationId,
                            DisplayName = args.IterationId,
                            Input = new IterationInput()
                            {
                                SpeakerCount = args.SpeakerCount,
                                SubtitleMaxCharCountPerSegment = args.SubtitleMaxCharCountPerSegment,
                                ExportSubtitleInVideo = args.ExportSubtitleInVideo,
                                WebvttFile = args.TypedWebvttFileAzureBlobUrl == null ? null : new WebvttFile()
                                {
                                    Kind = args.TypedWebvttFileKind ?? WebvttFileKind.TargetLocaleSubtitle,
                                    Url = args.TypedWebvttFileAzureBlobUrl,
                                }
                            }
                        };

                        var iterationResponse = await iterationClient.CreateIterationAndWaitUntilTerminatedAsync(
                            translationId: args.TranslationId,
                            iteration: iteration,
                            additionalHeaders: null).ConfigureAwait(false);
                        break;
                    }

                case Mode.QueryIterations:
                    {
                        var translations = await translationClient.GetIterationsAsync(args.TranslationId).ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            translations,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.QueryIteration:
                    {
                        var translations = await translationClient.GetIterationAsync(args.TranslationId, args.IterationId).ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            translations,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                default:
                    throw new NotSupportedException(args.Mode.AsString());
            }
        }
        catch (Exception e)
        {
            Console.WriteLine($"Failed to run with exception: {e.Message}");
            return ExitCode.GenericError;
        }

        Console.WriteLine();
        Console.WriteLine("Process completed successfully.");

        return ExitCode.NoError;
    }
}
