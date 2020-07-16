using System;
using System.Collections.Generic;

namespace BatchClient
{
    public class WebHookUpdate : DtoUpdateBase
    {
        public WebHookPropertiesUpdate Properties { get; private set; }

        public IReadOnlyDictionary<WebHookEventKind, bool> Events { get; private set; }

        public Uri WebUrl { get; private set; }

    }
}
