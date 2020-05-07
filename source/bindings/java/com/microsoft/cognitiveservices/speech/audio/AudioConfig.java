package com.microsoft.cognitiveservices.speech.audio;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;

import java.io.Closeable;
import java.io.IOException;

import com.microsoft.cognitiveservices.speech.PropertyCollection;
import com.microsoft.cognitiveservices.speech.PropertyId;
import com.microsoft.cognitiveservices.speech.SpeechConfig;

/**
 * Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
 * Updated in version 1.7.0
 */
public final class AudioConfig implements Closeable {

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
     * Creates an AudioConfig object representing the default microphone on the system.
     * @return The audio input configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromDefaultMicrophoneInput() {
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioInputFromDefaultMicrophone(configHandle));
        return new AudioConfig(configHandle);
    }

    /**
     * Creates an AudioConfig object representing a specific microphone on the system.
     * @param deviceName Specifies the platform-specific id of the audio input device.
     *     Please refer to the <a href="https://aka.ms/csspeech/microphone-selection">the online documentation</a> about how to retrieve that id on different platforms.
     * This functionality was added in version 1.3.0.
     * @return The audio input configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromMicrophoneInput(String deviceName) {
        Contracts.throwIfNull(deviceName, "deviceName");
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioInputFromMicrophoneInput(configHandle, deviceName));
        return new AudioConfig(configHandle);
    }

    /**
     * Creates an AudioConfig object representing the specified file.
     * @param fileName Specifies the audio input file. 
     * @return The audio input configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromWavFileInput(String fileName) {
        Contracts.throwIfNull(fileName, "fileName");
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioInputFromWavFileName(configHandle, fileName));
        return new AudioConfig(configHandle);
    }

    /**
     * Creates an AudioConfig object representing the specified stream.
     * @param audioStream Specifies the custom audio input stream. 
     * @return The audio input configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromStreamInput(AudioInputStream audioStream) {
        Contracts.throwIfNull(audioStream, "audioStream");
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioInputFromStream(configHandle, audioStream.getImpl()));        
        return new AudioConfig(configHandle, audioStream);
    }

    /**
     * Creates an AudioConfig object representing the specified stream.
     * @param callback Specifies the pull audio input stream callback. 
     * @return The audio input configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromStreamInput(PullAudioInputStreamCallback callback) {
        Contracts.throwIfNull(callback, "callback");
        PullAudioInputStream pullStream = PullAudioInputStream.create(callback);
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioInputFromStream(configHandle, pullStream.getImpl()));
        AudioConfig config = new AudioConfig(configHandle, pullStream);
        config.closeKeepAliveOnClose = true;
        return config;
    }

    /**
     * Creates an AudioConfig object representing the default speaker on the system.
     * Added in version 1.7.0
     * @return The audio output configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromDefaultSpeakerOutput() {
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioOutputFromDefaultSpeaker(configHandle));
        return new AudioConfig(configHandle);
    }

    /**
     * Creates an AudioConfig object representing the specified file.
     * Added in version 1.7.0
     * @param fileName Specifies the audio output file. The parent directory must already exist.
     * @return The audio output configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromWavFileOutput(String fileName) {
        Contracts.throwIfNull(fileName, "fileName");
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioOutputFromWavFileName(configHandle, fileName));
        return new AudioConfig(configHandle);
    }

    /**
     * Creates an AudioConfig object representing the specified stream.
     * Added in version 1.7.0
     * @param audioStream Specifies the custom audio output stream.
     * @return The audio output configuration being created.
     */
    public static com.microsoft.cognitiveservices.speech.audio.AudioConfig fromStreamOutput(AudioOutputStream audioStream) {
        IntRef configHandle = new IntRef(0);
        Contracts.throwIfFail(createAudioOutputFromStream(configHandle, audioStream.streamHandle));
        return new AudioConfig(configHandle, audioStream);
    }

    /**
     * Explicitly frees any external resource attached to the object
     */
    @Override
    public void close() {
        if(this.inputStreamKeepAlive != null && this.closeKeepAliveOnClose) {
            this.inputStreamKeepAlive.close();
        }
        this.inputStreamKeepAlive = null;

        if (propertyHandle != null)
        {
            propertyHandle.close();
            propertyHandle = null;
        }
        if (this.audioConfigHandle != null) {
            this.audioConfigHandle.close();
            this.audioConfigHandle = null;
        }
    }

    /**
     * Sets a named property as value.
     * @param name the name of the property.
     * @param value the value.
     */
    public void setProperty(String name, String value) {
        Contracts.throwIfNullOrWhitespace(name, "name");
        Contracts.throwIfNullOrWhitespace(value, "value");

        propertyHandle.setProperty(name, value);
    }

    /**
     * Sets the property by propertyId.
     * @param id PropertyId of the property.
     * @param value The value.
     */
    public void setProperty(PropertyId id, String value) {
        Contracts.throwIfNullOrWhitespace(value, "value");

        propertyHandle.setProperty(id, value);
    }

    AudioConfig(IntRef config) {
        Contracts.throwIfNull(config, "config");
        this.audioConfigHandle = new SafeHandle(config.getValue(), SafeHandleType.AudioConfig);
        this.inputStreamKeepAlive = null;
        this.outputStreamKeepAlive = null;

        IntRef propHandle = new IntRef(0);
        Contracts.throwIfFail(getPropertyBag(audioConfigHandle, propHandle));
        this.propertyHandle = new PropertyCollection(propHandle);
    }

    AudioConfig(IntRef config, AudioInputStream audioStream) {
        this(config);
        this.inputStreamKeepAlive = audioStream;
    }

    AudioConfig(IntRef config, AudioOutputStream audioStream) {
        this(config);
        this.outputStreamKeepAlive = audioStream;
    }

    /*! \cond INTERNAL */

    /**
     * Returns the audio input configuration.
     * @return The implementation of the config.
     */
    public SafeHandle getImpl() {
        return this.audioConfigHandle;
    }
    
    /*! \endcond */

    private SafeHandle audioConfigHandle = null;
    private AudioInputStream inputStreamKeepAlive = null;
    private AudioOutputStream outputStreamKeepAlive = null;
    private boolean closeKeepAliveOnClose = false;
    private PropertyCollection propertyHandle = null;

    private final static native long createAudioInputFromWavFileName(IntRef audioConfigHandle, String fileName);
    private final static native long createAudioInputFromDefaultMicrophone(IntRef configHandle);
    private final static native long createAudioInputFromMicrophoneInput(IntRef configHandle, String deviceName);
    private final static native long createAudioInputFromStream(IntRef configHandle, SafeHandle streamHandle);
    private final static native long createAudioOutputFromDefaultSpeaker(IntRef configHandle);
    private final static native long createAudioOutputFromWavFileName(IntRef configHandle, String fileName);
    private final static native long createAudioOutputFromStream(IntRef configHandle, SafeHandle streamHandle);
    private final native long getPropertyBag(SafeHandle configHandle, IntRef propHandle);

}
