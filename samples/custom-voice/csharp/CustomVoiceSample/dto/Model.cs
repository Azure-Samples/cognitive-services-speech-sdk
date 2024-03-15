//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

public class Model
{
    public string Id { get; set; }

    public string VoiceName { get; set; }

    public string Description { get; set; }

    public Recipe Recipe { get; set; }

    public string Locale { get; set; }

    public string TrainingSetId { get; set; }

    public string ProjectId { get; set; }

    public string ConsentId { get; set; }

    public ModelProperties Properties { get; set; }

    public string EngineVersion { get; set; }

    public DateTime CreatedDateTime { get; set; }

    public DateTime LastActionDateTime { get; set; }

    public Status? Status { get; set; }
}
