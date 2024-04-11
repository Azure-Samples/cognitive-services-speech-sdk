//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation;

public enum Mode
{
    None = 0,

    QueryMetadata,

    UploadVideoOrAudioFile,

    UploadVideoOrAudioFileIfNotExist,

    UploadVideoOrAudioFileAndCreateTranslation,

    CreateTranslation,

    DeleteVideoOrAudioFile,

    QueryVideoOrAudioFile,

    QueryVideoOrAudioFiles,

    DeleteTranslation,

    QueryTranslation,

    QueryTranslations,

    QueryTargetLocales,

    QueryTargetLocale,

    UpdateTargetLocaleEdittingWebvttFile,

    DeleteTargetLocale,
}