//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// <csharp-enumerate-audio-device>
using System;
using System.Collections.Generic;
using NAudio.CoreAudioApi;

namespace ConsoleApp
{
    class Program
    {
        static void Main()
        {
            var enumerator = new MMDeviceEnumerator();
            var dataFlow = new List<DataFlow>() { DataFlow.Capture, DataFlow.Render };

            foreach (var flow in dataFlow)
            { 
                foreach (var endpoint in
                         enumerator.EnumerateAudioEndPoints(flow, DeviceState.Active))
                {
                    Console.WriteLine("{0} endpoint: {1} ({2})", (flow == DataFlow.Capture ? "Capture" : "Render"), endpoint.FriendlyName, endpoint.ID);
                }
            }
        }
    }
}
// </csharp-enumerate-audio-device>
