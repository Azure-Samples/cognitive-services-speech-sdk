//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.azure.core.annotation.ServiceClient;
import com.azure.core.exception.HttpRequestException;
import com.azure.core.http.*;
import com.azure.core.http.rest.Response;
import com.azure.core.implementation.RestProxy;
import com.azure.core.util.polling.LongRunningOperationStatus;
import com.azure.core.util.polling.PollResponse;
import com.azure.core.util.polling.PollerFlux;
import com.azure.core.util.polling.PollingContext;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import reactor.core.publisher.Mono;

import java.time.Duration;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Function;
import static com.azure.core.implementation.util.FluxUtil.withContext;

@ServiceClient(builder = RemoteConversationTranscriptionClientBuilder.class, isAsync = true, serviceInterfaces = RemoteConversationTranscriptionService.class)
class RemoteConversationTranscriptionClientImpl {

    private final RemoteConversationTranscriptionService service;
    private final String hostNameSuffix = ".cts.speech.microsoft.com/api/v1";
    private final String hostNamePrefix = "https://transcribe.";
    private String subscriptionKey = null;
    private String speechEndpoint = null;
    private String authToken = null;
    private boolean useToken = false;
    private Map<String, RemoteConversationTranscriptionResult> localResultCollection = new HashMap<String, RemoteConversationTranscriptionResult>();

    RemoteConversationTranscriptionClientImpl(HttpPipeline pipeline, SpeechConfig config) {
        String key = config.getProperty(PropertyId.SpeechServiceConnection_Key);
        String region = config.getProperty(PropertyId.SpeechServiceConnection_Region);
        String token = config.getProperty(PropertyId.SpeechServiceAuthorization_Token);
        String endpoint = config.getProperty(PropertyId.SpeechServiceConnection_Endpoint);

        if(endpoint != null && !endpoint.isEmpty()) {
            speechEndpoint = endpoint;
        }else {
            Contracts.throwIfNullOrWhitespace(region, "Must have a valid speech region or endpoint");
            speechEndpoint = hostNamePrefix + region + hostNameSuffix;
        }

        if((token == null || token.isEmpty()) && (key == null || key.isEmpty())) {
            Contracts.throwIfNull(null, "Must have a valid authorization token or subscription key");
        }

        if(token != null && !token.isEmpty()) {
            authToken = token;
            useToken = true;
        }else {
            subscriptionKey = key;
        }

        this.service = RestProxy.create(RemoteConversationTranscriptionService.class, pipeline);
    }

    Mono<Integer> deleteRemoteTranscriptionAsyncImpl(String id) {
        if(useToken) {
            return withContext(context ->  service.deleteRemoteConversationTranscriptionOperationAuthToken(speechEndpoint,
                    id,
                    authToken,
                    "application/json",
                    context)
                    .flatMap(this::processDeleteOperationResponse))
                    .doOnError(e -> {
                        System.out.println(e.getMessage());
                    });
        }
        else {
            return withContext(context ->  service.deleteRemoteConversationTranscriptionOperationKey(speechEndpoint,
                    id,
                    subscriptionKey,
                    "application/json",
                    context)
                    .flatMap(this::processDeleteOperationResponse))
                    .doOnError(e -> {
                        System.out.println(e.getMessage());
                    });
        }
    }

    private Mono<Integer> processDeleteOperationResponse(Response<RemoteDeleteOperationResultModel> remoteOperationResponse) {
        return Mono.just(remoteOperationResponse.getStatusCode());
    }

