//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 *  Defines channels used to send service properties.
 *  Added in version 1.5.0.
 */
public enum ServicePropertyChannel
{
    /**
     * Uses URI query parameter to pass property settings to service.
     */
    UriQueryParameter(com.microsoft.cognitiveservices.speech.internal.ServicePropertyChannel.UriQueryParameter);

    private ServicePropertyChannel(com.microsoft.cognitiveservices.speech.internal.ServicePropertyChannel id) {
        this.id = id;
    }

    /**
     * Returns the internal value
     *
     * @return the channel value
     */
    public com.microsoft.cognitiveservices.speech.internal.ServicePropertyChannel getValue() { return this.id; }

    private final com.microsoft.cognitiveservices.speech.internal.ServicePropertyChannel id;
}