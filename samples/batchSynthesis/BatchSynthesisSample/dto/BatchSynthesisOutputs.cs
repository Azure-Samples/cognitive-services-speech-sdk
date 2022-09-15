//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class BatchSynthesisOutputs
{
    /// <summary>
    /// Gets or sets a sas url of the result file.
    /// It would be relative path when "destinationContainerUrl" is specified.
    /// </summary>
    public string Result { get; set; }

    /// <summary>
    /// Gets or sets a sas url of the summary file.
    /// It would be relative path when "destinationContainerUrl" is specified.
    /// </summary>
    public string Summary { get; set; }
}
