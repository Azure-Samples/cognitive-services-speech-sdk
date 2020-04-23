package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.SpeechConfig;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
  * An abstract base class that defines callback methods (read() and close()) for custom audio input streams).
  */
public abstract class PullAudioInputStreamCallback
{
    private static final String EMPTY_STRING = "";

    // load the native library.
    static {
        // trigger loading of native library
        try {
            Class.forName(SpeechConfig.class.getName());
        }
        catch (ClassNotFoundException ex) {
            throw new IllegalStateException(ex);
        }
    }

    /**
     * Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
     * If there is no data immediately available, read() blocks until the next data becomes available.
     * Note: The dataBuffer returned by read() should not contain any audio header.
     * @param dataBuffer The byte array to store the read data.
     * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
     */
    public abstract int read(byte[] dataBuffer);

    /**
      * Get property associated to data buffer, such as a timestamp or userId. If the property is not available, an empty string must be returned.
      * Added in version 1.5.0
      * @param id The Property id.
      * @return The String value associated to Property id.
      */
    public String getProperty(PropertyId id) { return EMPTY_STRING; }

    /**
      * Closes the audio input stream.
      * @return
      */
    public abstract void close();

    /*! \cond INTERNAL */

    private PropertyId getPropertyId(int id) {
        PropertyId pArr[] = PropertyId.values();
        for (PropertyId pId: pArr){
            if (id == pId.getValue())
            {
                return pId;
            }
        }
        return null;
    }

    /*! \endcond */
}
