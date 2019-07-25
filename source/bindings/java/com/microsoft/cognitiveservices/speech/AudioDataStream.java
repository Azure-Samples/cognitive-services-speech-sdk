//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
package com.microsoft.cognitiveservices.speech;

import java.io.Closeable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

import com.microsoft.cognitiveservices.speech.SpeechSynthesisResult;
import com.microsoft.cognitiveservices.speech.util.Contracts;

/**
 * Represents audio data stream used for operating audio data as a stream.
 * Added in version 1.7.0
 */
public final class AudioDataStream implements Closeable
{
    private static ExecutorService s_executorService;

    static {
        s_executorService = Executors.newCachedThreadPool();
    }

    private com.microsoft.cognitiveservices.speech.internal.AudioDataStream streamImpl;

    private com.microsoft.cognitiveservices.speech.PropertyCollection parameters;

    private boolean disposed = false;
    private final Object streamLock = new Object();
    private int activeAsyncActionCounter = 0;

    /**
     * Creates a memory backed AudioDataStream from given speech synthesis result.
     * @param result The speech synthesis result.
     * @return The audio data stream being created.
     */
    public static AudioDataStream fromResult(SpeechSynthesisResult result) {
        Contracts.throwIfNull(result, "result");
        return new AudioDataStream(result);
    }

    /**
     * Get current status of the audio data stream.
     * @return Current status.
     */
    public StreamStatus getStatus() {
        return StreamStatus.values()[this.streamImpl.GetStatus().swigValue()];
    }

    /**
     * Check whether the stream has enough data to be read.
     * @param bytesRequested The requested data size in bytes.
     * @return A bool indicating the result.
     */
    public boolean canReadData(long bytesRequested) {
        return this.streamImpl.CanReadData(bytesRequested);
    }

    /**
     * Check whether the stream has enough data to be read, starting from the specified position.
     * @param pos The position to start with.
     * @param bytesRequested The requested data size in bytes.
     * @return A bool indicating the result.
     */
    public boolean canReadData(long pos, long bytesRequested) {
        return this.streamImpl.CanReadData(pos, bytesRequested);
    }

    /**
     * Reads the audio data specified by making an internal copy of the data.
     * The maximal number of bytes to be read is determined by the size of dataBuffer.
     * If there is no data immediately available, read() blocks until the next data becomes available.
     * @param dataBuffer The audio buffer of which this function will make a copy to.
     * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
     */
    public long readData(byte[] dataBuffer) {
        return this.streamImpl.ReadData(dataBuffer);
    }

    /**
     * Reads the audio data specified by making an internal copy of the data, starting from the specified position.
     * The maximal number of bytes to be read is determined by the size of dataBuffer.
     * If there is no data immediately available, read() blocks until the next data becomes available.
     * @param pos The position to start with.
     * @param dataBuffer The audio buffer of which this function will make a copy to.
     * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
     */
    public long readData(long pos, byte[] dataBuffer) {
        return this.streamImpl.ReadData(pos, dataBuffer);
    }

    /**
     * Save the audio data to a file, synchronously.
     * @param fileName The file name with full path.
     */
    public void saveToWavFile(String fileName) {
        this.streamImpl.SaveToWavFile(fileName);
    }

    /**
     * Save the audio data to a file, asynchronously.
     * @param fileName The file name with full path.
     * @return An asynchronous operation representing the saving.
     */
    public Future<Void> saveToWavFileAsync(String fileName) {
        final AudioDataStream thisStream = this;
        final String finalFileName = fileName;

        return s_executorService.submit(new java.util.concurrent.Callable<Void>() {
            public Void  call() {
                Runnable runnable = new Runnable() { public void run() { streamImpl.SaveToWavFile(finalFileName); }};
                thisStream.doAsyncAction(runnable);
                return null;
            }});
    }

    /**
     * Get current position of the audio data stream.
     * @return Current position.
     */
    public long getPosition() {
        return this.streamImpl.GetPosition();
    }

    /**
     * Set current position of the audio data stream.
     * @param pos Position to be set.
     */
    public void setPosition(long pos) {
        this.streamImpl.SetPosition(pos);
    }

    /**
     * The collection or properties and their values defined for this audio data stream.
     * @return The collection or properties and their values defined for this audio data stream.
     */
    public PropertyCollection getProperties() {
        return this.parameters;
    }

    /**
     * Returns the audio data stream implementation.
     * @return The implementation of the stream.
     */
    public com.microsoft.cognitiveservices.speech.internal.AudioDataStream getImpl() {
        return this.streamImpl;
    }

    /**
     * Dispose of associated resources.
     */
    @Override
    public void close() {
        synchronized (streamLock) {
            if (activeAsyncActionCounter != 0) {
                throw new IllegalStateException("Cannot dispose an audio data stream while async saving to wave file is running. Await async operation to avoid unexpected disposals.");
            }
            dispose(true);
        }
    }

    private AudioDataStream(SpeechSynthesisResult result) {
        this.streamImpl = com.microsoft.cognitiveservices.speech.internal.AudioDataStream.FromResult(result.getResultImpl());
        this.parameters = new PrivatePropertyCollection(this.streamImpl.getProperties());
    }

    private void doAsyncAction(Runnable action) {
        synchronized (streamLock) {
            activeAsyncActionCounter++;
        }
        if (disposed) {
            throw new IllegalStateException(this.getClass().getName());
        }
        try {
            action.run();
        } finally {
            synchronized (streamLock) {
                activeAsyncActionCounter--;
            }
        }
    }

    private void dispose(boolean disposing)
    {
        if (this.disposed) {
            return;
        }

        if (disposing) {
            this.streamImpl.delete();
            this.parameters.close();
            this.disposed = true;
        }
    }

    private class PrivatePropertyCollection extends com.microsoft.cognitiveservices.speech.PropertyCollection {
        public PrivatePropertyCollection(com.microsoft.cognitiveservices.speech.internal.PropertyCollection collection) {
            super(collection);
        }
    }
}
