// <copyright file="Summary.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;

    using Newtonsoft.Json;

    public class Summary
    {
        [JsonProperty("issues")]
        public IEnumerable<string> Issues { get; set; }
    }
}
