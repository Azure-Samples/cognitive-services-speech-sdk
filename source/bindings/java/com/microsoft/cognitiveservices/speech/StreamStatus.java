//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

/**
 * Defines the possible status of audio data stream.
 * Added in version 1.7.0
 */
public enum StreamStatus
{
    /**
     * The audio data stream status is unknown
     */
    Unknown,

    /**
     * The audio data stream contains no data
     */
    NoData,

    /**
     * The audio data stream contains partial data of a speak request
     */
    PartialData,

    /**
     * The audio data stream contains all data of a speak request
     */
    AllData,

    /**
     * The audio data stream was cancelled
     */
    Canceled
}
