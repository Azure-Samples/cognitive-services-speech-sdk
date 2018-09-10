//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

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
}
