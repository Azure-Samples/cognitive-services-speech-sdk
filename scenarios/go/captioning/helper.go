//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package main

import(
    "bufio"
    "fmt"
    "github.com/Microsoft/cognitive-services-speech-sdk-go/common"
    "github.com/Microsoft/cognitive-services-speech-sdk-go/audio"
    "io"
    "log"
    "os"
)

type BinaryFileReaderCallback struct{
    file *os.File
    reader *bufio.Reader
}

func GetBinaryFileReaderCallback(filename string) *BinaryFileReaderCallback {
    file, err := os.Open(filename)
    if nil != err {
        log.Fatal(err)
    }
    return &BinaryFileReaderCallback{file: file, reader: bufio.NewReader(file)}
}

func (this *BinaryFileReaderCallback) Read(maxSize uint32) ([]byte, int) {
    buffer := make([]byte, maxSize)
    n, err := this.reader.Read(buffer)
    if err == io.EOF {
        return buffer, n
    }
    if nil != err {
        log.Fatal(err)
    }
    return buffer, n
}

// TODO1 FIX? It keeps getting properties 11001 and 11002.
func (this *BinaryFileReaderCallback) GetProperty(id common.PropertyID) string {
// TODO1 TEMP
//    print ("GetProperty: ")
//    print (id)
//    print ("\n")
    return ""
}

func (this *BinaryFileReaderCallback) CloseStream() {
    defer this.file.Close()
}

func ReadWavFileHeader(filename string) (int, int, int) {

    return 0, 0, 0
}

type UserConfig struct { 
    compressedAudioFormat audio.AudioStreamContainerFormat
    removeProfanity bool
    disableMaskingProfanity bool
    inputFile *string
    outputFile *string
    suppressConsoleOutput bool
    showRecognizingResults bool
    stablePartialResultThreshold *string
    useSubRipTextCaptionFormat bool
    subscriptionKey string
    region string
}

type Timestamp struct {
    startHours int
    endHours int
    startMinutes int
    endMinutes int
    startSeconds int
    endSeconds int
    startMilliseconds int
    endMilliseconds int
}

var newline = "\n"

// Note: Type parameters require Go version 1.18 beta 1 or later.
func IndexOf(xs []string, x string)(bool, int) {
    for i, j := range xs {
        if j == x {
            return true, i
        }
    }
    return false, -1
}

func GetCmdOption(args []string, option string) *string {
    var result, index = IndexOf(args, option)
    if result && index < len(args) - 2 {
        // We found the option (for example, "-o"), so advance from that to the value (for example, "filename").
        return &args[index + 1]
    } else {
        return nil
    }
}

func CmdOptionExists(args []string, option string) bool {
    var result, _ = IndexOf(args, option)
    return result
}

func WriteToConsole(text string, userConfig UserConfig) {
    if !userConfig.suppressConsoleOutput {
        fmt.Print(text)
    }
}

func WriteToConsoleOrFile(text string, userConfig UserConfig) {
    WriteToConsole(text, userConfig)
    if nil != userConfig.outputFile {
        file, err := os.OpenFile(*userConfig.outputFile, os.O_CREATE|os.O_APPEND|os.O_WRONLY, 0644)
        if nil != err {
            log.Fatal(err)
        }
        defer file.Close()
        _, err = file.WriteString(text)
        if nil != err {
            log.Fatal(err)
        }        
    }
}
