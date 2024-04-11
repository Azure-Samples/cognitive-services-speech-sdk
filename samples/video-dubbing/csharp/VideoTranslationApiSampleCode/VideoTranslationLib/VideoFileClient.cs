//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Net;
using System.Threading.Tasks;
using System.Xml.Linq;
using Flurl;
using Flurl.Http;
using Microsoft.SpeechServices.CommonLib.Enums;
using Microsoft.SpeechServices.DataContracts;
using Microsoft.SpeechServices.VideoTranslation.DataContracts.DTOs;

public class VideoFileClient<TDeploymentEnvironment> : VideoTranslationClientBase<TDeploymentEnvironment>
    where TDeploymentEnvironment : System.Enum
{
    public VideoFileClient(TDeploymentEnvironment environment, string subKey)
        : base(environment, subKey)
    {
    }

    public override string ControllerName => "VideoFiles";

    public async Task<VideoFileTargetLocale> QueryTargetLocaleAsync(
        Guid videoOrAudioFileId,
        CultureInfo targetLocale)
    {
        var url = this.BuildRequestBase();
        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
               .AppendPathSegment(videoOrAudioFileId)
               .AppendPathSegment(targetLocale.Name)
               .GetAsync()
               .ReceiveJson<VideoFileTargetLocale>()
               .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<string> UploadVideoFileWithStringResponseAsync(
        string name,
        string description,
        CultureInfo locale,
        int? speakerCount,
        string videoFilePath)
    {
        return await this.RequestWithRetryAsync(async () =>
        {
            var response = this.PostUploadVideoFileWithResponseAsync(
                name: name,
                description: description,
                locale: locale,
                speakerCount: speakerCount,
                videoFilePath: videoFilePath);

            return await response
                .ReceiveString();
        }).ConfigureAwait(false);
    }

    public async Task<TVideoFileMetadata> UploadVideoFileAsync<TVideoFileMetadata>(
        string name,
        string description,
        CultureInfo locale,
        int? speakerCount,
        string videoFilePath)
        where TVideoFileMetadata : VideoFileMetadata
    {
        return await this.RequestWithRetryAsync(async () =>
        {
            var response = this.PostUploadVideoFileWithResponseAsync(
                name: name,
                description: description,
                locale: locale,
                speakerCount: speakerCount,
                videoFilePath: videoFilePath);

            return await response
                .ReceiveJson<TVideoFileMetadata>();
        }).ConfigureAwait(false);
    }

    public async Task<PaginatedResources<VideoFileMetadata>> QueryVideoFilesAsync()
    {
        var url = this.BuildRequestBase();
        return await this.RequestWithRetryAsync(async () =>
        {
            return await url.GetAsync()
                .ReceiveJson<PaginatedResources<VideoFileMetadata>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<IFlurlResponse> DeleteVideoFileAsync(
        Guid videoFileId,
        bool deleteAssociations)
    {
        var queryParams = new Dictionary<string, string>();
        if (deleteAssociations)
        {
            queryParams["deleteAssociations"] = bool.TrueString;
        }

        return await this.RequestWithRetryAsync(async () =>
        {
            return await this.DeleteByIdAsync(videoFileId, queryParams).ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<PaginatedResources<Translation>> QueryTranslationsAsync(Guid videoFileId)
    {
        var url = this.BuildRequestBase()
            .AppendPathSegment(videoFileId.ToString())
            .AppendPathSegment("translations");

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .GetAsync()
                .ReceiveJson<PaginatedResources<Translation>>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<IFlurlResponse> DeleteTargetLocaleAsync(
        Guid videoFileId,
        CultureInfo locale,
        bool deleteAssociations)
    {
        var url = this.BuildRequestBase()
            .AppendPathSegment(videoFileId.ToString())
            .AppendPathSegment(locale.Name);
        if (deleteAssociations)
        {
            url = url.SetQueryParam("deleteAssociations", deleteAssociations);
        }

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .DeleteAsync()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<WebVttFileMetadata> QueryTargetLocaleWebVttAsync(
        Guid videoFileId,
        CultureInfo locale)
    {
        var url = this.BuildRequestBase()
            .AppendPathSegment(videoFileId.ToString())
            .AppendPathSegment(locale.Name)
            .AppendPathSegment("webvtt");

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .GetAsync()
                .ReceiveJson<WebVttFileMetadata>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    public async Task<VideoFileMetadata> QueryVideoFileWithLocaleAndFileContentSha256Async(
        CultureInfo locale,
        string fileContentSha256)
    {
        if (locale == null || string.IsNullOrEmpty(locale.Name))
        {
            throw new ArgumentNullException(nameof(locale));
        }

        if (string.IsNullOrEmpty(fileContentSha256))
        {
            throw new ArgumentNullException(nameof(fileContentSha256));
        }

        var url = this.BuildRequestBase()
            .AppendPathSegment("QueryByFileContentSha256")
            .SetQueryParam("locale", locale.Name)
            .SetQueryParam("fileContentSha256", fileContentSha256);

        return await this.RequestWithRetryAsync(async () =>
        {
            try
            {
                return await url
                    .GetAsync()
                    .ReceiveJson<VideoFileMetadata>()
                    .ConfigureAwait(false);
            }
            catch (FlurlHttpException ex)
            {
                if (ex.StatusCode == (int)HttpStatusCode.NotFound)
                {
                    return null;
                }

                Console.Write($"Response failed with error: {await ex.GetResponseStringAsync().ConfigureAwait(false)}");
                throw;
            }
        }).ConfigureAwait(false);
    }

    public async Task<TVideoFileMetadata> QueryVideoFileAsync<TVideoFileMetadata>(Guid id)
        where TVideoFileMetadata : VideoFileMetadata
    {
        var url = this.BuildRequestBase();

        return await this.RequestWithRetryAsync(async () =>
        {
            return await url
                .AppendPathSegment(id.ToString())
                .GetAsync()
                .ReceiveJson<TVideoFileMetadata>()
                .ConfigureAwait(false);
        }).ConfigureAwait(false);
    }

    private async Task<IFlurlResponse> PostUploadVideoFileWithResponseAsync(
        string name,
        string description,
        CultureInfo locale,
        int? speakerCount,
        string videoFilePath)
    {
        if (string.IsNullOrWhiteSpace(name))
        {
            throw new ArgumentNullException(nameof(name));
        }

        var url = this.BuildRequestBase();

        url.ConfigureRequest(settings => settings.Timeout = VideoTranslationConstant.UploadVideoOrAudioFileTimeout);

        return await url
            .PostMultipartAsync(mp =>
            {
                if (!string.IsNullOrWhiteSpace(name))
                {
                    mp.AddString(nameof(VideoFileMetadata.DisplayName), name);
                }

                if (!string.IsNullOrWhiteSpace(description))
                {
                    mp.AddString(nameof(VideoFileMetadata.Description), description);
                }

                if (locale != null && !string.IsNullOrEmpty(locale.Name))
                {
                    mp.AddString(nameof(VideoFileMetadata.Locale), locale.Name);
                }

                if (speakerCount != null)
                {
                    mp.AddString(nameof(VideoFileMetadata.SpeakerCount), speakerCount.Value.ToString(CultureInfo.InvariantCulture));
                }

                mp.AddFile("videoFile", videoFilePath);
            });
    }
}
