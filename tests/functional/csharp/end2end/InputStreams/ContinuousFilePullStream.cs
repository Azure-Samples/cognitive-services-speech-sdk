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

    internal class ContinuousFilePullStream : PullAudioInputStreamCallback
    {
        FileStream fs;
        readonly object fsLock = new object();

        public ContinuousFilePullStream(string fileName)
        {
            Console.WriteLine("Trying to open " + fileName);
            fs = File.OpenRead(fileName);
        }

        public override int Read(byte[] dataBuffer, uint size)
        {
            lock (fsLock)
            {
                if (fs == null)
                {
                    return 0;
                }

                if (fs.Read(dataBuffer, 0, (int)size) < size)
                {
                    // reset the file stream.
                    fs.Seek(0, SeekOrigin.Begin);
                }
            }

            return dataBuffer.Length;
        }

        public override void Close()
        {
            lock (fsLock)
            {
                fs.Dispose();
                fs = null;
            }

            base.Close();
        }
    }

}
