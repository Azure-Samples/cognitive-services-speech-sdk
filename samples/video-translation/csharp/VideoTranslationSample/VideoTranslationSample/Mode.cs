//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.VideoTranslation.ApiSampleCode.PublicPreview;

public enum Mode
{
    None = 0,

    // Combined operations.
    CreateTranslationAndIterationAndWaitUntilTerminated,

    // Atom operations.
    CreateTranslation,

    QueryTranslations,

    QueryTranslation,

    DeleteTranslation,

    CreateIteration,

    QueryIterations,

    QueryIteration,
}