    Function<PollingContext<RemoteConversationTranscriptionOperation>, Mono<PollResponse<RemoteConversationTranscriptionOperation>>> createPollOperation(String id) {
        return prePollResponse -> {
            try {
                if(useToken) {
                    return withContext(context ->  service.getRemoteConversationTranscriptionOperationAuthToken(speechEndpoint,
                            id,
                            authToken,
                            "application/json",
                            context)
                            .flatMap(this::processRemoteOperationResponse))
                            .onErrorResume((e) -> {
                                RemoteConversationTranscriptionOperation result = new RemoteConversationTranscriptionOperation(id);
                                result.setError(e.getMessage());
                                return Mono.just(new PollResponse<>(LongRunningOperationStatus.FAILED, result));
                            });
                }else {
                    return withContext(context ->  service.getRemoteConversationTranscriptionOperationKey(speechEndpoint,
                            id,
                            subscriptionKey,
                            "application/json",
                            context)
                            .flatMap(this::processRemoteOperationResponse))
                            .onErrorResume((e) -> {
                                RemoteConversationTranscriptionOperation result = new RemoteConversationTranscriptionOperation(id);
                                result.setError(e.getMessage());
                                return Mono.just(new PollResponse<>(LongRunningOperationStatus.FAILED, result));
                            });
                }
            } catch (HttpRequestException e) {
                RemoteConversationTranscriptionOperation result = new RemoteConversationTranscriptionOperation(id);
                result.setError(e.getMessage());
                return Mono.just(new PollResponse<>(LongRunningOperationStatus.FAILED, result));
            }
        };
    }

    private Mono<PollResponse<RemoteConversationTranscriptionOperation>> processRemoteOperationResponse(Response<RemoteConversationTranscriptionModel> remoteOperationResponse) {
        LongRunningOperationStatus status;
        switch(remoteOperationResponse.getValue().processingStatus)
        {
            case Processing:
                status = LongRunningOperationStatus.IN_PROGRESS;
                break;
            case Uploading:
                status = LongRunningOperationStatus.NOT_STARTED;
                break;
            case Completed:
                updateTranscriptionResult(remoteOperationResponse);
                status = LongRunningOperationStatus.SUCCESSFULLY_COMPLETED;
                break;
            case Deleted:
                status = LongRunningOperationStatus.SUCCESSFULLY_COMPLETED;
                break;
            default:
                status = LongRunningOperationStatus.FAILED;
        }

        RemoteConversationTranscriptionOperation operation = new RemoteConversationTranscriptionOperation(remoteOperationResponse.getValue().id);
        operation.setStatus(remoteOperationResponse.getValue().processingStatus.toString());

        return Mono.just(new PollResponse<>(status, operation));
    }

    private void updateTranscriptionResult(Response<RemoteConversationTranscriptionModel> remoteOperationResponse) {
        ObjectMapper objectMapper = new ObjectMapper();
        RemoteConversationTranscriptionResult result = new RemoteConversationTranscriptionResult();
        result.id = remoteOperationResponse.getValue().id;

        if (remoteOperationResponse.getValue().transcriptions != null) {
            for (int i = 0; i < remoteOperationResponse.getValue().transcriptions.size(); i++) {
                RemoteConversationTranscriptionResultImpl resImpl = new RemoteConversationTranscriptionResultImpl();
                RemoteTranscriptionModel tran = remoteOperationResponse.getValue().transcriptions.get(i);

                RemoteConversationTranscriptionPropertyCollectionImpl collection = new RemoteConversationTranscriptionPropertyCollectionImpl();

                try {
                    String json = objectMapper.writeValueAsString(tran);
                    collection.setProperty(PropertyId.SpeechServiceResponse_JsonResult, json);
                    collection.setProperty("RESULT-Json", json);
                } catch(JsonProcessingException e) {
                    collection.setProperty(PropertyId.SpeechServiceResponse_JsonErrorDetails, e.getMessage());
                    collection.setProperty("RESULT-ErrorDetails",e.getMessage());
                }

                resImpl.SetAllFields(tran.userId,
                        tran.resultId,
                        tran.reason,
                        tran.duration,
                        tran.offsetInTicks,
                        tran.text,
                        collection);
                result.transcriptions.add(resImpl);
            }
        }
        localResultCollection.put(remoteOperationResponse.getValue().id, result);
    }

    RemoteConversationTranscriptionResult getFinalResult(String id) {
        if(localResultCollection.size() > 0 && localResultCollection.containsKey(id)) {
            return localResultCollection.get(id);
        }
        else {
            return null;
        }
    }
}

