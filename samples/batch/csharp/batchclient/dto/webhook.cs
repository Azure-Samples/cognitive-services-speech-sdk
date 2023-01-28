//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace BatchClient
{
    using System;
    using System.Collections.Generic;

    public class WebHook : DtoBase
    {
        public WebHookLinks Links { get; set; }

        public WebHookProperties Properties { get; set; }

        public Uri WebUrl { get; set; }

        public IReadOnlyDictionary<WebHookEventKind, bool> Events { get; set; }
    }
}
