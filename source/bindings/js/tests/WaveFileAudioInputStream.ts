// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import * as fs from "fs";

export class WaveFileAudioInput {

    public static LoadFile(filename: string): File {
        // obtain and open the line.
        const fileContents: Buffer = fs.readFileSync(filename);

        const arrayBuffer: ArrayBuffer = Uint8Array.from(fileContents).buffer;
        const parts: ArrayBuffer[] = [arrayBuffer];
        const file: File = new File(parts, filename);

        return (file);
    }

    public static LoadArrayFromFile(filename: string): ArrayBuffer {
        const fileContents: Buffer = fs.readFileSync(filename);

        const ret = Uint8Array.from(fileContents);

        return ret.buffer;
    }
}
