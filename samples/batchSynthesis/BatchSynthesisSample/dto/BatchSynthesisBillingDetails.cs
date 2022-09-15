//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class BatchSynthesisBillingDetails
{
    /// <summary>
    /// Gets or sets the number of billable characters for custom neural voices in the input file.
    /// </summary>
    public long CustomNeural { get; set; }

    /// <summary>
    /// Gets or sets the number of billable characters for prebuild neural voices in the input file.
    /// </summary>
    public long Neural { get; set; }
}
