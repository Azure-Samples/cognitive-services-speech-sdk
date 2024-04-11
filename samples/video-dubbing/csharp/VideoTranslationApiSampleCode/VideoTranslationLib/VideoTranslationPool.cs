//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using Flurl.Util;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.CommonLib.Util;
using Microsoft.SpeechServices.CustomVoice.TtsLib.TtsUtil;
using Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;
using Microsoft.SpeechServices.VideoTranslation.DataContracts.Utility;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Threading.Tasks.Dataflow;

public class VideoTranslationPool<TDeploymentEnvironment, TVideoFileMetadata>
    where TDeploymentEnvironment : Enum
    where TVideoFileMetadata : VideoFileMetadata
{
    private VideoTranslationPool()
    {
        this.TranslationResults = new ConcurrentDictionary<Guid, VideoTranslationPoolOutputResult>();

        this.waitTranslationExecutionActionBlock = new ActionBlock<(Guid translationId, IReadOnlyDictionary<string, string> additionalHeaders)>(
            async (args) =>
            {
                try
                {
                    var translation = await this.TranslationClient.QueryTaskByIdUntilTerminatedAsync<Translation>(
                        id: args.translationId,
                        additionalHeaders: args.additionalHeaders,
                        printFirstQueryResult: false,
                        timeout: TimeSpan.FromHours(3)).ConfigureAwait(false);
                    this.TranslationResults[args.translationId].Translation = translation;
                }
                catch (Exception e)
                {
                    Console.WriteLine(ExceptionHelper.BuildExceptionMessage(e, true));
                    throw;
                }
            },
            new ExecutionDataflowBlockOptions() { MaxDegreeOfParallelism = 2 });

        this.createTranslationActionBlock = new ActionBlock<VideoTranslationPoolInputArgs>(async (args) =>
        {
            var result = new VideoTranslationPoolOutputResult()
            {
                InputArgs = args,
            };

            try
            {
                var videoFileContentSha256 = Sha256Helper.GetSha256WithExtensionFromFile(args.VideoFilePath);
                var videoFile = await this.FileClient.QueryVideoFileWithLocaleAndFileContentSha256Async(
                    args.SourceLocale,
                    videoFileContentSha256).ConfigureAwait(false);
                if (videoFile == null)
                {
                    Console.WriteLine($"Uploading file: {args.VideoFilePath}");
                    videoFile = await this.FileClient.UploadVideoFileAsync<TVideoFileMetadata>(
                        name: Path.GetFileName(args.VideoFilePath),
                        description: null,
                        locale: args.SourceLocale,
                        speakerCount: null,
                        videoFilePath: args.VideoFilePath).ConfigureAwait(false);
                }

                result.VideoFile = videoFile;

                var translationCreate = new TranslationCreate()
                {
                    DisplayName = $"{videoFile.DisplayName} : {videoFile.Locale.Name} => {string.Join(",", args.TargetLocales.Select(x => x.Name))}",
                    Description = this.TranslatoinDescription,
                    VideoFileId = videoFile.ParseIdFromSelf(),
                    EnableFeatures = args.EnableFeatures,
                    ProfileName = args.ProfileName,
                    WithoutSubtitleInTranslatedVideoFile = args.WithoutSubtitleInTranslatedVideoFile,
                    SubtitleMaxCharCountPerSegment = args.SubtitleMaxCharCountPerSegment,
                    ExportPersonalVoicePromptAudioMetadata = args.ExportPersonalVoicePromptAudioMetadata,
                    PersonalVoiceModelName = args.PersonalVoiceModelName,
                    IsAssociatedWithTargetLocale = args.IsAssociatedWithTargetLocale,
                    WebvttSourceKind = args.WebvttSourceKind,
                    VoiceKind = args.VoiceKind,
                    TargetLocales = args.TargetLocales.ToDictionary(x => x, x => (TranslationTargetLocaleCreate)null),
                };

                result.Translation = await this.TranslationClient.CreateTranslationAsync(
                    translation: translationCreate,
                    sourceLocaleWebVttFilePath: null,
                    filePaths: null,
                    additionalProperties: args.AdditionalProperties,
                    additionalHeaders: args.AdditionalHeaders).ConfigureAwait(false);

                var translationId = result.Translation.ParseIdFromSelf();
                this.TranslationResults[translationId] = result;

                this.PostWaitTranslationAction(translationId, args.AdditionalHeaders);
            }
            catch (Exception e)
            {
                result.Error = $"Caught Exception with error: {e.Message}";
                Console.WriteLine(ExceptionHelper.BuildExceptionMessage(e, true));
            }

        },
        new ExecutionDataflowBlockOptions()
        {
            MaxDegreeOfParallelism = 50,
        });
    }

    public void PostWaitTranslationAction(Guid translationId, IReadOnlyDictionary<string, string> additionalHeaders)
    {
        this.waitTranslationExecutionActionBlock.Post((translationId, additionalHeaders));
    }

    public VideoTranslationClient<TDeploymentEnvironment> TranslationClient { get; private set; }

    public VideoFileClient<TDeploymentEnvironment> FileClient { get; private set; }

    public string TranslatoinDescription { get; set; }

    public ConcurrentDictionary<Guid, VideoTranslationPoolOutputResult> TranslationResults { get; private set; }

    private ActionBlock<VideoTranslationPoolInputArgs> createTranslationActionBlock;

    private ActionBlock<(Guid translationId, IReadOnlyDictionary<string, string> additionalHeaders)> waitTranslationExecutionActionBlock;

    public VideoTranslationPool(TDeploymentEnvironment environment, string subKey)
        : this()
    {
        this.FileClient = new VideoFileClient<TDeploymentEnvironment>(environment, subKey);
        this.TranslationClient = new VideoTranslationClient<TDeploymentEnvironment>(environment, subKey);
    }

    public void Post(VideoTranslationPoolInputArgs args)
    {
        this.createTranslationActionBlock.Post(args);
    }

    public async Task WaitTranslationExecutionCompletion()
    {
        Console.WriteLine("Before this.waitTranslationExecutionActionBlock.Complete()");
        
        this.waitTranslationExecutionActionBlock.Complete();

        Console.WriteLine("Before await waitTranslationExecutionActionBlock.Completion");

        await this.waitTranslationExecutionActionBlock.Completion.ConfigureAwait(false);
        Console.WriteLine("After await waitTranslationExecutionActionBlock.Completion");
    }

    public async Task CompleteAndWaitTranslationCreationCompletion()
    {
        this.createTranslationActionBlock.Complete();
        await this.createTranslationActionBlock.Completion.ConfigureAwait(false);
    }
}
