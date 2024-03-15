//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class ModelProperties
{
    public string FailureReason { get; set; }

    public IEnumerable<string> PresetStyles { get; set; }

    public IReadOnlyDictionary<string, string> StyleTrainingSetIds { get; set; }
}
