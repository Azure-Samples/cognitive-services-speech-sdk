#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import struct

# Utility function to get the header size of a WAV file
def get_wav_header_size(wav_file):
    with open(wav_file, 'rb') as f:
        # Read RIFF chunk descriptor
        riff, chunk_size, fmt = struct.unpack('<4sI4s', f.read(12))
        if riff != b'RIFF' or fmt != b'WAVE':
            raise ValueError("Invalid WAV file format")

        while True:
            # Read sub-chunk header
            sub_chunk_id, sub_chunk_size = struct.unpack('<4sI', f.read(8))

            if sub_chunk_id == b'data':
                # Return the current file position as header size
                return f.tell()

            # Skip the sub-chunk data
            f.seek(sub_chunk_size, 1)