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
    UriQueryParameter(0),

    /**
    * Uses HttpHeader to set a key/value in a HTTP header.
    */
    HttpHeader(1);


    private ServicePropertyChannel(int id) {
        this.id = id;
    }

    /**
     * Returns the internal value
     *
     * @return the channel value
     */
    public int getValue() { return this.id; }

    private final int id;
}
