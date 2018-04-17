package Carbon.Recognition;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//


/// <summary>
/// Defines audio input stream.
/// </summary>
public class AudioInputStream
{
    /// <summary>
/// Reads data from audio input stream into the data buffer. The maximal number of bytes to be read is determined by the size of dataBuffer.
/// </summary>
/// <param name="dataBuffer">The byte array to store the read data.</param>
/// <returns>the number of bytes have been read.</returns>
public int Read(byte[] dataBuffer)
{
    throw new UnsupportedOperationException();
}

/// <summary>
/// Closes the audio input stream.
/// </summary>
    public void Close()
    {
        throw new UnsupportedOperationException();
    }
}
