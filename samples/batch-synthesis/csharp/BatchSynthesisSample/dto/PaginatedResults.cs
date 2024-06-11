//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Text.Json.Serialization;

public class PaginatedResults<T>
{
    /// <summary>
    /// The a list of entities limited by either the passed query parameters 'skip' and 'top' or their default values.
    /// </summary>
    public IEnumerable<T> Value { get; set; }

    /// <summary>
    /// The a link to the next set of paginated results if there are more entities available; otherwise <c>null</c>.
    /// </summary>
    public Uri NextLink { get; set; }
}
