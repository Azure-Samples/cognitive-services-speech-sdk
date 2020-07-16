//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Collections.Generic;

namespace BatchClient
{
    public class Model : ScopedLocalizedDtoBase
    {
        public EntityReference BaseModel { get; set; }

        public IEnumerable<EntityReference> Datasets { get; set; }

        public string Text { get; private set; }

        public ModelLinks Links { get; private set; }

        public ModelProperties Properties { get; set; }
    }
}
