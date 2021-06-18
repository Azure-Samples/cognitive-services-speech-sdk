//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    public abstract class ScopedLocalizedDtoBase : LocalizedDtoBase
    {
        public EntityReference Project { get; set; }
    }
}
