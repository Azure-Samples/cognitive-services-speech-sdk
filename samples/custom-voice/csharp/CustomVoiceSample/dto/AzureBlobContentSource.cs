//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class AzureBlobContentSource
{
    public Uri ContainerUrl { get; set; }

    public string Prefix { get; set; }

    public IEnumerable<string> Extensions { get; set; }
}
