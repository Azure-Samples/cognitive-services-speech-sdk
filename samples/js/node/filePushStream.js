// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// pull in the required packages.
import * as sdk from "microsoft-cognitiveservices-speech-sdk";
import * as fs from "fs";

function ReadInt32(fd) {
    var buffer = Buffer.alloc(4);
    var bytesRead = fs.readSync(fd, buffer);
    if (4 != bytesRead) {
        throw "Error reading 32-bit integer from .wav file header. Expected 4 bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.readInt32LE();
}

function ReadUInt16(fd) {
    var buffer = Buffer.alloc(2);
    var bytesRead = fs.readSync(fd, buffer);
    if (2 != bytesRead) {
        throw "Error reading 16-bit unsigned integer from .wav file header. Expected 2 bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.readUInt16LE();
}

function ReadUInt32(fd) {
    var buffer = Buffer.alloc(4);
    var bytesRead = fs.readSync(fd, buffer);
    if (4 != bytesRead) {
        throw "Error reading unsigned 32-bit integer from .wav file header. Expected 4 bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.readUInt32LE();
}

function ReadString(fd, length) {
    var buffer = Buffer.alloc(length);
    var bytesRead = fs.readSync(fd, buffer);
    if (length != bytesRead) {
        throw "Error reading string from .wav file header. Expected " + String(length) + " bytes. Actual bytes read: " + String(bytesRead);
    }
    return buffer.toString();
}

export const openPushStream = (filename) => {

    // Get the wave header for the file.
    var wavFileHeader = readWavFileHeader(filename);

    var format;

    switch (wavFileHeader.tag)
    {
        case 1: // PCM
            format = sdk.AudioFormatTag.PCM;
            break;
        case 6: 
            format = sdk.AudioFormatTag.ALaw;
            break;
        case 7:
            format = sdk.AudioFormatTag.MuLaw;
            break;
        default:
            throw new Error("Wave format " + wavFileHeader.tag + " is not supported");
    }

    // Create the format for PCM Audio.
    var format = sdk.AudioStreamFormat.getWaveFormat(wavFileHeader.framerate, wavFileHeader.bitsPerSample, wavFileHeader.nChannels, format);

    // create the push stream we need for the speech sdk.
    var pushStream = sdk.AudioInputStream.createPushStream(format);

    // open the file and push it to the push stream. 
    // Notice: we skip 44 bytes for the typical wav header.
    fs.createReadStream(filename, { start: 44}).on('data', function(arrayBuffer) {
        pushStream.write(arrayBuffer.slice());
    }).on('end', function() {
        pushStream.close();
    });

    return pushStream;
};

export const readWavFileHeader = (audioFileName) => {
    var fd = fs.openSync(audioFileName, 'r');

    if(ReadString(fd, 4) != "RIFF") {
        throw "Error reading .wav file header. Expected 'RIFF' tag.";
    }
    // File length
    ReadInt32(fd);
    if(ReadString(fd, 4) != "WAVE") {
        throw "Error reading .wav file header. Expected 'WAVE' tag.";
    }
    if(ReadString(fd, 4) != "fmt ") {
        throw "Error reading .wav file header. Expected 'fmt ' tag.";
    }
    // Format size
    var formatSize = ReadInt32(fd);
    if (formatSize > 16) {
        throw "Error reading .wav file header. Expected format size 16 bytes. Actual size: " + String(formatSize);
    }
    // Format tag
    var tag = ReadUInt16(fd);
    var nChannels = ReadUInt16(fd);
    var framerate = ReadUInt32(fd);
    // Average bytes per second
    ReadUInt32(fd);
    // Block align
    ReadUInt16(fd);
    var bitsPerSample = ReadUInt16(fd);

    fs.closeSync(fd);

    return { framerate : framerate, bitsPerSample : bitsPerSample, nChannels : nChannels, tag: tag };
};
