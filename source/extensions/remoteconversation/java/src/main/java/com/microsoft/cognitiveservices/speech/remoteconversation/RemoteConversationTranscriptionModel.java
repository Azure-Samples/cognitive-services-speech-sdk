//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import java.util.List;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonPropertyOrder({
        "id",
        "transcriptions",
        "processingStatus"
})
class RemoteConversationTranscriptionModel {

    @JsonProperty("id")
    String id;
    @JsonProperty("transcriptions")
    List<RemoteTranscriptionModel> transcriptions = null;
    @JsonProperty("processingStatus")
    RemoteTaskStatusModel processingStatus;
}