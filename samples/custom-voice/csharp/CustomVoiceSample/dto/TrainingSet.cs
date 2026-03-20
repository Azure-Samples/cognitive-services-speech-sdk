//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class TrainingSetProperties
{
    public int? UtteranceCount { get; set; }

    public double? DurationInSeconds { get; set; }

    public bool? IsContextual { get; set; }
}

public class TrainingSet
{
    public string Id { get; set; }

    public string Description { get; set; }

    public string Locale { get; set; }

    public string ProjectId { get; set; }

    public TrainingSetProperties Properties { get; set; }

    public DateTime CreatedDateTime { get; set; }

    public DateTime LastActionDateTime { get; set; }

    public Status? Status { get; set; }
}
