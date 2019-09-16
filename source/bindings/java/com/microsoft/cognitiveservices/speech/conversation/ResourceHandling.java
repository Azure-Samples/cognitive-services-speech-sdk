//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech.conversation;

/**
 * Defines how the service handles allocated resources after the conversation transcriber is stopped.
 * Added in version 1.7.0
 */
public enum ResourceHandling
{
    /**
     * The service destroys allocated resources, such as adapted language models.
     */
    DestroyResources(com.microsoft.cognitiveservices.speech.internal.ResourceHandling.DestroyResources),

    /**
     * The service keeps allocated resources.
     */
    KeepResources(com.microsoft.cognitiveservices.speech.internal.ResourceHandling.KeepResources);

    private ResourceHandling(com.microsoft.cognitiveservices.speech.internal.ResourceHandling id) {
        this.id = id;
    }

    /**
     * Returns the internal ResourceHandling value
     *
     * @return the ResourceHandling value
     */
    public com.microsoft.cognitiveservices.speech.internal.ResourceHandling getValue() {
        return this.id;
    }

    private final com.microsoft.cognitiveservices.speech.internal.ResourceHandling id;

}
