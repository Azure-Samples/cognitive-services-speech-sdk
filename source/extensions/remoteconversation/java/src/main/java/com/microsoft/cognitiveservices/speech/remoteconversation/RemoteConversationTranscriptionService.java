//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.azure.core.annotation.*;
import com.azure.core.exception.*;
import com.azure.core.http.rest.Response;
import com.azure.core.util.Context;
import reactor.core.publisher.Mono;

@Host("{url}")
@ServiceInterface(name = "Speech")
interface RemoteConversationTranscriptionService {

    @Get("offlinetask")
    @ExpectedResponses({200})
    @UnexpectedResponseExceptionType(code = {400}, value = HttpResponseException.class)
    @UnexpectedResponseExceptionType(code = {404}, value = ResourceNotFoundException.class)
    @UnexpectedResponseExceptionType(code = {401}, value = ClientAuthenticationException.class)
    @UnexpectedResponseExceptionType(HttpResponseException.class)
    Mono<Response<RemoteConversationTranscriptionModel>> getRemoteConversationTranscriptionOperationKey(@HostParam("url") String url,
                                                                                                     @QueryParam("id") String id,
                                                                                                     @HeaderParam("Ocp-Apim-Subscription-Key") String key,
                                                                                                     @HeaderParam("accept") String type,
                                                                                                     Context context);

    @Get("offlinetask")
    @ExpectedResponses({200})
    @UnexpectedResponseExceptionType(code = {400}, value = HttpResponseException.class)
    @UnexpectedResponseExceptionType(code = {404}, value = ResourceNotFoundException.class)
    @UnexpectedResponseExceptionType(code = {401}, value = ClientAuthenticationException.class)
    @UnexpectedResponseExceptionType(HttpResponseException.class)
    Mono<Response<RemoteConversationTranscriptionModel>> getRemoteConversationTranscriptionOperationAuthToken(@HostParam("url") String url,
                                                                                                     @QueryParam("id") String id,
                                                                                                     @HeaderParam("Authorization") String token,
                                                                                                     @HeaderParam("accept") String type,
                                                                                                     Context context);

    @Delete("offlinetask")
    @ExpectedResponses({200})
    @UnexpectedResponseExceptionType(code = {400}, value = HttpResponseException.class)
    @UnexpectedResponseExceptionType(code = {404}, value = ResourceNotFoundException.class)
    @UnexpectedResponseExceptionType(code = {401}, value = ClientAuthenticationException.class)
    @UnexpectedResponseExceptionType(HttpResponseException.class)
    Mono<Response<RemoteDeleteOperationResultModel>> deleteRemoteConversationTranscriptionOperationKey(@HostParam("url") String url,
                                                                                                    @QueryParam("id") String id,
                                                                                                    @HeaderParam("Ocp-Apim-Subscription-Key") String key,
                                                                                                    @HeaderParam("accept") String type,
                                                                                                    Context context);

    @Delete("offlinetask")
    @ExpectedResponses({200})
    @UnexpectedResponseExceptionType(code = {400}, value = HttpResponseException.class)
    @UnexpectedResponseExceptionType(code = {404}, value = ResourceNotFoundException.class)
    @UnexpectedResponseExceptionType(code = {401}, value = ClientAuthenticationException.class)
    @UnexpectedResponseExceptionType(HttpResponseException.class)
    Mono<Response<RemoteDeleteOperationResultModel>> deleteRemoteConversationTranscriptionOperationAuthToken(@HostParam("url") String url,
                                                                                                    @QueryParam("id") String id,
                                                                                                    @HeaderParam("Authorization") String token,
                                                                                                    @HeaderParam("accept") String type,
                                                                                                    Context context);

}
