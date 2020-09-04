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
import com.microsoft.cognitiveservices.speech.KeywordRecognitionResult;
import com.microsoft.cognitiveservices.speech.util.Contracts;
import com.microsoft.cognitiveservices.speech.util.IntRef;
import com.microsoft.cognitiveservices.speech.util.SafeHandle;
import com.microsoft.cognitiveservices.speech.util.SafeHandleType;

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

    private SafeHandle streamHandle = null;

    private com.microsoft.cognitiveservices.speech.PropertyCollection propertyHandle;

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

        IntRef streamRef = new IntRef(0);
        Contracts.throwIfFail(createFromResult(streamRef, result.getImpl()));
        return new AudioDataStream(streamRef);
    }

    /**
     * Obtains the memory backed AudioDataStream associated with a given KeywordRecognition result.
     * @param result The keyword recognition result.
     * @return An audio stream with the input to the KeywordRecognizer starting from right before the Keyword.
     */
    public static AudioDataStream fromResult(KeywordRecognitionResult result) {
        Contracts.throwIfNull(result, "result");

        IntRef streamRef = new IntRef(0);
        Contracts.throwIfFail(createFromKeywordResult(streamRef, result.getImpl()));
        return new AudioDataStream(streamRef);
    }

    /**
     * Get current status of the audio data stream.
     * @return Current status.
     */
    public StreamStatus getStatus() {
        IntRef statusRef = new IntRef(0);
        Contracts.throwIfFail(getStatus(streamHandle, statusRef));
        return StreamStatus.values()[(int)statusRef.getValue()];
    }

    /**
     * Check whether the stream has enough data to be read.
     * @param bytesRequested The requested data size in bytes.
     * @return A bool indicating the result.
     */
    public boolean canReadData(long bytesRequested) {
        return canReadData(streamHandle, bytesRequested);
    }

    /**
     * Check whether the stream has enough data to be read, starting from the specified position.
     * @param pos The position to start with.
     * @param bytesRequested The requested data size in bytes.
     * @return A bool indicating the result.
     */
    public boolean canReadData(long pos, long bytesRequested) {
        if (pos < 0 ) {
            throw new IndexOutOfBoundsException();
        }

        return canReadData(streamHandle, pos, bytesRequested);
    }

    /**
     * Reads the audio data specified by making an internal copy of the data.
     * The maximal number of bytes to be read is determined by the size of dataBuffer.
     * If there is no data immediately available, read() blocks until the next data becomes available.
     * @param dataBuffer The audio buffer of which this function will make a copy to.
     * @return The number of bytes filled, or 0 in case the stream hits its end and there is no more data available.
     */
    public long readData(byte[] dataBuffer) {
        IntRef filledSizeRef = new IntRef(0);
        Contracts.throwIfFail(readData(streamHandle, dataBuffer, filledSizeRef));
        return filledSizeRef.getValue();
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
        if (pos < 0) {
            throw new IndexOutOfBoundsException();
        }

        IntRef filledSizeRef = new IntRef(0);
        Contracts.throwIfFail(readData(streamHandle, dataBuffer, pos, filledSizeRef));
        return filledSizeRef.getValue();
    }

    /**
     * Save the audio data to a file, synchronously.
     * @param fileName The file name with full path.
     */
    public void saveToWavFile(String fileName) {
        Contracts.throwIfFail(saveToWaveFile(streamHandle, fileName));
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
                Runnable runnable = new Runnable() { public void run() { Contracts.throwIfFail(saveToWaveFile(thisStream.streamHandle, finalFileName)); }};
                thisStream.doAsyncAction(runnable);
                return null;
            }});
    }

    /**
     * Get current position of the audio data stream.
     * @return Current position.
     */
    public long getPosition() {
        IntRef posRef = new IntRef(0);
        Contracts.throwIfFail(getPosition(streamHandle, posRef));
        return posRef.getValue();
    }

    /**
     * Set current position of the audio data stream.
     * @param pos Position to be set.
     */
    public void setPosition(long pos) {
        if (pos < 0) {
            throw new IndexOutOfBoundsException();
        }

        Contracts.throwIfFail(setPosition(streamHandle, pos));
    }

    /**
     * Stops any more data from getting to the stream.
     */
    public void detachInput() {
        Contracts.throwIfFail(detachInput(streamHandle));
    }

    /**
     * The collection of properties and their values defined for this audio data stream.
     * @return The collection of properties and their values defined for this audio data stream.
     */
    public PropertyCollection getProperties() {
        return this.propertyHandle;
    }

    /*! \cond INTERNAL */

    /**
     * Returns the audio data stream implementation.
     * @return The implementation of the stream.
     */
    public SafeHandle getImpl() {
        return this.streamHandle;
    }

    /*! \endcond */

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

    private AudioDataStream(IntRef stream) {
        Contracts.throwIfNull(stream, "stream");

        streamHandle = new SafeHandle(stream.getValue(), SafeHandleType.AudioDataStream);
        IntRef propertyHandleRef = new IntRef(0);
        Contracts.throwIfFail(getPropertyBagFromStreamHandle(streamHandle, propertyHandleRef));
        propertyHandle = new PropertyCollection(propertyHandleRef);
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
            if (streamHandle != null) {
                streamHandle.close();
                streamHandle = null;
            }
            if (propertyHandle != null) {
                propertyHandle.close();
                propertyHandle = null;
            }
            this.disposed = true;
        }
    }

    private final static native long createFromResult(IntRef streamRef, SafeHandle resultHandle);
    private final static native long createFromKeywordResult(IntRef streamRef, SafeHandle resultHandle);
    private final native long getStatus(SafeHandle streamHandle, IntRef statusRef);
    private final native boolean canReadData(SafeHandle streamHandle, long bytesRequested);
    private final native boolean canReadData(SafeHandle streamHandle, long pos, long bytesRequested);
    private final native long readData(SafeHandle streamHandle, byte[] dataBuffer, IntRef filledSizeRef);
    private final native long readData(SafeHandle streamHandle, byte[] dataBuffer, long pos, IntRef filledSizeRef);
    private final native long saveToWaveFile(SafeHandle streamHandle, String fileName);
    private final native long getPosition(SafeHandle streamHandle, IntRef posRef);
    private final native long setPosition(SafeHandle streamHandle, long pos);
    private final native long detachInput(SafeHandle streamHandle);
    private final native long getPropertyBagFromStreamHandle(SafeHandle streamHandle, IntRef propertyHandleRef);
}
