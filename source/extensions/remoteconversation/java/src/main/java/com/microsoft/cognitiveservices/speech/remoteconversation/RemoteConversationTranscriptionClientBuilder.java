//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.azure.core.annotation.ServiceClientBuilder;
import com.azure.core.http.HttpPipeline;
import com.azure.core.http.HttpPipelineBuilder;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

@ServiceClientBuilder(serviceClients = {RemoteConversationTranscriptionClientImpl.class})
final class RemoteConversationTranscriptionClientBuilder {

    RemoteConversationTranscriptionClientBuilder() {
    }

    RemoteConversationTranscriptionClientImpl buildAsyncClient(SpeechConfig config) {
        HttpPipeline pipeline = new HttpPipelineBuilder()
                .build();

        return new RemoteConversationTranscriptionClientImpl(pipeline, config);
    }
}
