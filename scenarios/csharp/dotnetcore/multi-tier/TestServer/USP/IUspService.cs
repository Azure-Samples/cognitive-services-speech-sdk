//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Net.WebSockets;

namespace Azure.AI.Test.Common.Servers.USP
{
    public enum UspServiceState
    {
        Initial,
        Configuring,
        Reconnecting,
        Ready,
        InTurn,
        Failed,
        Closed
    }

    public interface IUspService : IDisposable
    {
        UspServiceState State { get; }

        event EventHandler<IUspMessageSerializable> ServiceReady;
        event EventHandler<IUspMessageSerializable> Turn;
        event EventHandler<IUspMessageSerializable> Insight;
        event EventHandler<CloseEventArgs> Closed;

        void MessageReceived(Message message);
        void CloseReceived(WebSocketCloseStatus status, string description);
    }
}
