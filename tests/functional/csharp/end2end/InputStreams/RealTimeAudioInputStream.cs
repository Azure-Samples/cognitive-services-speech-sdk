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

    // provides an input stream that does playback a file in "realtime", i.e.,
    // returns 32000bytes/sec assuming our standard mono/16bits/16000hz
    // wav file format.
    // Note: the behaviour can be configured per instance and has a default of "false"
    public class RealTimeAudioInputStream : PullAudioInputStreamCallback
    {
        // Default behavior for new streams. true, if realtime,
        // false, if best effort.
        public static bool defaultIsRealtime = false;

        AudioDataStream stream;
        DateTime notBefore;
        bool currentIsRealtime;

        public RealTimeAudioInputStream(string filename)
        {
            stream = AudioDataStream.FromWavFileInput(filename);
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
            return (int)stream.ReadData(dataBuffer);
        }
    }
}
