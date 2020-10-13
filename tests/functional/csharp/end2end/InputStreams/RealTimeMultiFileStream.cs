//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Tests.EndToEnd.Utils;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    using static SPXTEST;

    public class RealTimeMultiFileStream : PullAudioInputStreamCallback
    {
        // Default behavior for new streams. true, if realtime,
        // false, if best effort.
        public static bool defaultIsRealtime = false;

        AudioDataStream stream;
        DateTime notBefore;
        bool currentIsRealtime;
        string[] fileNames;
        int currentFile;
        object fsLock = new object();

        public RealTimeMultiFileStream(params string[] fileNames)
        {
            Console.WriteLine("Trying to open " + fileNames[0]);
            currentFile = 0;
            this.fileNames = fileNames;
            stream = AudioDataStream.FromWavFileInput(fileNames[0]);
            notBefore = DateTime.Now;
            currentIsRealtime = defaultIsRealtime;
        }

        public override int Read(byte[] dataBuffer, uint size)
        {
            var now = DateTime.Now;

            // calculates the deadline when the next data should arrive assuming
            // a stream of mono/16bits/16000hz audio input.
            var newNotBefore = notBefore.AddMilliseconds((1000 * size) / 32000);

            if (currentIsRealtime)
            {
                var delay = notBefore - now;
                if (delay > TimeSpan.Zero)
                {
                    Thread.Sleep((int)delay.TotalMilliseconds);
                }
            }

            notBefore = newNotBefore;

            lock (fsLock)
            {
                // If there isn't a current file to read from, be done.
                if (stream != null)
                {
                    SPXTEST_ISTRUE(dataBuffer.Length == size, "AudioDataStream should really allow specifying size to read; in this use case this is fine.");
                    var read = stream.ReadData(dataBuffer);
                }
                else
                {
                    return 0;
                }
            }

            // Just return the entire buffer. We either got enough from the file,
            // or are about to send silence.
            return dataBuffer.Length;

        }

        public void NextFile()
        {
            lock (fsLock)
            {
                stream.Dispose();
                stream = null;

                // If there are more files to read from, move to the next.
                if (++currentFile < fileNames.Length)
                {
                    stream = AudioDataStream.FromWavFileInput(fileNames[currentFile]);
                }
            }
        }
    }

}
