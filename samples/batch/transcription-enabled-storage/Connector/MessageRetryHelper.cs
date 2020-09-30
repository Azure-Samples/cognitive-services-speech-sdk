// <copyright file="MessageRetryHelper.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    public static class MessageRetryHelper
    {
        public static TimeSpan GetMessageRetryDelayTime(int retryCount)
        {
            var delayInMinutes = Math.Pow(2, Math.Min(retryCount, 8));
            return TimeSpan.FromMinutes(delayInMinutes);
        }
    }
}
