// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
import * as sdk from "../../../microsoft.cognitiveservices.speech.sdk";

export class ByteBufferAudioFile {
    public static Load(buffer: ArrayBuffer): File {

        const parts: ArrayBuffer[] = [buffer];
        const file: File = new File(parts, "file.wav");

        return file;
    }
}
