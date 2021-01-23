//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    public enum WebHookEventKind
    {
        DatasetCreation,
        DatasetProcessing,
        DatasetCompletion,
        DatasetDeletion,

        ModelCreation,
        ModelProcessing,
        ModelCompletion,
        ModelDeletion,

        EvaluationCreation,
        EvaluationProcessing,
        EvaluationCompletion,
        EvaluationDeletion,

        TranscriptionCreation,
        TranscriptionProcessing,
        TranscriptionCompletion,
        TranscriptionDeletion,

        EndpointCreation,
        EndpointProcessing,
        EndpointCompletion,
        EndpointDeletion,

        Ping,
        Challenge
    }
}
