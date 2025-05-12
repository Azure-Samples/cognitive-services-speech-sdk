//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Net.WebSockets;

namespace Azure.AI.Test.Common.Servers.USP
{
    public class CloseEventArgs : EventArgs
    {
        public CloseEventArgs(WebSocketCloseStatus status, string description = null)
        {
            this.Status = status;
            this.Description = description ?? string.Empty;
        }

        public WebSocketCloseStatus Status { get; }
        public string Description { get; }
    }
}
