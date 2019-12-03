package com.microsoft.cognitiveservices.speech.samples.console;

import com.microsoft.cognitiveservices.speech.audio.PushAudioOutputStreamCallback;

public class PushAudioOutputStreamSampleCallback extends PushAudioOutputStreamCallback {
    public PushAudioOutputStreamSampleCallback() {
        this.audioData = new byte[0];
    }

    @Override
    public int write(byte[] dataBuffer)
    {
        byte[] updatedAudioData = new byte[audioData.length + dataBuffer.length];
        System.arraycopy(audioData, 0, updatedAudioData, 0, audioData.length);
        System.arraycopy(dataBuffer, 0, updatedAudioData, audioData.length, dataBuffer.length);

        audioData = updatedAudioData;
        
        System.out.println(dataBuffer.length + " bytes received.");
        
        return dataBuffer.length;
    }

    @Override
    public void close()
    {
        System.out.println("Push audio output stream closed.");
    }

    public byte[] getAudioData()
    {
        return audioData;
    }

    private byte[] audioData;
}
