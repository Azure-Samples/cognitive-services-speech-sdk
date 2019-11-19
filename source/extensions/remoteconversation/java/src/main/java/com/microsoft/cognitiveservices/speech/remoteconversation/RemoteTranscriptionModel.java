//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.remoteconversation;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.microsoft.cognitiveservices.speech.ResultReason;
import java.math.BigInteger;

@JsonInclude(JsonInclude.Include.NON_NULL)
@JsonPropertyOrder({
        "userId",
        "resultId",
        "reason",
        "text",
        "duration",
        "offsetInTicks"
})

class RemoteTranscriptionModel {
    @JsonProperty("userId")
    String userId;
    @JsonProperty("resultId")
    String resultId;
    @JsonProperty("reason")
    ResultReason reason;
    @JsonProperty("text")
    String text;
    @JsonProperty("durationInTicks")
    BigInteger duration;
    @JsonProperty("offsetInTicks")
    BigInteger offsetInTicks;
}