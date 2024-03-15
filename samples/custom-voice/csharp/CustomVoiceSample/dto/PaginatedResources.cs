//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class PaginatedResources<T>
{
    public IEnumerable<T> Value { get; set; }

    public Uri NextLink { get; set; }
}
