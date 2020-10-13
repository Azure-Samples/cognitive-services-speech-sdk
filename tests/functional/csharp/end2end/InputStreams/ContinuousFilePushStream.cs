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

    internal class ContinuousFilePushStream
    {
        AudioDataStream fs;
        private Task filePump;
        private int bytesPerSecond = 16000 * 2 * 1;
        private TaskCompletionSource<bool> stopSource;

        private PushAudioInputStream ps;

        readonly object fsLock = new object();

        public ContinuousFilePushStream(PushAudioInputStream ps, string fileName)
        {
            Console.WriteLine("Trying to open " + fileName);
            fs = AudioDataStream.FromWavFileInput(fileName);
            filePump = null;
            this.ps = ps;
        }

        public void Start()
        {
            if (null != filePump)
            {
                throw new InvalidOperationException("Pump already running");
            }

            stopSource = new TaskCompletionSource<bool>();
            
            uint read = 0;

            filePump = Task.Run(async () =>
            {
                byte[] buffer = new byte[bytesPerSecond / 10];

                do
                {
                    if (0 == (read = fs.ReadData(buffer)))
                    {
                        fs.SetPosition(0);
                        read = fs.ReadData(buffer);
                    }
                    var t = (DateTime.UtcNow - DateTime.MinValue).TotalMilliseconds.ToString();
                    ps.SetProperty("timestamp", t);
                    ps.SetProperty("speakerid", $"SpeakerID{t}");
                    ps.SetProperty("timestamp", t);
                    ps.SetProperty("speakerid", $"SpeakerID{t}");
                    ps.Write(buffer, Convert.ToInt32(read));

                } while (await Task.WhenAny(Task.Delay(TimeSpan.FromSeconds(Convert.ToDouble(read) / Convert.ToDouble(bytesPerSecond))), stopSource.Task) != stopSource.Task);

            });
        }

        public async Task Stop()
        {
            stopSource.TrySetResult(true);
            await filePump;
            filePump = null;
            ps.Write(Array.Empty<byte>());
        }

        public async Task Pause()
        {
            stopSource.TrySetResult(true);
            await filePump;
        }

        public void Close()
        {
            ps.Close();
        }
    }

}
