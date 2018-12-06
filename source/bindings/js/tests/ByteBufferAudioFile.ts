// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

export class ByteBufferAudioFile {
    public static Load(buffer: ArrayBuffer): File {

        const parts: ArrayBuffer[] = [buffer];
        const file: File = new File(parts, "file.wav");

        return file;
    }
}
