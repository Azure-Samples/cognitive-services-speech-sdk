//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CommonLib.CommandParser;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;
using Microsoft.SpeechServices.VideoTranslation.Enums;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

public class Program
{
    static async Task<int> Main(string[] args)
    {
        ConsoleMaskSasHelper.ShowSas = true;
        return await ConsoleApp<Arguments<DeploymentEnvironment>>.RunAsync(
            args,
            ProcessAsync<DeploymentEnvironment, VideoFileMetadata>).ConfigureAwait(false);
    }

    public static async Task<int> ProcessAsync<TDeploymentEnvironment, TVideoFileMetadata>(
        Arguments<TDeploymentEnvironment> args)
        where TVideoFileMetadata : VideoFileMetadata
        where TDeploymentEnvironment : Enum
    {
        try
        {
            if (!string.IsNullOrEmpty(args.ApiVersion))
            {
                HttpClientBase<DeploymentEnvironment>.ApiVersion = args.ApiVersion;
            }

            var translationClient = new VideoTranslationClient<TDeploymentEnvironment>(args.Environment, args.SpeechSubscriptionKey);
            var fileClient = new VideoFileClient<TDeploymentEnvironment>(args.Environment, args.SpeechSubscriptionKey);
            var metadataClient = new VideoTranslationMetadataClient<TDeploymentEnvironment>(args.Environment, args.SpeechSubscriptionKey);
            var targetLocaleClient = new TargetLocaleClient<TDeploymentEnvironment>(args.Environment, args.SpeechSubscriptionKey);

            switch (args.Mode)
            {
                case Mode.QueryMetadata:
                    {
                        var metadata = await metadataClient.QueryMetadataAsync().ConfigureAwait(false);

                        Console.WriteLine(JsonConvert.SerializeObject(
                            metadata,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.QueryTargetLocales:
                    {
                        var targetLocales = await targetLocaleClient.QueryTargetLocalesAsync().ConfigureAwait(false);

                        Console.WriteLine(JsonConvert.SerializeObject(
                            targetLocales,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.QueryTargetLocale:
                    {
                        var targetLocale = await fileClient.QueryTargetLocaleAsync(
                            args.VideoOrAudioFileId,
                            args.TypedTargetLocales.First()).ConfigureAwait(false);

                        Console.WriteLine(JsonConvert.SerializeObject(
                            targetLocale,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.UpdateTargetLocaleEdittingWebvttFile:
                    {
                        var targetLocale = await targetLocaleClient.UpdateTargetLocaleEdittingWebvttFileAsync(
                            videoFileId: args.VideoOrAudioFileId,
                            targetLocale: args.TypedTargetLocales.First(),
                            kind: !string.IsNullOrEmpty(args.SourceLocaleWebvttFilePath) ? VideoTranslationWebVttFilePlainTextKind.SourceLocalePlainText : null,
                            webvttFilePath: !string.IsNullOrEmpty(args.SourceLocaleWebvttFilePath) ?
                             args.SourceLocaleWebvttFilePath :
                             args.TargetLocaleWebvttFilePath).ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            targetLocale,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.DeleteTargetLocale:
                    {
                        await fileClient.DeleteTargetLocaleAsync(
                            args.VideoOrAudioFileId,
                            args.TypedTargetLocales.First(),
                            args.DeleteAssociations).ConfigureAwait(false);
                        break;
                    }

                case Mode.UploadVideoOrAudioFile:
                    {
                        if (!File.Exists(args.SourceVideoOrAudioFilePath))
                        {
                            throw new FileNotFoundException(args.SourceVideoOrAudioFilePath);
                        }

                        Console.WriteLine($"Uploading file: {args.SourceVideoOrAudioFilePath}");
                        var videoFile = await fileClient.UploadVideoFileAsync<TVideoFileMetadata>(
                            name: Path.GetFileName(args.SourceVideoOrAudioFilePath),
                            description: null,
                            locale: args.TypedSourceLocale,
                            speakerCount: null,
                            videoFilePath: args.SourceVideoOrAudioFilePath).ConfigureAwait(false);

                        Console.WriteLine(JsonConvert.SerializeObject(
                            videoFile,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.UploadVideoOrAudioFileIfNotExist:
                    {
                        if (!File.Exists(args.SourceVideoOrAudioFilePath))
                        {
                            throw new FileNotFoundException(args.SourceVideoOrAudioFilePath);
                        }

                        var fileContentSha256 = Sha256Helper.GetSha256WithExtensionFromFile(args.SourceVideoOrAudioFilePath);
                        var videoFile = await fileClient.QueryVideoFileWithLocaleAndFileContentSha256Async(
                            args.TypedSourceLocale,
                            fileContentSha256).ConfigureAwait(false);
                        if (videoFile == null)
                        {
                            Console.WriteLine($"Uploading file: {args.SourceVideoOrAudioFilePath}");
                            videoFile = await fileClient.UploadVideoFileAsync<TVideoFileMetadata>(
                                name: Path.GetFileName(args.SourceVideoOrAudioFilePath),
                                description: null,
                                locale: args.TypedSourceLocale,
                                speakerCount: null,
                                videoFilePath: args.SourceVideoOrAudioFilePath).ConfigureAwait(false);
                        }

                        Console.WriteLine(JsonConvert.SerializeObject(
                            videoFile,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.UploadVideoOrAudioFileAndCreateTranslation:
                    {
                        if (string.IsNullOrEmpty(args.SourceVideoOrAudioFilePath))
                        {
                            throw new ArgumentException($"Please provide at least one of {nameof(args.VideoOrAudioFileId)} or {nameof(args.SourceVideoOrAudioFilePath)}");
                        }

                        if (args.TypedSourceLocale == null || string.IsNullOrEmpty(args.TypedSourceLocale.Name))
                        {
                            throw new ArgumentNullException(nameof(args.TypedSourceLocale));
                        }

                        if (!File.Exists(args.SourceVideoOrAudioFilePath))
                        {
                            throw new FileNotFoundException(args.SourceVideoOrAudioFilePath);
                        }

                        VideoFileMetadata videoOrAudioFile = null;
                        if (args.ReuseExistingVideoOrAudioFile)
                        {
                            var fileContentSha256 = Sha256Helper.GetSha256WithExtensionFromFile(args.SourceVideoOrAudioFilePath);
                            videoOrAudioFile = await fileClient.QueryVideoFileWithLocaleAndFileContentSha256Async(
                                args.TypedSourceLocale,
                                fileContentSha256).ConfigureAwait(false);
                        }

                        if (videoOrAudioFile == null)
                        {
                            Console.WriteLine($"Uploading file: {args.SourceVideoOrAudioFilePath}");
                            videoOrAudioFile = await fileClient.UploadVideoFileAsync<TVideoFileMetadata>(
                                name: Path.GetFileName(args.SourceVideoOrAudioFilePath),
                                description: null,
                                locale: args.TypedSourceLocale,
                                speakerCount: null,
                                videoFilePath: args.SourceVideoOrAudioFilePath).ConfigureAwait(false);
                            Console.WriteLine($"Uploaded new video file with ID {videoOrAudioFile.ParseIdFromSelf()} uploaded.");
                        }
                        else
                        {
                            Console.WriteLine($"Reuse existing video file with ID {videoOrAudioFile.ParseIdFromSelf()}.");
                        }

                        Console.WriteLine(JsonConvert.SerializeObject(
                            videoOrAudioFile,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));

                        var translation = await DoCreateTranslationAsync(
                            client: translationClient,
                            videoOrAudioFile: videoOrAudioFile,
                            args: args).ConfigureAwait(false);
                        if (translation == null)
                        {
                            return ExitCode.GenericError;
                        }

                        break;
                    }

                case Mode.QueryVideoOrAudioFiles:
                    {
                        var videoFiles = await fileClient.QueryVideoFilesAsync().ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            videoFiles,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.QueryVideoOrAudioFile:
                    {
                        var videoFile = await fileClient.QueryVideoFileAsync<TVideoFileMetadata>(args.Id).ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            videoFile,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.DeleteVideoOrAudioFile:
                    {
                        var response = await fileClient.DeleteVideoFileAsync(args.Id, args.DeleteAssociations).ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            ((HttpStatusCode)response.StatusCode).AsString()),
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings);
                        break;
                    }

                case Mode.CreateTranslation:
                    {
                        var videoOrAudioFile = await fileClient.QueryVideoFileAsync<TVideoFileMetadata>(
                            args.VideoOrAudioFileId).ConfigureAwait(false);
                        if (videoOrAudioFile == null)
                        {
                            throw new InvalidDataException($"Failed to find video or audio file with ID {args.VideoOrAudioFileId}");
                        }

                        var translation = await DoCreateTranslationAsync(
                            client: translationClient,
                            videoOrAudioFile: videoOrAudioFile,
                            args: args).ConfigureAwait(false);
                        if (translation == null)
                        {
                            return ExitCode.GenericError;
                        }

                        break;
                    }

                case Mode.QueryTranslations:
                    {
                        var translations = await translationClient.QueryTranslationsAsync().ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            translations,
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings));
                        break;
                    }

                case Mode.QueryTranslation:
                    {
                        var translation =  await translationClient.QueryTaskByIdUntilTerminatedAsync<Translation>(
                            id: args.Id,
                            additionalHeaders: args.TypedAdditionalHttpHeaders,
                            printFirstQueryResult: true).ConfigureAwait(false);
                        if (translation == null)
                        {
                            Console.WriteLine($"Failed to find translation with ID: {args.Id}");
                        }

                        break;
                    }

                case Mode.DeleteTranslation:
                    {
                        var response = await translationClient.DeleteTranslationAsync(args.Id).ConfigureAwait(false);
                        Console.WriteLine(JsonConvert.SerializeObject(
                            ((HttpStatusCode)response.StatusCode).AsString()),
                            Formatting.Indented,
                            CustomContractResolver.WriterSettings);
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

    private static async Task<Translation> DoCreateTranslationAsync<TDeploymentEnvironment>(
        VideoTranslationClient<TDeploymentEnvironment> client,
        VideoFileMetadata videoOrAudioFile,
        Arguments<TDeploymentEnvironment> args)
        where TDeploymentEnvironment : Enum
    {
        var targetLocaleDefinition = new TranslationTargetLocaleCreate();
        var filePaths = new Dictionary<string, string>();
        var targetLocales = new Dictionary<CultureInfo, TranslationTargetLocaleCreate>();

        if (args.TypedTargetLocales.Count() == 1)
        {
            targetLocales[args.TypedTargetLocales.Single()] = targetLocaleDefinition;

            if (!string.IsNullOrEmpty(args.TargetLocaleWebvttFilePath))
            {
                if (!File.Exists(args.TargetLocaleWebvttFilePath))
                {
                    throw new FileNotFoundException(args.TargetLocaleWebvttFilePath);
                }

                targetLocaleDefinition.WebVttFileName = Path.GetFileName(args.TargetLocaleWebvttFilePath);
                filePaths[targetLocaleDefinition.WebVttFileName] = args.TargetLocaleWebvttFilePath;
            }
        }
        else
        {
            foreach (var targetLocale in args.TypedTargetLocales)
            {
                targetLocales[targetLocale] = null;
            }
        }

        var translationDefinition = new TranslationCreate()
        {
            VideoFileId = videoOrAudioFile.ParseIdFromSelf(),
            VoiceKind = args.VoiceKind,
            AlignKind = null,
            DisplayName = $"{videoOrAudioFile.DisplayName} : {videoOrAudioFile.Locale.Name} => {string.Join(",", args.TypedTargetLocales.Select(x => x.Name))}",
            TargetLocales = targetLocales,
            EnableFeatures = args.EnableFeatures,
            ProfileName = args.ProfileName,
            PersonalVoiceModelName = args.PersonalVoiceModelName,
            IsAssociatedWithTargetLocale = args.IsAssociatedWithTargetLocale,
            WebvttSourceKind = args.TypedWebvttSourceKind,
        };

        translationDefinition.WithoutSubtitleInTranslatedVideoFile = args.WithoutSubtitleInTranslatedVideoFile;

        translationDefinition.ExportPersonalVoicePromptAudioMetadata = args.ExportPersonalVoicePromptAudioMetadata;
        
        var translation = await client.CreateTranslationAsync(
            translation: translationDefinition,
            sourceLocaleWebVttFilePath: args.SourceLocaleWebvttFilePath,
            filePaths: filePaths,
            additionalProperties: args.TypedCreateTranslationAdditionalProperties,
            additionalHeaders: args.TypedAdditionalHttpHeaders).ConfigureAwait(false);
        if (translation == null)
        {
            return translation;
        }

        Console.WriteLine();
        Console.WriteLine($"New translation created with ID {translation.ParseIdFromSelf()}.");

        // Print resposne of creating instead of first query state to make sure resposne of creating correct.
        Console.WriteLine(JsonConvert.SerializeObject(
            translation,
            Formatting.Indented,
            CustomContractResolver.WriterSettings));
        var createdTranslation = await client.QueryTaskByIdUntilTerminatedAsync<Translation>(
            id: translation.ParseIdFromSelf(),
            additionalHeaders: args.TypedAdditionalHttpHeaders,
            printFirstQueryResult: false).ConfigureAwait(false);
        if (createdTranslation == null)
        {
            Console.WriteLine($"Failed to find translation with ID: {translation.ParseIdFromSelf()}");
            return null;
        }

        return createdTranslation;
    }
}