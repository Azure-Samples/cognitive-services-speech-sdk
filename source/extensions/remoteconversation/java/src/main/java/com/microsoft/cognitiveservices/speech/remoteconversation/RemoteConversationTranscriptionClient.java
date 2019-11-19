//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.azure.core.util.polling.PollResponse;
import com.azure.core.util.polling.PollerFlux;
import com.azure.core.util.polling.PollingContext;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import reactor.core.publisher.Mono;

/**
 * Performs async conversation transcription, and gets transcribed text and user id as a result.
 * Added in remote-conversation version 1.8.0
 */

import java.time.Duration;
import java.util.function.BiFunction;
import java.util.function.Function;

public class RemoteConversationTranscriptionClient {
    /**
     * Create a new instance of Remote Conversation Transcription Client.
     * @param speechConfig speech configuration.
     */
    public RemoteConversationTranscriptionClient(SpeechConfig config) {
        RemoteConversationTranscriptionClientBuilder builder = new RemoteConversationTranscriptionClientBuilder();
        clientImpl = builder.buildAsyncClient(config);
    }

    /**
     * Get the Remote Conversation Transcription Operation for the id with default polling duration of one seconds.
     * @param id conversation Id for the transcription.
     * @return a PollerFlux of RemoteConversationTranscriptionOperation.
     */
    public PollerFlux<RemoteConversationTranscriptionOperation, RemoteConversationTranscriptionResult> getTranscriptionOperation(String id) {
        return new PollerFlux<RemoteConversationTranscriptionOperation, RemoteConversationTranscriptionResult>(Duration.ofSeconds(1),
                activationOperation(),
                pollOperation(id),
                cancelOperation(),
                getFinalResultOperation(id));
    }

    /**
     * Get the Remote Conversation Transcription Operation for the id.
     * @param id conversation Id for the transcription.
     * @param pollingIntervalInSeconds polling duration in seconds.
     * @return a PollerFlux of RemoteConversationTranscriptionOperation.
     */
    public PollerFlux<RemoteConversationTranscriptionOperation, RemoteConversationTranscriptionResult> getTranscriptionOperation(String id, int pollingIntervalInSeconds) {
        return new PollerFlux<RemoteConversationTranscriptionOperation, RemoteConversationTranscriptionResult>(Duration.ofSeconds(pollingIntervalInSeconds),
                activationOperation(),
                pollOperation(id),
                cancelOperation(),
                getFinalResultOperation(id));
    }

    /**
     * Delete the transcription associated with the conversationId.
     * @param conversationId id for the conversation transcription.
     * @return a Mono of the service status code.
     */
    public Mono<Integer> deleteRemoteTranscriptionAsync(String conversationId) {
        return clientImpl.deleteRemoteTranscriptionAsyncImpl(conversationId);
    }

    private Function<PollingContext<RemoteConversationTranscriptionOperation>, Mono<RemoteConversationTranscriptionOperation>> activationOperation() {
        return pollingContext -> {
            return Mono.empty();
        };
    }

    private Function<PollingContext<RemoteConversationTranscriptionOperation>, Mono<PollResponse<RemoteConversationTranscriptionOperation>>> pollOperation(String id) {
        return clientImpl.createPollOperation(id);
    }

    private Function<PollingContext<RemoteConversationTranscriptionOperation>, Mono<RemoteConversationTranscriptionResult>> getFinalResultOperation(String id) {
        return pollingContext -> {
            RemoteConversationTranscriptionResult result = clientImpl.getFinalResult(id);
            if(result != null) {
                return Mono.just(result);
            }
            else {
                return Mono.empty();
            }
        };
    }

    private BiFunction<PollingContext<RemoteConversationTranscriptionOperation>, PollResponse<RemoteConversationTranscriptionOperation>, Mono<RemoteConversationTranscriptionOperation>> cancelOperation() {
        return (pollingContext, response) -> {
            return Mono.empty();
        };
    }

    private RemoteConversationTranscriptionClientImpl clientImpl;

}
