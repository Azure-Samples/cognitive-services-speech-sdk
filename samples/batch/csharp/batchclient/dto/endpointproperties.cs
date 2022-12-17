//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;

    public class EndpointProperties
    {
        public TimeSpan TimeToLive { get; set; }

        public string Email { get; set; }

        public bool IsLoggingEnabled { get; set; }

        public EntityError Error { get; set; }
    }
}
