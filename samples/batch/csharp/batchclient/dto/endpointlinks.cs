//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;

    public class EndpointLinks
    {
        public Uri Logs { get; set; }

        public Uri RestInteractive { get; set; }

        public Uri RestConversation { get; set; }

        public Uri RestDictation { get; set; }

        public Uri WebSocketInteractive { get; set; }

        public Uri WebSocketConversation { get; set; }

        public Uri WebSocketDictation { get; set; }
    }
}
