//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    public sealed class Endpoint : ScopedLocalizedDtoBase
    {
        public EntityReference Model { get; set; }

        public EndpointLinks Links { get; set; }

        public string Text { get; set; }

        public EndpointProperties Properties { get; set; }
    }
}

