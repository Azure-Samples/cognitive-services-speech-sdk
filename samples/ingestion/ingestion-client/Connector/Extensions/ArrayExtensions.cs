// <copyright file="ArrayExtensions.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Extensions
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    public static class ArrayExtensions
    {
        public static void RedactFrames(this float[] frames, long startFrameIndex, int durationInFrames, int channel, int totalChannels)
        {
            frames = frames ?? throw new ArgumentNullException(nameof(frames));

            var shifted = (startFrameIndex * totalChannels) + channel;
            for (int i = 0; i < durationInFrames; i += totalChannels)
            {
                frames[shifted + i] = 0;
            }
        }
    }
}
